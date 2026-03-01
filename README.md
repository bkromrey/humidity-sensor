# CS 467 - Online Capstone Project (Winter 2026)

Project: Microcontroller Home Humidity Sensor

Team: Brianna Kromrey, John David Lopez, Eugenia Uvarov

## Repository Structure

```bash
.
├── README.md
├── embedded
│   ├── README.md
│   ├── Wiring_Schematic.png
│   └── src
│       ├── CMakeLists.txt
│       ├── main.c
│       ├── hardware/    # buttons, LED array drivers
│       ├── core1/       # core1 entry + logic
│       └── data_flow/   # shared data types
├── web
│   ├── frontend         # React + Vite + TypeScript
│   └── backend          # Node.js + Express (MongoDB later)
└── pico-sdk             # Raspberry Pi Pico SDK
```

## Hardware - Build Pico Code

Build the embedded firmware (Pico W)
Prerequisites

* cmake
* ninja
* ARM toolchain: arm-none-eabi-gcc (and related tools)

### Environment Variables

In order to utilize the networking functionality of this application (transmit
the sensor readings to a web application) you must first set the following
environment variables before building.

The `PICO_MQTT_SERVER` should be a standard IPv4 address belonging to the MQTT
server; no port number is necessary.

```sh
export PICO_MQTT_SERVER="..."
export PICO_MQTT_USER="..."
export PICO_MQTT_PASS="..."
export PICO_WIFI_SSID="..."
export PICO_WIFI_PASS="..."
export PICO_SENSOR_ID="..."
```

### Build steps

From the repository root:

```sh
mkdir -p build_ninja
cmake -S . -B build_ninja -G Ninja
cmake --build build_ninja
```

Linux build instructions - within `embedded` directory:

```sh
cd embedded
mkdir build-local && cd build-local
cmake ../src
make
```

## Web Application - Frontend (React + TypeScript + Vite + TailvindCSS)

UI for the Humidity Sensor project.

### Requirements

* Node.js 20+

### Install

cd web/frontend
npm install

run dev verson:
`npm run dev`

production
`npm run build`

and preview:
`npm run preview`

Lint:
`npm run lint`

Styling:
This project uses Tailwind CSS v4.
Tailwind is enabled via the Vite plugin (@tailwindcss/vite)
Global styles are in src/index.css (@import "tailwindcss";)

## Web Application - Backend (Node + Express + TypeScript)

API server for the Humidity Sensor project.

### Requirements

* Node.js 20+

### Install

cd web/backend
npm install

### Backend environment variables

```sh
cd web/backend
cp .env.example .env
```

For MongoDB Atlas, you can also start from:

```sh
cd web/backend
cp .env.atlas.example .env
```

### Backend run

```sh
cd web/backend
npm run dev
```

Edit `web/backend/.env` and set these values:

* `PORT`
* `MONGODB_URI`
* `MONGODB_DB_NAME`
* `MQTT_URL`
* `MQTT_USERNAME`
* `MQTT_PASSWORD`
* `MQTT_CLIENT_ID`
* `HISTORY_INTERVAL_MINUTES`

### MongoDB Atlas setup (cloud)

Driver:

```sh
npm install mongodb
```

Atlas connection string for this project:

```sh
mongodb+srv://uvarove_db_user:<db_password>@humidity-sensor-monitor.mcieuu7.mongodb.net/?appName=humidity-sensor-monitoring
```

Replace `<db_password>` with the password for `uvarove_db_user`.

1. Create a cluster in Atlas:
   1. Open MongoDB Atlas and create a project/cluster.
   2. Wait until cluster status is ready.
2. Create a database user:
   1. Atlas -> `Database Access` -> `Add New Database User`.
   2. Give `Read and write to any database` (or minimum required role).
   3. Save username/password.
3. Configure network access:
   1. Atlas -> `Network Access` -> `Add IP Address`.
   2. Add your backend host IP (or temporarily `0.0.0.0/0` for development).
4. Get connection string:
   1. Atlas -> cluster -> `Connect` -> `Drivers`.
   2. Copy `mongodb+srv://...` URI.
   3. Replace `<db_user>` and `<db_password>`.
5. Put values in backend `.env`:
   1. `MONGODB_URI=<your mongodb+srv uri>`
   2. `MONGODB_DB_NAME=humidity_sensor` (or your DB name)
6. Run backend:

```sh
cd web/backend
npm run dev
```

### MQTT topic and payload contract

Backend subscribes to both topic styles:

* `sensors/<sensor_id>/raw`
* `<sensor_id>`

Expected JSON payload example:

```json
{
  "temperature_f": "72.5",
  "temperature_c": "22.5",
  "humidity": "45.3",
  "light": "621",
  "ts": "2026-02-26T21:25:00Z"
}
```

### Data model behavior

* `sensor_latest`: updated for every incoming MQTT message.
* `sensor_history`: stores snapshots no more than once every 30 minutes per sensor.
* All source-of-truth timestamps are UTC in DB and API responses.
* User-local display time should be formatted on the frontend.

### API and realtime endpoints

* `GET /health`
* `GET /api/sensors/latest`
* `GET /api/sensors/:sensorId/history?from=<iso>&to=<iso>`
* `GET /latest` (backward-compatible current frontend endpoint)
* `WS /ws/sensors` (pushes latest sensor events in realtime)
