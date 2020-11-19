# cMoon
Simple command-line app to compute moon phases, sunrise/sunset and positions of planets (RA/dec). The code was originally created to plan for optimal observation and astrophotgraphic "conditions" (other than weather). This app was designed to give rough observation times and dates.

Current version (v0.4) displays(dumps) all calculations when ./cMoon is invoked - initially configured by ../cMoon.ini.

Build:
-----
1) Need 'boost' libraries (libboost-all-dev)

2) Create 'build' directory (for cmake and binary).

3) cd build

4) cmake ..

5) make

NOTE: I don't have 'make install' or Unit-tests, yet.

Invocation:
----------
Linux: use 'build' directory.

Before running the application, configure your location (LAT-LONG cooridnates) using **<repo>/cMoon/cMoon.ini** (or relative directory path: ../cMoon.ini). LAT-LONG coordinates are in decimal-degrees. LAT uses "N" (as positive Latitude) and LONG as "E" (as positive Longitude). For most of US, LAT is positive number and LONG is a negative number ("W"). Default location (if you do not configure ../cMoon.ini) is north of Boston Massachusetts (southern New Hampshire).

Run:

>./cMoon

This will show the following results:

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

- Current version (0.4) displays (more like dumps) all calculations. This could be overwhelming. To limit the output, use:

>**./cMoon -m** for moon phase calculations.

>**./cMoon -r** for moon-rise/sun-rise calculations.

>**./cMoon -s** for sunrise/sunset calculations.

>**./cMoon -p** for planetary calculations. Specific planets, use '-p#' (0=Mercury, 1=Venus, 3=Mars, 4=Jupiter, 5=Saturn, 6=Uranus, 7=Neptune, and 8=Pluto. 2=Earth-prints all)

- If you want to specify the date/time (instead of "now") specify date and/or time:

>**./cMoon YYYY-MM-DD hh:mm:ss**

>NOTE: Date uses "-" and Time uses ":". You can also use Julian date/time:

>**./cMoon JD.ddddd** (where "." is the differentiator). Julian Date is based on the Proleptic Julian calendar (1/1/4713 BC) and not J2000 (Maybe I will fix that by subtracting 2451545). Julian time (decimal portion) assumes UTC noon is zero (0) - not 0.5.

- If you want to see all the internal calculations (curiosity got the best of you), you can put verbose mode:

> **./cMoon -vA** to get all debug levels exposed. For help on debug mode, use **--help verbose**

Features/Bug List:
-----------------
See TODO.txt for details of further development and bug list.





