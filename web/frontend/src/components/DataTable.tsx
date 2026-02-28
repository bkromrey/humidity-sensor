import { Fragment } from 'react';
import type { DeviceReading, TemperatureUnit, WeeklyPoint } from '../types/monitoring';
import { temperatureByUnit, temperatureUnitSymbol } from '../utils/sensorMath';
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
  TEMP_COLUMN_CLASS,
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
  // selected global temperature unit
  temperatureUnit: TemperatureUnit;
};

const MAIN_TABLE_HEADERS_BASE = ['Pico', 'Updated', 'Humidity', 'Light %'];
const WEEKLY_TABLE_HEADERS_BASE = ['Day', 'Updated', 'Humidity', 'Light %'];

export function DataTable({ devices, selectedId, weeklyData, temperatureUnit }: DataTableProps) {
  // fallback to first item if id is not found
  const selectedDevice = devices.find((device) => device.id === selectedId) ?? devices[0];
  const temperatureHeader = `Temp ${temperatureUnitSymbol(temperatureUnit)}`;
  const mainTableHeaders = [MAIN_TABLE_HEADERS_BASE[0], MAIN_TABLE_HEADERS_BASE[1], temperatureHeader, ...MAIN_TABLE_HEADERS_BASE.slice(2)];
  const weeklyTableHeaders = [
    WEEKLY_TABLE_HEADERS_BASE[0],
    WEEKLY_TABLE_HEADERS_BASE[1],
    temperatureHeader,
    ...WEEKLY_TABLE_HEADERS_BASE.slice(2),
  ];

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
              {mainTableHeaders.map((header, index) => (
                <th key={header} className={`${CELL_CLASS} ${index === 2 ? TEMP_COLUMN_CLASS : ''}`}>
                  {header}
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {devices.map((device) => {
              const temperature = temperatureByUnit(device.temperatureC, device.temperatureF, temperatureUnit);
              const isSelected = device.id === selectedId;

              return (
                <Fragment key={device.id}>
                  <tr className={`${MAIN_ROW_CLASS} ${isSelected ? MAIN_ROW_SELECTED_CLASS : ''}`}>
                    <td className={`${CELL_CLASS} ${FONT_SEMIBOLD_CLASS}`}>{device.name}</td>
                    <td className={`${CELL_CLASS} ${MUTED_TEXT_CLASS}`}>{device.updatedAt}</td>
                    <td className={`${CELL_CLASS} ${TEMP_COLUMN_CLASS} ${TEMP_TEXT_CLASS}`}>
                      {temperature.toFixed(1)}
                      {temperatureUnitSymbol(temperatureUnit)}
                    </td>
                    <td className={`${CELL_CLASS} ${HUMIDITY_TEXT_CLASS}`}>{device.humidity}%</td>
                    <td className={`${CELL_CLASS} ${LIGHT_TEXT_CLASS}`}>{device.lightPercent}%</td>
                  </tr>

                  {isSelected ? (
                    // extra nested table only for selected row
                    <tr className={NESTED_WRAP_ROW_CLASS}>
                      <td colSpan={5} className={NESTED_SECTION_CELL_CLASS}>
                        <p className={WEEKLY_TITLE_CLASS}>Weekly history for {device.name}</p>

                        <table className={NESTED_TABLE_CLASS}>
                          <thead>
                            <tr className={NESTED_HEAD_ROW_CLASS}>
                              {weeklyTableHeaders.map((header, index) => (
                                <th key={header} className={`${NESTED_CELL_CLASS} ${index === 2 ? TEMP_COLUMN_CLASS : ''}`}>
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
                                <td className={`${NESTED_CELL_CLASS} ${TEMP_COLUMN_CLASS} ${TEMP_TEXT_CLASS}`}>
                                  {temperatureByUnit(point.temperatureC, point.temperatureF, temperatureUnit).toFixed(1)}
                                  {temperatureUnitSymbol(temperatureUnit)}
                                </td>
                                <td className={`${NESTED_CELL_CLASS} ${HUMIDITY_TEXT_CLASS}`}>{point.humidity}%</td>
                                <td className={`${NESTED_CELL_CLASS} ${LIGHT_TEXT_CLASS}`}>{point.lightPercent}%</td>
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
