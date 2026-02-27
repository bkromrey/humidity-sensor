export const toFahrenheit = (valueC: number) => (valueC * 9) / 5 + 32;

export const resistanceToLux = (ohm: number) => {
  const clamped = Math.max(400, Math.min(3000, ohm));
  const normalized = 1 - (clamped - 400) / (3000 - 400);
  return Math.round(40 + normalized * 460);
};

export const luxToResistance = (lux: number) => {
  const clamped = Math.max(40, Math.min(500, lux));
  const normalized = (clamped - 40) / 460;
  return Math.round(400 + (1 - normalized) * (3000 - 400));
};
