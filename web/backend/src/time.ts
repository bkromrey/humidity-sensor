export function toLocalIsoString(utcDate: Date, timeZone: string): string {
  // Keep local timestamp explicit with timezone offset for UI clarity.
  const dtf = new Intl.DateTimeFormat("sv-SE", {
    timeZone,
    year: "numeric",
    month: "2-digit",
    day: "2-digit",
    hour: "2-digit",
    minute: "2-digit",
    second: "2-digit",
    hourCycle: "h23",
    timeZoneName: "shortOffset",
  });

  const parts = dtf.formatToParts(utcDate);
  const get = (type: string): string => parts.find((p) => p.type === type)?.value ?? "";
  let offset = get("timeZoneName").replace("GMT", "");
  if (!offset) offset = "+00";
  if (/^[+-]\d{1,2}$/.test(offset)) {
    const sign = offset.startsWith("-") ? "-" : "+";
    const hour = offset.slice(1).padStart(2, "0");
    offset = `${sign}${hour}:00`;
  }

  return `${get("year")}-${get("month")}-${get("day")}T${get("hour")}:${get("minute")}:${get("second")}${offset}`;
}
