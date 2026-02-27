import type { CSSProperties } from 'react';

type MetricCardVariant = 'temp' | 'humidity' | 'light';

export const VARIANT_STYLES: Record<MetricCardVariant, CSSProperties> = {
  temp: { background: 'var(--kpi-temp-bg)', color: 'var(--kpi-temp-text)' },
  humidity: { background: 'var(--kpi-hum-bg)', color: 'var(--kpi-hum-text)' },
  light: { background: 'var(--kpi-light-bg)', color: 'var(--kpi-light-text)' },
};

export const CARD_CLASS = 'rounded-2xl p-5 text-center shadow-lg';
export const VALUE_CLASS =
  'whitespace-nowrap text-[clamp(1.2rem,2.1vw,2.2rem)] font-semibold leading-none tracking-tight';
export const SUBVALUE_CLASS = 'mt-1 text-xs opacity-90 sm:text-sm';
export const LABEL_CLASS = 'mt-3 text-xs uppercase tracking-[0.18em] sm:text-sm sm:tracking-[0.2em]';
