import type { TemperatureUnit } from '../types/monitoring';

export const temperatureByUnit = (valueC: number, valueF: number, unit: TemperatureUnit) =>
  unit === 'fahrenheit' ? valueF : valueC;
export const temperatureUnitSymbol = (unit: TemperatureUnit) =>
  unit === 'fahrenheit' ? '°F' : '°C';
