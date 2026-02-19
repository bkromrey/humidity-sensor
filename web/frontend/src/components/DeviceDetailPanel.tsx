import { SegmentedControl } from './SegmentedControl';
import { MetricCard } from './MetricCard';
import { WeeklyChart } from './WeeklyChart';
import type { DetailMode, DeviceReading, WeeklyPoint } from '../types/monitoring';
import { resistanceToLux, toFahrenheit } from '../utils/sensorMath';
import {
  DETAIL_MODE_ITEM_CLASS,
  DEVICE_META_CLASS,
  DEVICE_NAME_CLASS,
  METRIC_GRID_CLASS,
  PANEL_BODY_CLASS,
  PANEL_CLASS,
  PANEL_HEADER_CLASS,
  TWO_COL_SEGMENT_CLASS,
} from './DeviceDetailPanel.styles';

type DeviceDetailPanelProps = {
  device: DeviceReading;
  viewMode: DetailMode;
  weeklyData: WeeklyPoint[];
  onViewModeChange: (mode: DetailMode) => void;
};

const DETAIL_MODE_OPTIONS: { value: DetailMode; label: string }[] = [
  { value: 'now', label: 'Now' },
  { value: 'chart', label: 'Chart' },
];

export function DeviceDetailPanel({
  device,
  viewMode,
  weeklyData,
  onViewModeChange,
}: DeviceDetailPanelProps) {
  const selectedLux = resistanceToLux(device.photoResistorOhm);

  return (
    <article className={PANEL_CLASS}>
      <div className={PANEL_HEADER_CLASS}>
        <h2 className={DEVICE_NAME_CLASS}>{device.name}</h2>
        <p className={DEVICE_META_CLASS}>Pico • Updated {device.updatedAt}</p>
      </div>

      <div className={PANEL_BODY_CLASS}>
        <SegmentedControl
          value={viewMode}
          onChange={onViewModeChange}
          options={DETAIL_MODE_OPTIONS}
          className={TWO_COL_SEGMENT_CLASS}
          itemClassName={DETAIL_MODE_ITEM_CLASS}
        />

        {viewMode === 'now' ? (
          <div className={METRIC_GRID_CLASS}>
            <MetricCard
              variant="temp"
              value={`${device.temperatureC.toFixed(1)} °C`}
              subValue={`${toFahrenheit(device.temperatureC).toFixed(1)} °F`}
              label="Temperature"
            />
            <MetricCard
              variant="humidity"
              value={`${device.humidity}%`}
              subValue="Relative humidity"
              label="Humidity"
            />
            <MetricCard
              variant="light"
              value={`${selectedLux} lx`}
              subValue={`${device.photoResistorOhm} ohm`}
              label="Light"
            />
          </div>
        ) : (
          <WeeklyChart data={weeklyData} />
        )}
      </div>
    </article>
  );
}
