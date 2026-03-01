import type { HistoryRange } from '../types/monitoring';
import {
  BUTTON_CLASS_BASE,
  CURRENT_VALUE_CLASS,
  DAY_BUTTON_CLASS,
  NEXT_VALUE_CLASS,
  WEEK_BUTTON_CLASS,
} from './HistoryRangeToggle.styles';

type HistoryRangeToggleProps = {
  historyRange: HistoryRange;
  onToggle: () => void;
  className?: string;
};

export function HistoryRangeToggle({
  historyRange,
  onToggle,
  className = '',
}: HistoryRangeToggleProps) {
  const currentValue = historyRange === '1d' ? 'D' : 'W';
  const nextValue = historyRange === '1d' ? 'W' : 'D';

  return (
    <button
      type="button"
      onClick={onToggle}
      aria-label={`Switch history range to ${historyRange === '1d' ? 'week' : 'day'}`}
      title={`Switch to ${historyRange === '1d' ? 'Week' : 'Day'}`}
      className={`${BUTTON_CLASS_BASE} ${historyRange === '1d' ? DAY_BUTTON_CLASS : WEEK_BUTTON_CLASS} ${className}`}
    >
      <span className={CURRENT_VALUE_CLASS}>{currentValue}</span>
      <span className={NEXT_VALUE_CLASS}>{nextValue}</span>
    </button>
  );
}
