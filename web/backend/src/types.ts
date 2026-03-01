export type SensorReading = {
  sensorId: string;
  topic: string;
  receivedAtUtc: Date;
  sourceTimestamp?: string | null;
  temperatureC: number | null;
  temperatureF: number | null;
  humidity: number | null;
  light: number | null;
  rawPayload: unknown;
};

export type SensorLatestDocument = SensorReading & {
  _id: string;
  updatedAtUtc: Date;
  lastHistoryWriteAtUtc?: Date;
};

export type SensorHistoryDocument = SensorReading & {
  createdAtUtc: Date;
};
