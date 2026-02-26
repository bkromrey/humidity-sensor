import mqtt, { MqttClient } from "mqtt";
import { DbCollections } from "./db";
import { config } from "./config";
import { toLocalIsoString } from "./time";
import { SensorHistoryDocument, SensorLatestDocument, SensorReading } from "./types";
import { RealtimeHub } from "./realtime";

function parseNumber(value: unknown): number | null {
  if (typeof value === "number" && Number.isFinite(value)) return value;
  if (typeof value === "string" && value.trim() !== "") {
    const parsed = Number(value);
    if (Number.isFinite(parsed)) return parsed;
  }
  return null;
}

function inferSensorId(topic: string): string {
  const split = topic.split("/");
  if (split.length === 3 && split[0] === "sensors" && split[2] === "raw") {
    return split[1];
  }
  return topic;
}

function parseReading(topic: string, message: Buffer): SensorReading | null {
  let parsedPayload: unknown;
  try {
    parsedPayload = JSON.parse(message.toString("utf8"));
  } catch {
    return null;
  }

  const sensorId = inferSensorId(topic);
  const backendTimezone = config.backendTimezone;

  const body = parsedPayload as Record<string, unknown>;
  const receivedAtUtc = new Date();
  const receivedAtLocal = toLocalIsoString(receivedAtUtc, backendTimezone);

  return {
    sensorId,
    topic,
    receivedAtUtc,
    receivedAtLocal,
    backendTimezone,
    sourceTimestamp: typeof body.ts === "string" ? body.ts : null,
    temperatureC: parseNumber(body.temperature_c),
    temperatureF: parseNumber(body.temperature_f),
    humidity: parseNumber(body.humidity),
    light: parseNumber(body.light),
    rawPayload: body,
  };
}

async function persistReading(collections: DbCollections, reading: SensorReading): Promise<void> {
  const now = reading.receivedAtUtc;
  const historyWindowMs = config.historyIntervalMinutes * 60 * 1000;

  const previous = await collections.sensorLatest.findOne(
    { _id: reading.sensorId },
    { projection: { lastHistoryWriteAtUtc: 1 } }
  );

  await collections.sensorLatest.updateOne(
    { _id: reading.sensorId },
    {
      $set: {
        ...reading,
        updatedAtUtc: now,
      } satisfies Omit<SensorLatestDocument, "_id">,
    },
    { upsert: true }
  );

  const shouldWriteHistory =
    !previous?.lastHistoryWriteAtUtc ||
    now.getTime() - previous.lastHistoryWriteAtUtc.getTime() >= historyWindowMs;

  if (!shouldWriteHistory) return;

  await collections.sensorHistory.insertOne({
    ...reading,
    createdAtUtc: now,
  } satisfies SensorHistoryDocument);

  await collections.sensorLatest.updateOne(
    { _id: reading.sensorId },
    { $set: { lastHistoryWriteAtUtc: now } }
  );
}

export function startMqttIngest(collections: DbCollections, realtimeHub: RealtimeHub): MqttClient {
  const client = mqtt.connect(config.mqttUrl, {
    username: config.mqttUsername,
    password: config.mqttPassword,
    clientId: config.mqttClientId,
  });

  client.on("connect", () => {
    const topics = ["sensors/+/raw", "+"];
    client.subscribe(topics, { qos: 0 }, (err) => {
      if (err) {
        console.error("MQTT subscribe failed:", err.message);
        return;
      }
      console.log("MQTT subscribed to:", topics.join(", "));
    });
  });

  client.on("message", async (topic, message) => {
    const reading = parseReading(topic, message);
    if (!reading) return;
    try {
      await persistReading(collections, reading);
      realtimeHub.broadcastReading(reading);
    } catch (err) {
      console.error("Failed to process MQTT reading:", err);
    }
  });

  client.on("error", (err) => {
    console.error("MQTT client error:", err.message);
  });

  return client;
}
