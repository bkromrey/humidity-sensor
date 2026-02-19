import type { CSSProperties } from 'react';
import { CARD_CLASS, LABEL_CLASS, SUBVALUE_CLASS, VALUE_CLASS, VARIANT_STYLES } from './MetricCard.styles';

type MetricCardVariant = 'temp' | 'humidity' | 'light';

type MetricCardProps = {
  value: string;
  subValue: string;
  label: string;
  variant: MetricCardVariant;
};

export function MetricCard({ value, subValue, label, variant }: MetricCardProps) {
  return (
    <div className={CARD_CLASS} style={VARIANT_STYLES[variant] as CSSProperties}>
      <p className={VALUE_CLASS}>{value}</p>
      <p className={SUBVALUE_CLASS}>{subValue}</p>
      <p className={LABEL_CLASS}>{label}</p>
    </div>
  );
}
