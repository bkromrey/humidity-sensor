import type { ReactNode } from 'react';
import { ACTIVE_CLASS, ACTIVE_STYLE, INACTIVE_CLASS, ITEM_CLASS, WRAP_CLASS } from './SegmentedControl.styles';

type SegmentOption<T extends string> = {
  value: T;
  label: ReactNode;
};

type SegmentedControlProps<T extends string> = {
  value: T;
  options: SegmentOption<T>[];
  onChange: (value: T) => void;
  className?: string;
  itemClassName?: string;
};

export function SegmentedControl<T extends string>({
  value,
  options,
  onChange,
  className = '',
  itemClassName = '',
}: SegmentedControlProps<T>) {
  return (
    <div className={`${WRAP_CLASS} ${className}`}>
      {options.map((option) => {
        const active = option.value === value;
        return (
          <button
            key={option.value}
            type="button"
            onClick={() => onChange(option.value)}
            className={`${ITEM_CLASS} ${itemClassName} ${active ? ACTIVE_CLASS : INACTIVE_CLASS}`}
            style={active ? ACTIVE_STYLE : undefined}
          >
            {option.label}
          </button>
        );
      })}
    </div>
  );
}
