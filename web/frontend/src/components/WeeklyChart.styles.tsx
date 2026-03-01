import type { ReactNode } from 'react';
import type { ChartMetric } from '../types/monitoring';

export const EMPTY_STATE_CLASS =
  'rounded-2xl border border-[var(--line)] bg-[var(--soft)] p-4 text-sm text-[var(--text-soft)]';
export const CHART_WRAP_CLASS = 'rounded-2xl border border-[var(--line)] bg-[var(--soft)] p-4 md:p-5';
export const HISTORY_HINT_CLASS =
  'mb-2 text-[10px] font-semibold uppercase tracking-[0.08em] text-[var(--text-soft)] sm:text-xs';
export const METRIC_SWITCH_CLASS = 'mb-3 grid-cols-3 text-xs sm:text-sm';
export const METRIC_SWITCH_ITEM_CLASS = 'px-2 py-1.5 sm:px-6';
export const CHART_SVG_CLASS = 'h-auto w-full';
export const GRID_LINE_STROKE = '#cfd7e3';
export const AXIS_LABEL_FONT_SIZE = 12;
export const AXIS_LABEL_FONT_WEIGHT = 600;
export const DAY_LABEL_FILL = '#6b7688';
export const AXIS_LABEL_X = 8;
export const DAY_LABEL_Y_OFFSET = 8;

export const CHART_METRIC_OPTIONS: { value: ChartMetric; label: ReactNode }[] = [
  {
    value: 'temperatureC',
    label: (
      <>
        <span className="sm:hidden">Temp</span>
        <span className="hidden sm:inline">Temperature</span>
      </>
    ),
  },
  {
    value: 'humidity',
    label: (
      <>
        <span className="sm:hidden">Hum</span>
        <span className="hidden sm:inline">Humidity</span>
      </>
    ),
  },
  { value: 'lightPercent', label: 'Light' },
];
