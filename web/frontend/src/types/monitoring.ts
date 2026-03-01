export type DeviceStatus = 'online' | 'offline';

export type DeviceReading = {
  id: string;
  name: string;
  status: DeviceStatus;
  updatedAt: string;
  updatedAtFull: string;
  temperatureC: number;
  temperatureF: number;
  humidity: number;
  lightPercent: number;
};

export type WeeklyPoint = {
  ts: number;
  label: string;
  updatedAt: string;
  temperatureC: number;
  temperatureF: number;
  humidity: number;
  lightPercent: number;
};

export type PanelMode = 'normal' | 'table';
export type DetailMode = 'now' | 'chart';
export type ChartMetric = 'temperatureC' | 'humidity' | 'lightPercent';
export type TemperatureUnit = 'celsius' | 'fahrenheit';
export type HistoryRange = '1d' | '1w';
