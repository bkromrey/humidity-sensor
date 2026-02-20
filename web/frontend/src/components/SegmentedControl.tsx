import type { ReactNode } from 'react';
import { ACTIVE_CLASS, ACTIVE_STYLE, INACTIVE_CLASS, ITEM_CLASS, WRAP_CLASS } from './SegmentedControl.styles';

// one segment item: value is what we save, label is what user sees
type SegmentOption<T extends string> = {
  value: T;
  label: ReactNode;
};

// component props: all values use same type, so we do not mix options by mistake
type SegmentedControlProps<T extends string> = {
  value: T;
  options: SegmentOption<T>[];
  onChange: (value: T) => void;
  className?: string;
  itemClassName?: string;
};

// T keeps value/options/onChange in one exact string union type
export function SegmentedControl<T extends string>({
  value,
  options,
  onChange,
  className = '',
  itemClassName = '',
}: SegmentedControlProps<T>) {
  return (
    // wrapper gets base styles + optional custom class
    <div className={`${WRAP_CLASS} ${className}`}>
      {options.map((option) => {
        const active = option.value === value;
        return (
          <button
            key={option.value}
            type="button"
            onClick={() => onChange(option.value)}
            // button class = base + optional item class + state class
            className={`${ITEM_CLASS} ${itemClassName} ${active ? ACTIVE_CLASS : INACTIVE_CLASS}`}
            // active style is inline, inactive uses default styles
            style={active ? ACTIVE_STYLE : undefined}
          >
            {option.label}
          </button>
        );
      })}
    </div>
  );
}
