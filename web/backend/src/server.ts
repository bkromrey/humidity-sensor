import "dotenv/config";
import { createServer } from "http";
import cors from "cors";
import express from "express";
import { config } from "./config";
import { closeDb, connectDb } from "./db";
import { createRealtimeHub } from "./realtime";
import { startMqttIngest } from "./mqttIngest";

async function main(): Promise<void> {
  const collections = await connectDb();

  const app = express();
  app.use(cors());
  app.use(express.json());

  app.get("/health", (_req, res) => {
    res.json({ ok: true, service: "backend", ts: Date.now() });
  });

  app.get("/api/sensors/latest", async (_req, res) => {
    const docs = await collections.sensorLatest
      .find({}, { projection: { rawPayload: 0 } })
      .sort({ _id: 1 })
      .toArray();
    res.json(docs);
  });

  app.get("/api/sensors/:sensorId/history", async (req, res) => {
    const sensorId = req.params.sensorId;
    const fromRaw = typeof req.query.from === "string" ? req.query.from : undefined;
    const toRaw = typeof req.query.to === "string" ? req.query.to : undefined;

    const from = fromRaw ? new Date(fromRaw) : undefined;
    const to = toRaw ? new Date(toRaw) : undefined;
    if ((from && Number.isNaN(from.getTime())) || (to && Number.isNaN(to.getTime()))) {
      res.status(400).json({ ok: false, error: "Invalid from/to datetime" });
      return;
    }

    const filter: Record<string, unknown> = { sensorId };
    if (from || to) {
      filter.receivedAtUtc = {
        ...(from ? { $gte: from } : {}),
        ...(to ? { $lte: to } : {}),
      };
    }

    const docs = await collections.sensorHistory
      .find(filter, { projection: { rawPayload: 0 } })
      .sort({ receivedAtUtc: -1 })
      .limit(5000)
      .toArray();

    res.json(docs);
  });

  // Backward-compatible endpoint used by current frontend.
  app.get("/latest", async (_req, res) => {
    const doc = await collections.sensorLatest
      .find({}, { projection: { rawPayload: 0 } })
      .sort({ updatedAtUtc: -1 })
      .limit(1)
      .next();
    if (!doc) {
      res.json({
        humidity: null,
        temperatureC: null,
        temperatureF: null,
        photores: null,
        ts: Date.now(),
      });
      return;
    }
    res.json({
      humidity: doc.humidity,
      temperatureC: doc.temperatureC,
      temperatureF: doc.temperatureF,
      photores: doc.light,
      ts: doc.receivedAtUtc.getTime(),
      sensorId: doc.sensorId,
      backendTimezone: doc.backendTimezone,
      backendLocalTs: doc.receivedAtLocal,
    });
  });

  const server = createServer(app);
  const realtimeHub = createRealtimeHub(server);
  const mqttClient = startMqttIngest(collections, realtimeHub);

  server.listen(config.port, () => {
    console.log(`Backend listening on http://localhost:${config.port}`);
  });

  const shutdown = async () => {
    console.log("Shutting down backend...");
    mqttClient.end(true);
    await closeDb();
    server.close();
  };

  process.on("SIGINT", () => {
    void shutdown();
  });
  process.on("SIGTERM", () => {
    void shutdown();
  });
}

void main().catch((err) => {
  console.error("Backend startup failed:", err);
  process.exit(1);
});
