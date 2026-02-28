import type { TemperatureUnit } from '../types/monitoring';
import {
  BUTTON_CLASS_BASE,
  CELSIUS_BUTTON_CLASS,
  CURRENT_UNIT_CLASS,
  FAHRENHEIT_BUTTON_CLASS,
  NEXT_UNIT_CLASS,
} from './TemperatureToggle.styles';

type TemperatureToggleProps = {
  temperatureUnit: TemperatureUnit;
  onToggle: () => void;
  className?: string;
};

export function TemperatureToggle({ temperatureUnit, onToggle, className = '' }: TemperatureToggleProps) {
  const currentUnit = temperatureUnit === 'celsius' ? 'C' : 'F';
  const nextUnit = temperatureUnit === 'celsius' ? 'F' : 'C';

  return (
    <button
      type="button"
      onClick={onToggle}
      aria-label={`Switch temperature unit to ${temperatureUnit === 'celsius' ? 'fahrenheit' : 'celsius'}`}
      title={`Switch to ${temperatureUnit === 'celsius' ? 'Fahrenheit' : 'Celsius'}`}
      className={`${BUTTON_CLASS_BASE} ${temperatureUnit === 'celsius' ? CELSIUS_BUTTON_CLASS : FAHRENHEIT_BUTTON_CLASS} ${className}`}
    >
      <span className={CURRENT_UNIT_CLASS}>{currentUnit}</span>
      <span className={NEXT_UNIT_CLASS}>{nextUnit}</span>
    </button>
  );
}
