import type { DeviceReading, WeeklyPoint } from '../types/monitoring';

export const MOCK_DEVICES: DeviceReading[] = [
  {
    id: 'pico1',
    name: 'Pico #1',
    status: 'online',
    updatedAt: '11:44',
    temperatureC: 23.4,
    humidity: 45,
    photoResistorOhm: 1300,
  },
  {
    id: 'pico2',
    name: 'Pico #2',
    status: 'offline',
    updatedAt: '11:42',
    temperatureC: 22.0,
    humidity: 50,
    photoResistorOhm: 1600,
  },
  {
    id: 'pico3',
    name: 'Pico #3',
    status: 'online',
    updatedAt: '11:43',
    temperatureC: 24.1,
    humidity: 40,
    photoResistorOhm: 900,
  },
];

export const DEVICE_WEEKLY_HISTORY: Record<string, WeeklyPoint[]> = {
  pico1: [
    { label: 'Mon', temperatureC: 22.8, humidity: 49, lux: 280 },
    { label: 'Tue', temperatureC: 23.1, humidity: 47, lux: 300 },
    { label: 'Wed', temperatureC: 23.4, humidity: 46, lux: 320 },
    { label: 'Thu', temperatureC: 23.0, humidity: 48, lux: 295 },
    { label: 'Fri', temperatureC: 23.2, humidity: 45, lux: 335 },
    { label: 'Sat', temperatureC: 23.5, humidity: 44, lux: 360 },
    { label: 'Sun', temperatureC: 23.4, humidity: 45, lux: 341 },
  ],
  pico2: [
    { label: 'Mon', temperatureC: 21.6, humidity: 52, lux: 240 },
    { label: 'Tue', temperatureC: 21.8, humidity: 51, lux: 250 },
    { label: 'Wed', temperatureC: 22.0, humidity: 50, lux: 270 },
    { label: 'Thu', temperatureC: 21.9, humidity: 51, lux: 260 },
    { label: 'Fri', temperatureC: 22.1, humidity: 50, lux: 275 },
    { label: 'Sat', temperatureC: 22.0, humidity: 49, lux: 285 },
    { label: 'Sun', temperatureC: 22.0, humidity: 50, lux: 288 },
  ],
  pico3: [
    { label: 'Mon', temperatureC: 23.6, humidity: 43, lux: 350 },
    { label: 'Tue', temperatureC: 23.9, humidity: 42, lux: 370 },
    { label: 'Wed', temperatureC: 24.0, humidity: 41, lux: 390 },
    { label: 'Thu', temperatureC: 24.2, humidity: 40, lux: 420 },
    { label: 'Fri', temperatureC: 24.1, humidity: 39, lux: 410 },
    { label: 'Sat', temperatureC: 24.0, humidity: 40, lux: 400 },
    { label: 'Sun', temperatureC: 24.1, humidity: 40, lux: 412 },
  ],
};
