import { Fragment } from 'react';
import type { DeviceReading, WeeklyPoint } from '../types/monitoring';
import { luxToResistance, resistanceToLux, toFahrenheit } from '../utils/sensorMath';
import {
  CELL_CLASS,
  FONT_SEMIBOLD_CLASS,
  HEADER_CLASS,
  HUMIDITY_TEXT_CLASS,
  LIGHT_TEXT_CLASS,
  MAIN_HEAD_ROW_CLASS,
  MAIN_ROW_CLASS,
  MAIN_ROW_SELECTED_CLASS,
  MUTED_TEXT_CLASS,
  NESTED_CELL_CLASS,
  NESTED_HEAD_ROW_CLASS,
  NESTED_ROW_CLASS,
  NESTED_SECTION_CELL_CLASS,
  NESTED_TABLE_CLASS,
  NESTED_WRAP_ROW_CLASS,
  PANEL_CLASS,
  SUBTITLE_CLASS,
  TABLE_CLASS,
  TABLE_WRAP_CLASS,
  TEMP_TEXT_CLASS,
  TITLE_CLASS,
  WEEKLY_TITLE_CLASS,
} from './DataTable.styles';

type DataTableProps = {
  // all available devices
  devices: DeviceReading[];
  // active device id from sidebar
  selectedId: string;
  // weekly points for selected device
  weeklyData: WeeklyPoint[];
};

const MAIN_TABLE_HEADERS = ['Pico', 'Updated', 'Temp °C', 'Temp °F', 'Humidity', 'Light lx', 'Photoresistor'];
const WEEKLY_TABLE_HEADERS = ['Day', 'Updated', 'Temp °C', 'Temp °F', 'Humidity', 'Light lx', 'Photoresistor'];

export function DataTable({ devices, selectedId, weeklyData }: DataTableProps) {
  // fallback to first item if id is not found
  const selectedDevice = devices.find((device) => device.id === selectedId) ?? devices[0];

  return (
    <article className={PANEL_CLASS}>
      <div className={HEADER_CLASS}>
        <h2 className={TITLE_CLASS}>Data Table</h2>
        <p className={SUBTITLE_CLASS}>Selected: {selectedDevice.name}</p>
      </div>

      <div className={TABLE_WRAP_CLASS}>
        <table className={TABLE_CLASS}>
          <thead>
            {/* main table header */}
            <tr className={MAIN_HEAD_ROW_CLASS}>
              {MAIN_TABLE_HEADERS.map((header) => (
                <th key={header} className={CELL_CLASS}>
                  {header}
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {devices.map((device) => {
              // convert sensor resistance to lux for display
              const lux = resistanceToLux(device.photoResistorOhm);
              const isSelected = device.id === selectedId;

              return (
                <Fragment key={device.id}>
                  <tr className={`${MAIN_ROW_CLASS} ${isSelected ? MAIN_ROW_SELECTED_CLASS : ''}`}>
                    <td className={`${CELL_CLASS} ${FONT_SEMIBOLD_CLASS}`}>{device.name}</td>
                    <td className={`${CELL_CLASS} ${MUTED_TEXT_CLASS}`}>{device.updatedAt}</td>
                    <td className={`${CELL_CLASS} ${TEMP_TEXT_CLASS}`}>{device.temperatureC.toFixed(1)}°C</td>
                    <td className={CELL_CLASS}>{toFahrenheit(device.temperatureC).toFixed(1)}°F</td>
                    <td className={`${CELL_CLASS} ${HUMIDITY_TEXT_CLASS}`}>{device.humidity}%</td>
                    <td className={`${CELL_CLASS} ${LIGHT_TEXT_CLASS}`}>{lux}</td>
                    <td className={`${CELL_CLASS} ${MUTED_TEXT_CLASS}`}>{device.photoResistorOhm} ohm</td>
                  </tr>

                  {isSelected ? (
                    // extra nested table only for selected row
                    <tr className={NESTED_WRAP_ROW_CLASS}>
                      <td colSpan={7} className={NESTED_SECTION_CELL_CLASS}>
                        <p className={WEEKLY_TITLE_CLASS}>Weekly history for {device.name}</p>

                        <table className={NESTED_TABLE_CLASS}>
                          <thead>
                            <tr className={NESTED_HEAD_ROW_CLASS}>
                              {WEEKLY_TABLE_HEADERS.map((header) => (
                                <th key={header} className={NESTED_CELL_CLASS}>
                                  {header}
                                </th>
                              ))}
                            </tr>
                          </thead>
                          <tbody>
                            {weeklyData.map((point) => (
                              <tr key={point.label} className={NESTED_ROW_CLASS}>
                                <td className={`${NESTED_CELL_CLASS} ${FONT_SEMIBOLD_CLASS}`}>{point.label}</td>
                                <td className={`${NESTED_CELL_CLASS} ${MUTED_TEXT_CLASS}`}>{device.updatedAt}</td>
                                <td className={`${NESTED_CELL_CLASS} ${TEMP_TEXT_CLASS}`}>{point.temperatureC.toFixed(1)}°C</td>
                                <td className={NESTED_CELL_CLASS}>{toFahrenheit(point.temperatureC).toFixed(1)}°F</td>
                                <td className={`${NESTED_CELL_CLASS} ${HUMIDITY_TEXT_CLASS}`}>{point.humidity}%</td>
                                <td className={`${NESTED_CELL_CLASS} ${LIGHT_TEXT_CLASS}`}>{point.lux}</td>
                                <td className={`${NESTED_CELL_CLASS} ${MUTED_TEXT_CLASS}`}>{luxToResistance(point.lux)} ohm</td>
                              </tr>
                            ))}
                          </tbody>
                        </table>
                      </td>
                    </tr>
                  ) : null}
                </Fragment>
              );
            })}
          </tbody>
        </table>
      </div>
    </article>
  );
}
