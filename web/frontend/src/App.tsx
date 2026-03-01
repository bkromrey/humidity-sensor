import { useEffect, useMemo, useState } from 'react';
import { DataTable } from './components/DataTable';
import { DeviceDetailPanel } from './components/DeviceDetailPanel';
import { DeviceSidebar } from './components/DeviceSidebar';
import { HistoryRangeToggle } from './components/HistoryRangeToggle';
import { SegmentedControl } from './components/SegmentedControl';
import { TemperatureToggle } from './components/TemperatureToggle';
import { ThemeToggle } from './components/ThemeToggle';
import type {
  DetailMode,
  DeviceReading,
  HistoryRange,
  PanelMode,
  TemperatureUnit,
  WeeklyPoint,
} from './types/monitoring';
import {
  CONTAINER_CLASS,
  CONTENT_GRID_NORMAL_CLASS,
  CONTENT_GRID_TABLE_CLASS,
  HEADER_CLASS,
  HEADER_CONTROLS_CLASS,
  MINI_TOGGLES_CLASS,
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
  temperatureF: number | null;
  light: number | null;
  receivedAtUtc?: string;
  ts?: number;
};

type ApiHistoryPoint = {
  temperatureC: number | null;
  temperatureF: number | null;
  humidity: number | null;
  light: number | null;
  receivedAtUtc?: string;
  ts?: number;
};

type RawHistoryPoint = {
  ts: number;
  temperatureC: number | null;
  temperatureF: number | null;
  humidity: number | null;
  light: number | null;
};

const API_BASE = import.meta.env.VITE_API_BASE_URL ?? 'http://localhost:3001';
const WS_URL = import.meta.env.VITE_WS_URL ?? 'ws://localhost:3001/ws/sensors';
const ONLINE_WINDOW_MS = 2 * 60 * 1000;

function asNumber(value: unknown): number {
  if (typeof value === 'number' && Number.isFinite(value)) return value;
  return 0;
}

function asNullableNumber(value: unknown): number | null {
  if (typeof value === 'number' && Number.isFinite(value)) return value;
  return null;
}

function clamp(value: number, min: number, max: number): number {
  return Math.min(max, Math.max(min, value));
}

function asPercent(value: unknown): number {
  return clamp(asNumber(value), 0, 100);
}

function asNullablePercent(value: unknown): number | null {
  const parsed = asNullableNumber(value);
  if (parsed === null) return null;
  return clamp(parsed, 0, 100);
}

function toTimeLabel(timestamp: number): string {
  return new Date(timestamp).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
}

function toDateLabel(timestamp: number): string {
  const date = new Date(timestamp);
  const day = date.getDate();
  const month = date.toLocaleString('en-US', { month: 'short' });
  const year = date.getFullYear();
  return `${month} ${day}, ${year}`;
}

function toDateTimeLabel(timestamp: number): string {
  return `${toDateLabel(timestamp)}, ${toTimeLabel(timestamp)}`;
}

function toDeviceReading(item: ApiLatestDevice): DeviceReading {
  const parsed = item.receivedAtUtc ? Date.parse(item.receivedAtUtc) : item.ts ?? Date.now();
  const ts = Number.isFinite(parsed) ? parsed : Date.now();
  return {
    id: item.sensorId,
    name: item.sensorId,
    status: Date.now() - ts <= ONLINE_WINDOW_MS ? 'online' : 'offline',
    updatedAt: toTimeLabel(ts),
    updatedAtFull: toDateTimeLabel(ts),
    temperatureC: asNumber(item.temperatureC),
    temperatureF: asNumber(item.temperatureF),
    humidity: asPercent(item.humidity),
    lightPercent: asPercent(item.light),
  };
}

function toRawHistoryPoint(item: ApiHistoryPoint): RawHistoryPoint {
  const parsed = item.receivedAtUtc ? Date.parse(item.receivedAtUtc) : item.ts ?? NaN;
  const ts = Number.isFinite(parsed) ? parsed : Date.now();
  return {
    ts,
    temperatureC: asNullableNumber(item.temperatureC),
    temperatureF: asNullableNumber(item.temperatureF),
    humidity: asNullablePercent(item.humidity),
    light: asNullablePercent(item.light),
  };
}

function average(values: Array<number | null>): number {
  const validValues = values.filter((value): value is number => typeof value === 'number');
  if (validValues.length === 0) return 0;
  const total = validValues.reduce((sum, value) => sum + value, 0);
  return total / validValues.length;
}

function toDayStart(timestamp: number): number {
  const date = new Date(timestamp);
  return new Date(date.getFullYear(), date.getMonth(), date.getDate()).getTime();
}

function toAggregatedPoint(bucketTs: number, bucketPoints: RawHistoryPoint[], historyRange: HistoryRange): WeeklyPoint {
  return {
    ts: bucketTs,
    label:
      historyRange === '1d'
        ? toTimeLabel(bucketTs)
        : new Date(bucketTs).toLocaleDateString('en-US', { weekday: 'short' }),
    updatedAt: toDateLabel(bucketTs),
    temperatureC: average(bucketPoints.map((point) => point.temperatureC)),
    temperatureF: average(bucketPoints.map((point) => point.temperatureF)),
    humidity: clamp(average(bucketPoints.map((point) => point.humidity)), 0, 100),
    lightPercent: clamp(average(bucketPoints.map((point) => point.light)), 0, 100),
  };
}

function buildHistoryPoints(rawPoints: RawHistoryPoint[], historyRange: HistoryRange): WeeklyPoint[] {
  if (rawPoints.length === 0) return [];

  const bucketMap = new Map<number, RawHistoryPoint[]>();
  const threeHoursMs = 3 * 60 * 60 * 1000;

  for (const point of rawPoints) {
    const bucketTs =
      historyRange === '1d' ? Math.floor(point.ts / threeHoursMs) * threeHoursMs : toDayStart(point.ts);
    const bucket = bucketMap.get(bucketTs);
    if (bucket) {
      bucket.push(point);
    } else {
      bucketMap.set(bucketTs, [point]);
    }
  }

  return Array.from(bucketMap.entries())
    .sort(([leftTs], [rightTs]) => leftTs - rightTs)
    .map(([bucketTs, bucketPoints]) => toAggregatedPoint(bucketTs, bucketPoints, historyRange));
}

export default function App() {
  // here we keep main page states
  const [devices, setDevices] = useState<DeviceReading[]>([]);
  const [historyByDevice, setHistoryByDevice] = useState<Record<string, WeeklyPoint[]>>({});
  const [selectedId, setSelectedId] = useState('');
  const [panelMode, setPanelMode] = useState<PanelMode>('normal');
  const [historyRange, setHistoryRange] = useState<HistoryRange>('1d');
  const [detailMode, setDetailMode] = useState<DetailMode>('now');
  const [temperatureUnit, setTemperatureUnit] = useState<TemperatureUnit>('celsius');
  const [isDarkMode, setIsDarkMode] = useState(false);
  const toggleTemperatureUnit = () =>
    setTemperatureUnit((prev) => (prev === 'celsius' ? 'fahrenheit' : 'celsius'));

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
    const sensorId = selectedDevice.id;

    async function loadHistory(): Promise<void> {
      const now = new Date();
      const from =
        historyRange === '1d'
          ? new Date(now.getTime() - 24 * 60 * 60 * 1000)
          : new Date(now.getTime() - 7 * 24 * 60 * 60 * 1000);
      const query = new URLSearchParams({
        from: from.toISOString(),
        to: now.toISOString(),
      });
      const response = await fetch(`${API_BASE}/api/sensors/${sensorId}/history?${query.toString()}`);
      if (!response.ok) return;
      const payload = (await response.json()) as ApiHistoryPoint[];
      const rawPoints = payload.reverse().map(toRawHistoryPoint);
      const points = buildHistoryPoints(rawPoints, historyRange);
      setHistoryByDevice((current) => ({ ...current, [sensorId]: points }));
    }

    void loadHistory();
  }, [historyRange, selectedDevice]);

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
            <div className={MINI_TOGGLES_CLASS}>
              <HistoryRangeToggle
                historyRange={historyRange}
                onToggle={() => setHistoryRange((prev) => (prev === '1d' ? '1w' : '1d'))}
                className="ml-2"
              />
              <TemperatureToggle
                temperatureUnit={temperatureUnit}
                onToggle={toggleTemperatureUnit}
              />
              <ThemeToggle isDarkMode={isDarkMode} onToggle={() => setIsDarkMode((prev) => !prev)} />
            </div>
          </div>
        </header>

        {/* main layout */}
        <div className={contentGridClass}>
          {/* left side device cards */}
          <div className={MIN_WIDTH_RESET_CLASS}>
            <DeviceSidebar
              devices={devices}
              selectedId={selectedId}
              onSelect={setSelectedId}
              temperatureUnit={temperatureUnit}
            />
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
                historyRange={historyRange}
                onViewModeChange={setDetailMode}
                temperatureUnit={temperatureUnit}
              />
            ) : (
              <DataTable
                devices={devices}
                selectedId={selectedId}
                weeklyData={weeklyData}
                historyRange={historyRange}
                temperatureUnit={temperatureUnit}
              />
            )}
          </div>
        </div>
      </section>
    </main>
  );
}
