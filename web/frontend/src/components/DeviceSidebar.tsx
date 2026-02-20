import type { DeviceReading } from '../types/monitoring';
import { resistanceToLux } from '../utils/sensorMath';
import {
  ASIDE_CLASS,
  CARD_CLASS,
  DEVICE_NAME_CLASS,
  HUMIDITY_CELL_CLASS,
  HUMIDITY_VALUE_CLASS,
  INACTIVE_CARD_CLASS,
  LIGHT_CELL_CLASS,
  LIGHT_UNIT_CLASS,
  METRICS_GRID_CLASS,
  METRIC_LABEL_CLASS,
  METRIC_VALUE_CLASS,
  SELECTED_CARD_CLASS,
  TEMP_CELL_CLASS,
  UPDATED_CLASS,
} from './DeviceSidebar.styles';

type DeviceSidebarProps = {
  // all devices we render in sidebar
  devices: DeviceReading[];
  // currently active device id
  selectedId: string;
  // callback when user clicks a device card
  onSelect: (id: string) => void;
};

const MOBILE_TEMP_LABEL = 'Temp';
const MOBILE_HUMIDITY_LABEL = 'Hum';
const DESKTOP_TEMP_LABEL = 'Temperature';
const DESKTOP_HUMIDITY_LABEL = 'Humidity';

export function DeviceSidebar({ devices, selectedId, onSelect }: DeviceSidebarProps) {
  return (
    <aside className={ASIDE_CLASS}>
      {devices.map((device) => {
        // convert sensor value to lux for UI
        const lux = resistanceToLux(device.photoResistorOhm);
        const selected = selectedId === device.id;

        return (
          <button
            key={device.id}
            type="button"
            onClick={() => onSelect(device.id)}
            // base card class + selected/inactive state class
            className={`${CARD_CLASS} ${selected ? SELECTED_CARD_CLASS : INACTIVE_CARD_CLASS}`}
          >
            <h2 className={DEVICE_NAME_CLASS}>{device.name}</h2>
            <p className={UPDATED_CLASS}>Updated {device.updatedAt}</p>

            <div className={METRICS_GRID_CLASS}>
              <div className={TEMP_CELL_CLASS}>
                <p className={METRIC_LABEL_CLASS}>
                  <ResponsiveMetricLabel mobileLabel={MOBILE_TEMP_LABEL} desktopLabel={DESKTOP_TEMP_LABEL} />
                </p>
                <p className={`${METRIC_VALUE_CLASS} text-[var(--temp)]`}>
                  <span>{device.temperatureC.toFixed(1)}</span>
                  <span>°C</span>
                </p>
              </div>

              <div className={HUMIDITY_CELL_CLASS}>
                <p className={METRIC_LABEL_CLASS}>
                  <ResponsiveMetricLabel
                    mobileLabel={MOBILE_HUMIDITY_LABEL}
                    desktopLabel={DESKTOP_HUMIDITY_LABEL}
                  />
                </p>
                <p className={HUMIDITY_VALUE_CLASS}>{device.humidity}%</p>
              </div>

              <div className={LIGHT_CELL_CLASS}>
                <p className={METRIC_LABEL_CLASS}>Light</p>
                <p className={`${METRIC_VALUE_CLASS} text-[var(--light)]`}>
                  <span>{lux}</span>
                  <span className={LIGHT_UNIT_CLASS}>lx</span>
                </p>
              </div>
            </div>
          </button>
        );
      })}
    </aside>
  );
}

type ResponsiveMetricLabelProps = {
  mobileLabel: string;
  desktopLabel: string;
};

function ResponsiveMetricLabel({ mobileLabel, desktopLabel }: ResponsiveMetricLabelProps) {
  return (
    <>
      {/* shorter text on small screens */}
      <span className="sm:hidden">{mobileLabel}</span>
      {/* full text on bigger screens */}
      <span className="hidden sm:inline">{desktopLabel}</span>
    </>
  );
}
