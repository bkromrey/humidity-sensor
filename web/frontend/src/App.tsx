import { useEffect, useMemo, useState } from 'react';
import { DataTable } from './components/DataTable';
import { DeviceDetailPanel } from './components/DeviceDetailPanel';
import { DeviceSidebar } from './components/DeviceSidebar';
import { SegmentedControl } from './components/SegmentedControl';
import { TemperatureToggle } from './components/TemperatureToggle';
import { ThemeToggle } from './components/ThemeToggle';
import type { DetailMode, DeviceReading, PanelMode, TemperatureUnit, WeeklyPoint } from './types/monitoring';
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

function resolveWebSocketUrl(): string {
  const configuredWsUrl = import.meta.env.VITE_WS_URL;
  if (configuredWsUrl) {
    const parsed = new URL(configuredWsUrl, window.location.origin);
    if (window.location.protocol === 'https:' && parsed.protocol === 'ws:') {
      parsed.protocol = 'wss:';
    }
    return parsed.toString();
  }

  const parsedApiBase = new URL(API_BASE, window.location.origin);
  parsedApiBase.protocol = parsedApiBase.protocol === 'https:' ? 'wss:' : 'ws:';
  parsedApiBase.pathname = '/ws/sensors';
  parsedApiBase.search = '';
  parsedApiBase.hash = '';
  return parsedApiBase.toString();
}

const WS_URL = resolveWebSocketUrl();
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

function toAggregatedPoint(bucketTs: number, bucketPoints: RawHistoryPoint[]): WeeklyPoint {
  const latestPoint = bucketPoints.reduce((latest, point) => (point.ts > latest.ts ? point : latest));
  return {
    ts: bucketTs,
    label: toTimeLabel(bucketTs),
    updatedAt: toDateLabel(bucketTs),
    temperatureC: asNumber(latestPoint.temperatureC),
    temperatureF: asNumber(latestPoint.temperatureF),
    humidity: asPercent(latestPoint.humidity),
    lightPercent: asPercent(latestPoint.light),
  };
}

function buildHistoryPoints(rawPoints: RawHistoryPoint[]): WeeklyPoint[] {
  if (rawPoints.length === 0) return [];

  const bucketMap = new Map<number, RawHistoryPoint[]>();
  const oneHourMs = 60 * 60 * 1000;

  for (const point of rawPoints) {
    const bucketTs = Math.floor(point.ts / oneHourMs) * oneHourMs;
    const bucket = bucketMap.get(bucketTs);
    if (bucket) {
      bucket.push(point);
    } else {
      bucketMap.set(bucketTs, [point]);
    }
  }

  return Array.from(bucketMap.entries())
    .sort(([leftTs], [rightTs]) => leftTs - rightTs)
    .map(([bucketTs, bucketPoints]) => toAggregatedPoint(bucketTs, bucketPoints));
}

export default function App() {
  // here we keep main page states
  const [devices, setDevices] = useState<DeviceReading[]>([]);
  const [historyByDevice, setHistoryByDevice] = useState<Record<string, WeeklyPoint[]>>({});
  const [selectedId, setSelectedId] = useState('');
  const [panelMode, setPanelMode] = useState<PanelMode>('normal');
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
      const from = new Date(now.getTime() - 24 * 60 * 60 * 1000);
      const query = new URLSearchParams({
        from: from.toISOString(),
        to: now.toISOString(),
      });
      const response = await fetch(`${API_BASE}/api/sensors/${sensorId}/history?${query.toString()}`);
      if (!response.ok) return;
      const payload = (await response.json()) as ApiHistoryPoint[];
      const rawPoints = payload.reverse().map(toRawHistoryPoint);
      const points = buildHistoryPoints(rawPoints);
      setHistoryByDevice((current) => ({ ...current, [sensorId]: points }));
    }

    void loadHistory();
    const pollId = window.setInterval(() => {
      void loadHistory();
    }, 60_000);

    return () => {
      window.clearInterval(pollId);
    };
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
            <div className={MINI_TOGGLES_CLASS}>
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
                onViewModeChange={setDetailMode}
                temperatureUnit={temperatureUnit}
              />
            ) : (
              <DataTable
                devices={devices}
                selectedId={selectedId}
                weeklyData={weeklyData}
                temperatureUnit={temperatureUnit}
              />
            )}
          </div>
        </div>
      </section>
    </main>
  );
}
