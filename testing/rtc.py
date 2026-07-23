import rtc

r = rtc.RTC()    # Get reference to the onboard RTC

# READ current time
now = r.datetime
print(now)       # time.struct_time(tm_year=2000, tm_mon=1, tm_mday=1, ...)

# WRITE to set the time
r.datetime = time.struct_time(
    tm_year=2026,   # Year (4-digit)
    tm_mon=7,       # Month (1-12)
    tm_mday=14,     # Day (1-31)
    tm_hour=16,     # Hour (0-23)
    tm_min=20,      # Minutes (0-59)
    tm_sec=0,       # Seconds (0-59)
    tm_wday=1,      # Weekday (0=Mon, 6=Sun) — often ignored by assignment
    tm_yday=195,    # Day of year (1-366) — often ignored by assignment
    tm_isdst=-1     # DST flag: -1=unknown, 0=no, 1=yes
)