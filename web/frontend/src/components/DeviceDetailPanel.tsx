import { SegmentedControl } from './SegmentedControl';
import { MetricCard } from './MetricCard';
import { WeeklyChart } from './WeeklyChart';
import type { DetailMode, DeviceReading, TemperatureUnit, WeeklyPoint } from '../types/monitoring';
import { temperatureByUnit, temperatureUnitSymbol } from '../utils/sensorMath';
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
  // selected device data from sidebar
  device: DeviceReading;
  // current panel mode: now cards or chart
  viewMode: DetailMode;
  // weekly points for chart mode
  weeklyData: WeeklyPoint[];
  // change detail mode from segmented control
  onViewModeChange: (mode: DetailMode) => void;
  // selected global temperature unit
  temperatureUnit: TemperatureUnit;
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
  temperatureUnit,
}: DeviceDetailPanelProps) {
  const temperature = temperatureByUnit(device.temperatureC, device.temperatureF, temperatureUnit);

  return (
    <article className={PANEL_CLASS}>
      <div className={PANEL_HEADER_CLASS}>
        <h2 className={DEVICE_NAME_CLASS}>{device.name}</h2>
        <p className={DEVICE_META_CLASS}>Pico • Updated {device.updatedAt}</p>
      </div>

      <div className={PANEL_BODY_CLASS}>
        {/* switch between now and chart views */}
        <SegmentedControl
          value={viewMode}
          onChange={onViewModeChange}
          options={DETAIL_MODE_OPTIONS}
          className={TWO_COL_SEGMENT_CLASS}
          itemClassName={DETAIL_MODE_ITEM_CLASS}
        />

        {viewMode === 'now' ? (
          // current values as metric cards
          <div className={METRIC_GRID_CLASS}>
            <MetricCard
              variant="temp"
              value={`${temperature.toFixed(1)} ${temperatureUnitSymbol(temperatureUnit)}`}
              subValue="Ambient reading"
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
              value={`${device.lightPercent}%`}
              subValue="Ambient brightness"
              label="Light"
            />
          </div>
        ) : (
          // week trend chart
          <WeeklyChart data={weeklyData} temperatureUnit={temperatureUnit} />
        )}
      </div>
    </article>
  );
}
