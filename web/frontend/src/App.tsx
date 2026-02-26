import { useEffect, useMemo, useState } from 'react';
import { DataTable } from './components/DataTable';
import { DeviceDetailPanel } from './components/DeviceDetailPanel';
import { DeviceSidebar } from './components/DeviceSidebar';
import { SegmentedControl } from './components/SegmentedControl';
import { ThemeToggle } from './components/ThemeToggle';
import type { DetailMode, DeviceReading, PanelMode, WeeklyPoint } from './types/monitoring';
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

type ApiLatestDevice = {
  sensorId: string;
  humidity: number | null;
  temperatureC: number | null;
  light: number | null;
  receivedAtUtc?: string;
  ts?: number;
};

type ApiHistoryPoint = {
  temperatureC: number | null;
  humidity: number | null;
  light: number | null;
  receivedAtUtc?: string;
  ts?: number;
};

const API_BASE = import.meta.env.VITE_API_BASE_URL ?? 'http://localhost:3001';
const WS_URL = import.meta.env.VITE_WS_URL ?? 'ws://localhost:3001/ws/sensors';
const ONLINE_WINDOW_MS = 2 * 60 * 1000;

function asNumber(value: unknown): number {
  if (typeof value === 'number' && Number.isFinite(value)) return value;
  return 0;
}

function toTimeLabel(timestamp: number): string {
  return new Date(timestamp).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
}

function toDeviceReading(item: ApiLatestDevice): DeviceReading {
  const parsed = item.receivedAtUtc ? Date.parse(item.receivedAtUtc) : item.ts ?? Date.now();
  const ts = Number.isFinite(parsed) ? parsed : Date.now();
  return {
    id: item.sensorId,
    name: item.sensorId,
    status: Date.now() - ts <= ONLINE_WINDOW_MS ? 'online' : 'offline',
    updatedAt: toTimeLabel(ts),
    temperatureC: asNumber(item.temperatureC),
    humidity: asNumber(item.humidity),
    photoResistorOhm: asNumber(item.light),
  };
}

function toWeeklyPoint(item: ApiHistoryPoint): WeeklyPoint {
  const parsed = item.receivedAtUtc ? Date.parse(item.receivedAtUtc) : item.ts ?? Date.now();
  const ts = Number.isFinite(parsed) ? parsed : Date.now();
  return {
    label: new Date(ts).toLocaleDateString([], { weekday: 'short' }),
    temperatureC: asNumber(item.temperatureC),
    humidity: asNumber(item.humidity),
    lux: asNumber(item.light),
  };
}

export default function App() {
  // here we keep main page states
  const [devices, setDevices] = useState<DeviceReading[]>([]);
  const [historyByDevice, setHistoryByDevice] = useState<Record<string, WeeklyPoint[]>>({});
  const [selectedId, setSelectedId] = useState('');
  const [panelMode, setPanelMode] = useState<PanelMode>('normal');
  const [detailMode, setDetailMode] = useState<DetailMode>('now');
  const [isDarkMode, setIsDarkMode] = useState(false);

  // get selected device data
  const selectedDevice = useMemo<DeviceReading | null>(() => {
    return devices.find((device) => device.id === selectedId) ?? devices[0] ?? null;
  }, [devices, selectedId]);

  const weeklyData = useMemo(() => {
    if (!selectedDevice) return [];
    return historyByDevice[selectedDevice.id] ?? [];
  }, [historyByDevice, selectedDevice]);
  const contentGridClass =
    panelMode === 'table' ? CONTENT_GRID_TABLE_CLASS : CONTENT_GRID_NORMAL_CLASS;

  // syncronisation of light /darkmode as global state
  useEffect(() => {
    document.documentElement.dataset.theme = isDarkMode ? 'dark' : 'light';
  }, [isDarkMode]);

  useEffect(() => {
    async function loadLatest(): Promise<void> {
      const response = await fetch(`${API_BASE}/api/sensors/latest`);
      if (!response.ok) return;
      const payload = (await response.json()) as ApiLatestDevice[];
      const nextDevices = payload.map(toDeviceReading);
      setDevices(nextDevices);
      if (nextDevices.length > 0) {
        setSelectedId((current) =>
          nextDevices.some((device) => device.id === current) ? current : nextDevices[0].id
        );
      }
    }

    void loadLatest();
    const pollId = window.setInterval(() => {
      void loadLatest();
    }, 5000);

    const ws = new WebSocket(WS_URL);
    ws.onmessage = (event) => {
      try {
        const payload = JSON.parse(event.data as string) as {
          type?: string;
          data?: ApiLatestDevice;
        };
        if (payload.type !== 'sensor_reading' || !payload.data?.sensorId) return;
        const next = toDeviceReading(payload.data);
        setDevices((current) => {
          const idx = current.findIndex((device) => device.id === next.id);
          if (idx === -1) return [...current, next];
          const copy = [...current];
          copy[idx] = next;
          return copy;
        });
        setSelectedId((current) => current || next.id);
      } catch {
        // Ignore malformed websocket message.
      }
    };

    return () => {
      window.clearInterval(pollId);
      ws.close();
    };
  }, []);

  useEffect(() => {
    if (!selectedDevice) return;

    async function loadHistory(): Promise<void> {
      const response = await fetch(`${API_BASE}/api/sensors/${selectedDevice.id}/history`);
      if (!response.ok) return;
      const payload = (await response.json()) as ApiHistoryPoint[];
      const points = payload.slice(0, 7).reverse().map(toWeeklyPoint);
      setHistoryByDevice((current) => ({ ...current, [selectedDevice.id]: points }));
    }

    void loadHistory();
  }, [selectedDevice]);

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
            <DeviceSidebar devices={devices} selectedId={selectedId} onSelect={setSelectedId} />
          </div>

          {/* right side content by selected mode */}
          <div className={MIN_WIDTH_RESET_CLASS}>
            {!selectedDevice ? (
              <article className="rounded-[24px] border border-[var(--line)] bg-[var(--card)] p-6 text-[var(--text-soft)]">
                Waiting for backend sensor data...
              </article>
            ) : panelMode === 'normal' ? (
              <DeviceDetailPanel
                device={selectedDevice}
                viewMode={detailMode}
                weeklyData={weeklyData}
                onViewModeChange={setDetailMode}
              />
            ) : (
              <DataTable devices={devices} selectedId={selectedId} weeklyData={weeklyData} />
            )}
          </div>
        </div>
      </section>
    </main>
  );
}
