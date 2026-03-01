export const config = {
  port: Number(process.env.PORT ?? 3001),
  mongoUri: process.env.MONGODB_URI ?? "mongodb://localhost:27017",
  mongoDbName: process.env.MONGODB_DB_NAME ?? "humidity_sensor",
  mqttUrl: process.env.MQTT_URL ?? "mqtt://localhost:1883",
  mqttUsername: process.env.MQTT_USERNAME,
  mqttPassword: process.env.MQTT_PASSWORD,
  mqttClientId: process.env.MQTT_CLIENT_ID ?? `humidity-backend-${process.pid}`,
  historyIntervalMinutes: Number(process.env.HISTORY_INTERVAL_MINUTES ?? 30),
};
