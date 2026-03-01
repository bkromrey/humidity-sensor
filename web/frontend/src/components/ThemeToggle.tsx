import {
  HIDE_ON_HOVER_CLASS,
  ICON_CLASS_BASE,
  MOON_ICON_CLASS,
  MOON_PATH,
  SHOW_ON_HOVER_CLASS,
  STROKE_COMMON,
  STROKE_WIDTH,
  SUN_ICON_CLASS,
  SUN_RAYS_PATH,
  SVG_CLASS,
} from './ThemeToggle.styles';

type ThemeToggleProps = {
  isDarkMode: boolean;
  onToggle: () => void;
};

export function ThemeToggle({ isDarkMode, onToggle }: ThemeToggleProps) {
  // show moon in dark mode, sun in light mode
  const shownThemeIcon = isDarkMode ? 'moon' : 'sun';

  return (
    <button
      type="button"
      onClick={onToggle}
      aria-label={isDarkMode ? 'Switch to light mode' : 'Switch to dark mode'}
      title={isDarkMode ? 'Switch to light mode' : 'Switch to dark mode'}
      className={`${ICON_CLASS_BASE} ${shownThemeIcon === 'moon' ? MOON_ICON_CLASS : SUN_ICON_CLASS}`}
    >
      <svg
        viewBox="0 0 24 24"
        className={`${SVG_CLASS} ${isDarkMode ? 'opacity-90' : 'opacity-100'}`}
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
      >
        {shownThemeIcon === 'moon' ? (
          <>
            <path
              className={HIDE_ON_HOVER_CLASS}
              d={MOON_PATH}
              stroke={STROKE_COMMON}
              strokeWidth={STROKE_WIDTH}
              strokeLinecap="round"
              strokeLinejoin="round"
            />
            <g className={SHOW_ON_HOVER_CLASS}>
              <circle cx="12" cy="12" r="4.2" stroke={STROKE_COMMON} strokeWidth={STROKE_WIDTH} />
              <path d={SUN_RAYS_PATH} stroke={STROKE_COMMON} strokeWidth={STROKE_WIDTH} strokeLinecap="round" />
            </g>
          </>
        ) : (
          <>
            <g className={HIDE_ON_HOVER_CLASS}>
              <circle cx="12" cy="12" r="4.2" stroke={STROKE_COMMON} strokeWidth={STROKE_WIDTH} />
              <path d={SUN_RAYS_PATH} stroke={STROKE_COMMON} strokeWidth={STROKE_WIDTH} strokeLinecap="round" />
            </g>
            <path
              className={SHOW_ON_HOVER_CLASS}
              d={MOON_PATH}
              stroke={STROKE_COMMON}
              strokeWidth={STROKE_WIDTH}
              strokeLinecap="round"
              strokeLinejoin="round"
            />
          </>
        )}
      </svg>
    </button>
  );
}
