# cMoon
Simple command-line app to compute moon phases, sunrise/sunset and positions of planets (RA/dec). The code was originally created to plan for optimal observation and astrophotgraphic "conditions" (other than weather). This was not supposed to be precise.

Current version (v0.2) displays(dumps) all calculations when ./cMoon is invoked.

See TODO.md for details of further development.

Invocation:
----------
>./cMoon

This will show the following results - the location is north of Boston Massachusetts:

>Date and time in UTC

>Moon phase - as days of New Moon, phase, percentage of phase, day to next phase

>Moonrise/moonset (UTC of local area)

>Sunrise/sunset (UTC of local area)

>Nautical twilight (UTC of local area)

>Next moon-phase (from today)

>Sunrise/Sunset - using sidereal time computation (more accurate than above)

>Position, distance and altitude of planets (including Pluto)

Command Line Arguments and Program Adjustments:
----------------------------------------------

- Quick way of getting "help": **./cMoon -h**. This will give you all the options.

- Current version (0.2) displays (more like dumps) all calculations. This could be overwhelming. To limit the output, use:

>**./cMoon -m** for moon phase calculations.

>**./cMoon -r** for moon-rise/sun-rise calculations.

>**./cMoon -s** for sunrise/sunset calculations.

- Getting LAT-LONG to computation. Unless you are in southern New Hampshire, you will want to specify your 'location'. LAT-LONG coordinates are in decimal-degrees. LAT defaults to "N" (positive LAT) and LONG to "E" (positive LONG). Default location (until I can get the INI file setup) is LAT 42.81751, LONG -71.2781. You can change this by:

> **./cMoon --latlong LAT.dddd -LONG.dddd --zone -5**  (note: "-" can be replaced with "W" - think it works. Decimal degrees. Elevation is not settable, yet - working on that).

- If you want to specify the date/time (instead of "now"): **./cMoon YYYY-MM-DD hh:mm:ss**  (note: date uses "-" and time uses ":"). You can also use Julian date/time: **./cMoon JD.ddddd** (where "." is the differentiator)

- If you want to see all the internal calculations (but why?), you can put verbose mode:

> **./cMoon -vA** to get all debug levels exposed. For help on debug mode, use **--help verbose**






