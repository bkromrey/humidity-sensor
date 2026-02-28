import type { CSSProperties } from 'react';
import { CARD_CLASS, LABEL_CLASS, SUBVALUE_CLASS, VALUE_CLASS, VARIANT_STYLES } from './MetricCard.styles';

// card type to pick right colors
type MetricCardVariant = 'temp' | 'humidity' | 'light';

// text values we show inside one metric card
type MetricCardProps = {
  value: string;
  subValue: string;
  subValueTitle?: string;
  label: string;
  variant: MetricCardVariant;
};

export function MetricCard({ value, subValue, subValueTitle, label, variant }: MetricCardProps) {
  return (
    // style color depends on selected variant
    <div className={CARD_CLASS} style={VARIANT_STYLES[variant] as CSSProperties}>
      <p className={VALUE_CLASS}>{value}</p>
      <p className={SUBVALUE_CLASS} title={subValueTitle}>
        {subValue}
      </p>
      <p className={LABEL_CLASS}>{label}</p>
    </div>
  );
}
