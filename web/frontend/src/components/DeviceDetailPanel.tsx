import { SegmentedControl } from './SegmentedControl';
import { MetricCard } from './MetricCard';
import { WeeklyChart } from './WeeklyChart';
import type {
  DetailMode,
  DeviceReading,
  TemperatureUnit,
  WeeklyPoint,
} from '../types/monitoring';
import { temperatureByUnit, temperatureUnitSymbol } from '../utils/sensorMath';
import {
  DETAIL_MODE_ITEM_CLASS,
  DEVICE_META_CLASS,
  DEVICE_NAME_CLASS,
  METRIC_GRID_CLASS,
  MODE_ROW_CLASS,
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

function getHumidityRank(value: number): { short: string; details: string } {
  if (value <= 29) {
    return {
      short: 'Too Dry',
      details:
        '0-29%: Air is excessively dry. May cause dry skin, irritated sinuses, and static electricity.',
    };
  }
  if (value <= 39) {
    return {
      short: 'Slightly Dry',
      details: '30-39%: Acceptable, but may feel dry during heating season.',
    };
  }
  if (value <= 50) {
    return {
      short: 'Optimal',
      details: '40-50%: Ideal comfort range for most homes.',
    };
  }
  if (value <= 60) {
    return {
      short: 'Acceptable',
      details: '51-60%: Still within recommended limits, but approaching high humidity.',
    };
  }
  return {
    short: 'Too Humid',
    details: '61-100%: Increased risk of mold growth, dust mites, and condensation issues.',
  };
}

function getTemperatureRank(valueF: number): { short: string; details: string } {
  if (valueF < 60) {
    return {
      short: 'Very Cold',
      details: 'Below 60°F (15°C): Very cold.',
    };
  }
  if (valueF <= 66) {
    return {
      short: 'Cool',
      details: '60-66°F (16-19°C): Cool and fresh; often comfortable for sleep.',
    };
  }
  if (valueF <= 72) {
    return {
      short: 'Comfortable',
      details: '67-72°F (19-22°C): Comfortable for most people.',
    };
  }
  if (valueF <= 77) {
    return {
      short: 'Warm',
      details: '73-77°F (23-25°C): Warm.',
    };
  }
  return {
    short: 'Hot',
    details: '78°F+ (26°C+): Hot.',
  };
}

function getLightRank(value: number): { short: string; details: string } {
  if (value <= 10) {
    return {
      short: 'Dark',
      details: '0-10%: Almost no light.',
    };
  }
  if (value <= 30) {
    return {
      short: 'Dim',
      details: '11-30%: Low light, soft ambient lighting.',
    };
  }
  if (value <= 60) {
    return {
      short: 'Normal',
      details: '31-60%: Typical indoor lighting.',
    };
  }
  if (value <= 85) {
    return {
      short: 'Bright',
      details: '61-85%: Very well lit room.',
    };
  }
  return {
    short: 'Very Bright',
    details: '86-100%: Strong daylight or direct light.',
  };
}

export function DeviceDetailPanel({
  device,
  viewMode,
  weeklyData,
  onViewModeChange,
  temperatureUnit,
}: DeviceDetailPanelProps) {
  const temperature = temperatureByUnit(device.temperatureC, device.temperatureF, temperatureUnit);
  const humidityRank = getHumidityRank(device.humidity);
  const temperatureRank = getTemperatureRank(device.temperatureF);
  const lightRank = getLightRank(device.lightPercent);

  return (
    <article className={PANEL_CLASS}>
      <div className={PANEL_HEADER_CLASS}>
        <h2 className={DEVICE_NAME_CLASS}>{device.name}</h2>
        <p className={DEVICE_META_CLASS}>Updated: {device.updatedAtFull}</p>
      </div>

      <div className={PANEL_BODY_CLASS}>
        {/* switch between now and chart views */}
        <div className={MODE_ROW_CLASS}>
          <SegmentedControl
            value={viewMode}
            onChange={onViewModeChange}
            options={DETAIL_MODE_OPTIONS}
            className={TWO_COL_SEGMENT_CLASS}
            itemClassName={DETAIL_MODE_ITEM_CLASS}
          />
        </div>

        {viewMode === 'now' ? (
          // current values as metric cards
          <div className={METRIC_GRID_CLASS}>
            <MetricCard
              variant="temp"
              value={`${temperature.toFixed(1)} ${temperatureUnitSymbol(temperatureUnit)}`}
              subValue={temperatureRank.short}
              subValueTitle={temperatureRank.details}
              label="Temperature"
            />
            <MetricCard
              variant="humidity"
              value={`${device.humidity.toFixed(1)}%`}
              subValue={humidityRank.short}
              subValueTitle={humidityRank.details}
              label="Humidity"
            />
            <MetricCard
              variant="light"
              value={`${device.lightPercent.toFixed(1)}%`}
              subValue={lightRank.short}
              subValueTitle={lightRank.details}
              label="Light"
            />
          </div>
        ) : (
          // week trend chart
          <WeeklyChart
            data={weeklyData}
            temperatureUnit={temperatureUnit}
          />
        )}
      </div>
    </article>
  );
}
