import { Collection, MongoClient } from "mongodb";
import { config } from "./config";
import { SensorHistoryDocument, SensorLatestDocument } from "./types";

let mongoClient: MongoClient | null = null;

export type DbCollections = {
  sensorLatest: Collection<SensorLatestDocument>;
  sensorHistory: Collection<SensorHistoryDocument>;
};

export async function connectDb(): Promise<DbCollections> {
  mongoClient = new MongoClient(config.mongoUri);
  await mongoClient.connect();

  const db = mongoClient.db(config.mongoDbName);
  const sensorLatest = db.collection<SensorLatestDocument>("sensor_latest");
  const sensorHistory = db.collection<SensorHistoryDocument>("sensor_history");

  // _id index exists by default on every Mongo collection.
  await sensorHistory.createIndex({ sensorId: 1, receivedAtUtc: -1 });

  return { sensorLatest, sensorHistory };
}

export async function closeDb(): Promise<void> {
  if (mongoClient) {
    await mongoClient.close();
    mongoClient = null;
  }
}
