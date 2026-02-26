import { Server as HttpServer } from "http";
import { WebSocket, WebSocketServer } from "ws";
import { SensorReading } from "./types";

export type RealtimeHub = {
  broadcastReading: (reading: SensorReading) => void;
};

export function createRealtimeHub(server: HttpServer): RealtimeHub {
  const wss = new WebSocketServer({ server, path: "/ws/sensors" });

  wss.on("connection", (ws) => {
    ws.send(JSON.stringify({ type: "connected", ts: new Date().toISOString() }));
  });

  function broadcastReading(reading: SensorReading): void {
    const payload = JSON.stringify({
      type: "sensor_reading",
      data: {
        ...reading,
        receivedAtUtc: reading.receivedAtUtc.toISOString(),
      },
    });

    for (const client of wss.clients) {
      if (client.readyState === WebSocket.OPEN) client.send(payload);
    }
  }

  return { broadcastReading };
}
