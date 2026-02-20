import { useEffect, useMemo, useState } from 'react';
import { DataTable } from './components/DataTable';
import { DeviceDetailPanel } from './components/DeviceDetailPanel';
import { DeviceSidebar } from './components/DeviceSidebar';
import { SegmentedControl } from './components/SegmentedControl';
import { ThemeToggle } from './components/ThemeToggle';
import { DEVICE_WEEKLY_HISTORY, MOCK_DEVICES } from './data/mockMonitoring';
import type { DetailMode, DeviceReading, PanelMode } from './types/monitoring';
import {
  CONTAINER_CLASS,
  CONTENT_GRID_NORMAL_CLASS,
  CONTENT_GRID_TABLE_CLASS,
  HEADER_CLASS,
  HEADER_CONTROLS_CLASS,
  MIN_WIDTH_RESET_CLASS,
  PAGE_CLASS,
  TITLE_CLASS,
  TWO_COL_SEGMENT_CLASS,
} from './App.styles';

// Options for the main panel mode switcher
const PANEL_MODE_OPTIONS: { value: PanelMode; label: string }[] = [
  { value: 'normal', label: 'Normal' },
  { value: 'table', label: 'Table' },
];

export default function App() {
  // here we keep main page states
  const [selectedId, setSelectedId] = useState(MOCK_DEVICES[0].id);
  const [panelMode, setPanelMode] = useState<PanelMode>('normal');
  const [detailMode, setDetailMode] = useState<DetailMode>('now');
  const [isDarkMode, setIsDarkMode] = useState(false);

  // get selected device data
  const selectedDevice = useMemo<DeviceReading>(() => {
    return MOCK_DEVICES.find((device) => device.id === selectedId) ?? MOCK_DEVICES[0];
  }, [selectedId]);

  const weeklyData = useMemo(() => DEVICE_WEEKLY_HISTORY[selectedDevice.id] ?? [], [selectedDevice.id]);
  const contentGridClass =
    panelMode === 'table' ? CONTENT_GRID_TABLE_CLASS : CONTENT_GRID_NORMAL_CLASS;

  // syncronisation of light /darkmode as global state
  useEffect(() => {
    document.documentElement.dataset.theme = isDarkMode ? 'dark' : 'light';
  }, [isDarkMode]);

  return (
    <main className={PAGE_CLASS}>
      <section className={CONTAINER_CLASS}>
        {/* page header */}
        <header className={HEADER_CLASS}>
          <h1 className={TITLE_CLASS}>Climate · Pico Monitoring</h1>

          {/* panel mode + theme switch */}
          <div className={HEADER_CONTROLS_CLASS}>
            <SegmentedControl
              value={panelMode}
              onChange={setPanelMode}
              options={PANEL_MODE_OPTIONS}
              className={TWO_COL_SEGMENT_CLASS}
            />
            <ThemeToggle isDarkMode={isDarkMode} onToggle={() => setIsDarkMode((prev) => !prev)} />
          </div>
        </header>

        {/* main layout */}
        <div className={contentGridClass}>
          {/* left side device cards */}
          <div className={MIN_WIDTH_RESET_CLASS}>
            <DeviceSidebar devices={MOCK_DEVICES} selectedId={selectedId} onSelect={setSelectedId} />
          </div>

          {/* right side content by selected mode */}
          <div className={MIN_WIDTH_RESET_CLASS}>
            {panelMode === 'normal' ? (
              <DeviceDetailPanel
                device={selectedDevice}
                viewMode={detailMode}
                weeklyData={weeklyData}
                onViewModeChange={setDetailMode}
              />
            ) : (
              <DataTable devices={MOCK_DEVICES} selectedId={selectedId} weeklyData={weeklyData} />
            )}
          </div>
        </div>
      </section>
    </main>
  );
}
