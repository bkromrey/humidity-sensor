import { useMemo, useState } from 'react';
import { SegmentedControl } from './SegmentedControl';
import type { ChartMetric, WeeklyPoint } from '../types/monitoring';
import {
  AXIS_LABEL_FONT_SIZE,
  AXIS_LABEL_FONT_WEIGHT,
  AXIS_LABEL_X,
  CHART_METRIC_OPTIONS,
  CHART_SVG_CLASS,
  CHART_WRAP_CLASS,
  DAY_LABEL_FILL,
  DAY_LABEL_Y_OFFSET,
  EMPTY_STATE_CLASS,
  GRID_LINE_STROKE,
  METRIC_SWITCH_CLASS,
  METRIC_SWITCH_ITEM_CLASS,
} from './WeeklyChart.styles';

type WeeklyChartProps = {
  data: WeeklyPoint[];
};

const metricConfig = {
  temperatureC: { label: 'Temperature', color: '#f97316', unit: '°C', digits: 1 },
  humidity: { label: 'Humidity', color: '#3b82f6', unit: '%', digits: 0 },
  lux: { label: 'Light', color: '#f59e0b', unit: 'lx', digits: 0 },
} as const;

export function WeeklyChart({ data }: WeeklyChartProps) {
  const [selectedMetric, setSelectedMetric] = useState<ChartMetric>('temperatureC');
  const prepared = useMemo(() => buildChartData(data, selectedMetric), [data, selectedMetric]);

  if (data.length === 0) {
    return <div className={EMPTY_STATE_CLASS}>No chart data yet.</div>;
  }

  const activeMetric = metricConfig[selectedMetric];

  return (
    <div className={CHART_WRAP_CLASS}>
      <SegmentedControl
        value={selectedMetric}
        onChange={setSelectedMetric}
        options={CHART_METRIC_OPTIONS}
        className={METRIC_SWITCH_CLASS}
        itemClassName={METRIC_SWITCH_ITEM_CLASS}
      />

      <svg viewBox={`0 0 ${prepared.width} ${prepared.height}`} className={CHART_SVG_CLASS}>
        {[0, 1, 2, 3, 4].map((line) => {
          const y = prepared.paddingY + (prepared.innerHeight * line) / 4;
          const ratio = line / 4;
          const tickValue = prepared.max - (prepared.max - prepared.min) * ratio;

          return (
            <g key={line}>
              <line
                x1={prepared.paddingX}
                y1={y}
                x2={prepared.width - prepared.paddingX}
                y2={y}
                stroke={GRID_LINE_STROKE}
                strokeWidth="1"
                strokeDasharray="4 6"
              />
              <text
                x={AXIS_LABEL_X}
                y={y + 5}
                fontSize={AXIS_LABEL_FONT_SIZE}
                fontWeight={AXIS_LABEL_FONT_WEIGHT}
                fill={activeMetric.color}
              >
                {tickValue.toFixed(activeMetric.digits)}
                {activeMetric.unit}
              </text>
            </g>
          );
        })}

        <polyline
          points={prepared.path}
          fill="none"
          stroke={activeMetric.color}
          strokeWidth="3"
          strokeLinecap="round"
          strokeLinejoin="round"
        />

        {data.map((point, index) => {
          const x = prepared.paddingX + index * prepared.xStep;
          return (
            <text
              key={point.label}
              x={x}
              y={prepared.height - DAY_LABEL_Y_OFFSET}
              textAnchor="middle"
              fontSize={AXIS_LABEL_FONT_SIZE}
              fontWeight={AXIS_LABEL_FONT_WEIGHT}
              fill={DAY_LABEL_FILL}
            >
              {point.label}
            </text>
          );
        })}
      </svg>
    </div>
  );
}

function buildChartData(data: WeeklyPoint[], metric: ChartMetric) {
  const width = 760;
  const height = 280;
  const paddingX = 58;
  const paddingY = 26;
  const innerWidth = width - paddingX * 2;
  const innerHeight = height - paddingY * 2;
  const xStep = data.length > 1 ? innerWidth / (data.length - 1) : 0;

  const values = data.map((point) => point[metric]);
  const min = Math.min(...values);
  const max = Math.max(...values);
  const range = max - min || 1;

  const path = values
    .map((value, index) => {
      const x = paddingX + index * xStep;
      const y = paddingY + ((max - value) / range) * innerHeight;
      return `${x},${y}`;
    })
    .join(' ');

  return {
    width,
    height,
    paddingX,
    paddingY,
    innerHeight,
    xStep,
    min,
    max,
    path,
  };
}
