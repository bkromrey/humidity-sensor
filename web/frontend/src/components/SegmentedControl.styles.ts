import type { CSSProperties } from 'react';

export const WRAP_CLASS =
  'inline-grid rounded-full border border-[var(--line)] bg-[var(--soft)] p-1 text-sm sm:text-base';
export const ITEM_CLASS =
  'w-full rounded-full px-6 py-2 text-center font-semibold transition sm:px-8 flex items-center justify-center';
export const ACTIVE_CLASS = 'shadow-[inset_0_1px_0_rgba(255,255,255,0.12)]';
export const INACTIVE_CLASS = 'text-[var(--text-soft)] hover:bg-[var(--switch-hover)]';
export const ACTIVE_STYLE: CSSProperties = {
  background: 'var(--switch-active-bg)',
  color: 'var(--switch-active-text)',
};
