cMoon - TODO
------------

[ ] - Next Moon Phase should be selectable:
      [x] - *Default "Next Phase" should use current phase and compute next*
      [ ] - Compute Current Moon phase from equation in Next Moon Phase computation
      [ ] - Forward or backward Moon cycles (+# or -#)
[ ] - Interactive (not command line processing) of each component of cMoon
      [ ] - process 'gets' from command line
      [ ] - create in-line "help"
      [ ] - create method to do "quick" mode - suppress interactive from command line
      [ ] - bump up version
[ ] - Separate LAT-LONG Alt, Actual, Civilian, Twilight - display settings
      [ ] - INI file for default values
      [ ] - Save INI from command line
[ ] - Need EST/EDT by date for UT-rise/set
      [ ] - Auto-compute timezone (struct tm does not have the capability)
      [ ] - Other than moon,
[ ] - Auto-compute EST/EDT by LAT/LONG and date
[ ] - Formatter for display time : all, (Jd,J2000), UTC, am/pm (12/24hr)
[ ] - Show JD/J2000 in decimal (current), day:hr:min:sec
[ ] - Get rise/set times for planets
      [x] - cMoon arg - select all or single planet
      [ ] - Show Mean and Apparent Sidereal times
      [ ] - Convert quad (ra/dec) approximation for planets (possibly involved)
      [ ] - use sunrise/set for "visible" in addition to rise/set
[ ] - Show RA-Decl for:
      [ ] - Moon - Possibly compute using GPS
      [ ] - Sun - possibly compute using GPS

[ ] - Convert into C-libraries (and sources)

[ ] - Access to Ephemeris:
      [ ] - Need to compute the exact time and update with ephemis (find where to get ephemis)
      [ ] - Add array of date/time

http://www.stargazing.net/kepler

Some Source Codes:
-----------------
https://www.projectpluto.com/source.htm

Test Next Lunar Phase Code:
--------------------------
This is fairly accurate - to the program (AMoon::nextMoonPhase()):
https://www.timeanddate.com/moon/phases/



Moon phase #DNOW# is 1-1-1970 in milliseconds

(((#DNOW#/2551442844-0.228535) + 0.00591997sin(#DNOW#/5023359217+3.1705094)
    +0.017672776sin(#DNOW#/378923968-1.5388144) - 0.0038844429sin(#DNOW#/437435791+2.0017235)
    -0.00041488sin(#DNOW#/138539900-1.236334))%1)

Very good work. Just tested your formula with a rotational version, compared to the astronomical calendar and it works nearly perfect.

Formula modified for the rotational:

(((((#DNOW#/2551442844-0.228535)+0.00591997 * sin(#DNOW#/5023359217+3.1705094)+0.017672776 * sin(#DNOW#/378923968-1.5388144)-0.0038844429 * sin(#DNOW#/437435791+2.0017235)-0.00041488 * sin(#DNOW#/138539900-1.236334))%1) * 360)/2)

In following example i used shadowmaps and the formula should be modified/reversed to:

((((((#DNOW#/2551442844-0.228535)+0.00591997 * sin(#DNOW#/5023359217+3.1705094)+0.017672776 * sin(#DNOW#/378923968-1.5388144)-0.0038844429 * sin(#DNOW#/437435791+2.0017235)-0.00041488 * sin(#DNOW#/138539900-1.236334))%1) * 360)/2)*-1)



---------------------
https://astronomy.stackexchange.com/questions/18073/how-do-i-know-mathematically-rather-than-from-observation-if-a-moon-is-full

See moonphase_formula.odt/

The Moon phases can be defined by the phase angle between the Sun, Moon and Earth; for example, at 0°, the Moon is defined as full, and at 180° it is defined as new. If you want to know how bright the Moon is at a given angle, we would use the phase angle to find the apparent and absolute magnitudes of the Moon.

Absolute magnitude, when referring to illuminated objects (objects that do not produce their own visible light), simply means their apparent magnitude if viewed from 1 AU away. This means it is almost entirely dependent on the object's phase angle. Right now, you're asking about how bright the Moon would seem to a person on Earth, so we'll find the apparent magnitude. The formula to find an illuminated object's apparent magnitude (in the Solar System), if we know its absolute magnitude H, is:

                      2    2
                    (d BS d BO
   m = H + 2.5 log10( ---------
					(     4
					(p(χ)d )
                          0
Where d0 is 1 AU, χ is the phase angle (in radians) and p(χ) is the phase integral (integration of reflected light). dBO is the distance between the observer and the body, dBS is the distance between the Sun and the body, and dOS is the distance between the observer and the Sun. This formula probably looks pretty scary, but it can be simplified with some approximations. First, we can approximate the phase integral as this:
p(χ)=23((1−χπ)cosχ+1πsinχ)
Where χ is the phase angle, in radians. In the Moon's case, we can set HMoon=+0.25 (this is the absolute magnitude during a full moon), dOS=dBS=1 AU and dBO=0.00257

AU. Now we get the formula:

mMoon=0.25+2.5log10(0.002572p(χ))

So now, we've got a formula that approximates the apparent magnitude of the Moon at any given phase angle. However, even though this gives a close approximation, it is not 100% accurate. Astronomers use empirically derived relationships to predict apparent magnitudes when accuracy is required.

Here's a quick script I wrote to calculate the apparent magnitude, given any phase angle: https://jsfiddle.net/fNPvf/33429/
