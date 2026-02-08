'use strict'
import express from "express";
import cors from "cors";

const app = express();

app.use(cors());
app.use(express.json());

app.get("/health", (_req, res) => {
  res.json({ ok: true, service: "backend", ts: Date.now() });
});




// test change for triger github ci 
app.get("/latest", (_req, res) => {
  res.json({
    humidity: 0,
    temperatureC: 0,
    temperatureF: 0,
    photores: 0,
    ts: Date.now(),
  });
});

const port = Number(process.env.PORT ?? 3001);

app.listen(port, () => {
  console.log(`Backend listening on http://localhost:${port}`);
});
