export type DeviceStatus = 'online' | 'offline';

export type DeviceReading = {
  id: string;
  name: string;
  status: DeviceStatus;
  updatedAt: string;
  temperatureC: number;
  humidity: number;
  photoResistorOhm: number;
};

export type WeeklyPoint = {
  label: string;
  temperatureC: number;
  humidity: number;
  lux: number;
};

export type PanelMode = 'normal' | 'table';
export type DetailMode = 'now' | 'chart';
export type ChartMetric = 'temperatureC' | 'humidity' | 'lux';
