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
* `BACKEND_TIMEZONE`

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
* All source-of-truth timestamps are UTC in DB; backend also attaches local time based on `BACKEND_TIMEZONE`.

### API and realtime endpoints

* `GET /health`
* `GET /api/sensors/latest`
* `GET /api/sensors/:sensorId/history?from=<iso>&to=<iso>`
* `GET /latest` (backward-compatible current frontend endpoint)
* `WS /ws/sensors` (pushes latest sensor events in realtime)
