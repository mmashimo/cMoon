/// @file
///
/// @brief ASun class implementation.
///
/// @copyright 2019-2020 M.Mashimo and licensors. All Right Reserved.
///
/// This file is part of cMoon application.
///
/// cMoon is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// any later version.
///
/// Foobar is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with Foobar.  If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"

#include <iostream>
#include <cstdio>
#include <cmath>
#include <iomanip>
#include <stdlib.h>

#include "ASun.h"
#include "ALocation.h"

int ASun::m_verboseLevel = 1;

ASun::ASun()
{

}


void ASun::setVerboseMode(const int level)
{
    m_verboseLevel = level;
}


// Computation of sunrise/sunset
void ASun::showSun(const ADateTime& procTime)
{
	ADateTime dateTime(procTime);

	std::cout << "\n-----------------Sunrise-Sunset------------------" << std::endl;

	// Temporary use of struct tm to get time

	struct tm jTime = dateTime.m_timeStruct;

	// sunrise equation is cos w0 = -tan phi x tan delta
	// w0 is the hour andle at sunrise (negative) sunset (positive)
	// phi is the latitude of the observer on Earth
	// delta is the sun declination

	// Earth rotate as an angular velocity of 15degrees/hour. 'w0 x 15degrees / hour' is the local solar noon
	// phi is 0 at equator, positive for Nother Hemisphere.
	// delta is 0 at vernal and autumnal equnoxes when sun is exactly above the equator; positive in Northern summer
	//    and negative at Northern hemisphere winter.

	// Atmospheric refraction - lifts the solar disc be approx 0.6 degrees on the horizon (solar disc is 0.5degrees)
	// cos(w0) = ( sin(alpha) - sin(phi) x sin(delta) ) / ( cos(phi) x cos(delta) )

	// alpha is altitude of the center of solar disc set to about -0.83degrees (or -50 arcminuntes).

	// 0.0008 is Julian seconds
	// Jnoon is the number of days since 2000-1-1-12:00pm (noon) = 2451545.
	// double Jnoon = jd - 2451545. + 0.0008;
	// NOTE: Our Julian already computes the UTC time. We need to add the 12-noon (0.5)
	double Jnoon = floor(dateTime.m_julian) - 2451544.5 + 0.0008;

	// TT was set to 32.184 seconds laggin TAI on January 1958. By 1972, when leap seconds were introduced, 10 sec were added.
	// By Jan 1, 2017, 27 more seconds were added comin to the total of 68.184 sec.
	// This is computed as 68.184 / 86400 or 0.0008 with DUT1 (UT1 - UTC)

	// Mean solar noon
	double Jmean = Jnoon - (ALocation::m_longitude / 360.);

	if (m_verboseLevel & DebugComputation)
	{
		std::cout << "Jnoon = " << Jnoon << " Jmean = " << Jmean << std::endl;
	}

	// Jmean is an approximation of the mean solar time at noon (Jnoon) as Julian date with the day fraction.
	// lw (LONG) is the longitude west in decimal degrees (in US, longitude is negative, east in Europe is positive) of observer.

	// Solar mean anomaly (M) - in degrees, for sin/cos, use rads (Mrd) - Need conversion as degrees/time
	// M = (357.5291 + (0.98560028 * Jmean)) MOD 360.;
	// https://en.wikipedia.org/wiki/Mean_anomaly
	double M = roundDegrees(357.5291 + (0.98560028 * Jmean));
	double Mrad = radianConvert(M);

	if (m_verboseLevel & DebugComputation)
	{
		std::cout << "Mean anomaly: " << M << " rad: " << Mrad << std::endl;
	}

	// Equation of the center (C) (need to use radianConvert) - used to calculate lambda
	// https://en.wikipedia.org/wiki/Equation_of_the_center
	// C = 1.9148 * sin(M) + 0.0200 * sin(2*M) + 0.0003 * sin(3*M)
	// double C = (1.9148 * sin(Mrad)) + (0.0200 * sin(radianConvert(2*M))) + (0.0003 * sin(radianConvert(3*M)));
	double C = (1.9148 * sin(Mrad)) + (0.0200 * sin(2 * Mrad)) + (0.0003 * sin(3 * Mrad));
	// 1.9148 is the coefficient of the equation of the center for the planet the observer is on (earth)

	// Ecliptic longitude (lambda) - in degrees
	// https://en.wikipedia.org/wiki/Ecliptic_coordinate_system#Spherical_coordinates
	// lambda = (M + C + 180 + 102.9372) % 360;
	double lambda = roundDegrees(M + C + 180. + 102.9372);
	double lrad = radianConvert(lambda);

	if (m_verboseLevel & DebugComputation)
	{
		// 102.9372 is the value for the argument of perihelion.
		std::cout << "Equation of Center: " << C << " degrees." << std::endl;
		std::cout << " Ecliptic longitude (lambda)  = " << lambda << " rad:" << lrad << std::endl;
	}

	// Solar Transit
	// Jtransit = 2451545.0 + Jmean + 0.0053 * sin(M) - 0.0069 * sin(2 * lambda);
	double equTime = (0.0053 * sin(Mrad)) - (0.0069 * sin(2 * lrad));
	double Jtransit = 2451545.0 + Jmean + equTime;
	// where: Jtransit is the julian date for the local true solar transit for solar noon)
	// 0.0053sinM - 0.0069sin2lambda is the simplified version of the equation of time.
	// https://en.wikipedia.org/wiki/Equation_of_time
	// The coefficients are fractional day minutes.
	if (m_verboseLevel & DebugComputation)
	{
		std::cout << "Jtransit = " << Jtransit << "  Equation of Time: " << equTime << " -- ";
	}

	dateTime.convertJulianToTime(fabs(equTime), jTime);

	// Declination of the Sun (delta)
	// sin(delta) = sin(lambda) * sin(23.44)
	double radDelta = asin(sin(lrad) * sin(radianConvert(23.44)));

	if (m_verboseLevel & DebugComputation)
	{
		std::cout << "Solar eclination (delta) = " << radDelta << " degrees: " << degreeConvert(radDelta) << std::endl;
	}

	// delta is the declination of the sun. arc-sin needed to get the declination in degrees.
	// 23.44 degrees is the Earth's maximum axial tilt towards the sunrise

	// Hour Angle (w0)
	// https://en.wikipedia.org/wiki/Hour_angle
	// cos(w0) = ( sin(-0.83) - sin(phi) * sin(delta) ) / ( cos(phi) * cos(delta) )

	// where w0 is the hour angle from the observers's zenith
	// phi is the north latitude of the observer (+ for north; negative for south)

	// Elevation Correction - added to -0.83 degress:
	// -1.15 degrees * sqrt(elevation_in_feet) / 60 degrees
	// or
	// -2.076 degrees * sqrt(elevation_in_meters)/60degrees
	//
	// At 10,000 feet, -115/60 or -1.92 + -0.83 or -2.75
	// double elev = -1.15 * sqrt(ELEV) / 60.;
	double elev = -1.15 * sqrt(ALocation::m_elevation) / 60.;

	// double twilightAdjust = -12.;
	// double radHorizon = radianConvert(-0.83 - elev + twilightAdjust);
	double radHorizon = radianConvert(-0.83 + elev);  // At actual twilight

	double phi = radianConvert(ALocation::m_latitude);

	double radw0 = acos((sin(radHorizon) - sin(phi) * sin(radDelta)) / (cos(phi) * cos(radDelta)));
	double w0 = degreeConvert(radw0);

	if (m_verboseLevel & DebugComputation)
	{
		std::cout << "Hour angle w0 = " << w0 << " (radian = " << radw0 << ")" << std::endl;
	}

	// Calculate sunrise and sunset:
	double Jrise = Jtransit - w0 / 360.;
	double Jset = Jtransit + w0 / 360.;
	double tzTime = dateTime.m_timeZone / 24.;
	double tmpJdTime;

	char *tzStr = getenv("TZ");
	if (tzStr != nullptr)
	{
		std::cout << "\nTimezone = " << tzStr << std::endl;
	}

	if (ADateTime::m_verboseLevel & DebugComputation)
	{
		std::cout << "Sunrise: ";
	}

	dateTime.convertJulianToTime(Jrise, jTime);
	std::cout << "Sunrise: " << dateTime.asString(jTime, "(%c UTC) ");

	tmpJdTime = Jrise + tzTime;
	dateTime.convertJulianToTime(tmpJdTime, jTime);
	time_t tmpTime = mktime(&jTime);

	std::cout << std::put_time(localtime(&tmpTime), "%F %T %Z %z") << std::endl;

	if (ADateTime::m_verboseLevel & DebugComputation)
	{
		std::cout << "Solar noon: ";
	}
	dateTime.convertJulianToTime(Jtransit, jTime);
	std::cout << "Solar noon: " << dateTime.asString(jTime, "(%c UTC) ");

	tmpJdTime = Jtransit + tzTime;
	dateTime.convertJulianToTime(tmpJdTime, jTime);
	tmpTime = mktime(&jTime);
	std::cout << std::put_time(localtime(&tmpTime), "%F %T %Z %z") << std::endl;

	if (ADateTime::m_verboseLevel & DebugComputation)
	{
		std::cout << "Sunset: ";
	}
	dateTime.convertJulianToTime(Jset, jTime);
	std::cout << "Sunset: " << dateTime.asString(jTime, "(%c UTC) ");

	tmpJdTime = Jset + tzTime;
	dateTime.convertJulianToTime(tmpJdTime, jTime);
	tmpTime = mktime(&jTime);
	std::cout << std::put_time(localtime(&tmpTime), "%F %T %Z %z") << std::endl;

	// NOTE: These are all Julian date and franctional time

	std::cout << "-------------------------------------------------" << std::endl << std::endl;

}


void sunPosition(const double jd, double& ra, double& decl)
{
	// Position of the Sun at 11:00 UT on 1997 August 7th
	// 1. Find the days before J2000.0 (d) from the table

	double d = 11/24 + 212 + 7 - 1096.5; // = -877.04167

	// 2. Find the Mean Longitude (L) of the Sun

	double L = 280.461 + 0.9856474 * d; // = -583.99284 + 720
	// (add multiples of 360 to bring in range 0 to 360)
	// = 136.00716

	// 3. Find the Mean anomaly (g) of the Sun

	double g = 357.528 + 0.9856003 * d;
		// = -506.88453 + 720
		// = 213.11547

	// 4. Find the ecliptic longitude (lambda) of the sun

	double lambda = L + 1.915 * sin(g) + 0.020 * sin(2*g);
		// = 134.97925

	// (note that the sin(g) and sin(2*g) terms constitute an 
	//	approximation to the 'equation of centre' for the orbit 
	//	of the Sun)

	double beta = 0;
	// (by definition as the Sun's orbit defines the
	// ecliptic plane. This results in a simplification
	// of the formulas below)

	// 5. Find the obliquity of the ecliptic plane (epsilon)

	double epsilon = 23.439 - 0.0000004 * d;	// = 23.439351

	// 6. Find the Right Ascension (alpha) and Declination (delta) of the Sun

	double Y = cos(epsilon) * sin(lambda);
	double X = cos(lambda);

	double a = atan(Y/X); // a = arctan(Y/X);

	// If X < 0 then alpha = a + 180
	// If Y < 0 and X > 0 then alpha = a + 360
	// else alpha = a
	double alpha = a;

	if (X < 0)
		alpha = a + 180;

	if ((Y < 0) && (X > 0))
		alpha = a + 360;
	else
		alpha = a;

	// Y =  0.6489924
	// X = -0.7068507
	// a = -42.556485
	// alpha = -42.556485 + 180 = 137.44352 (degrees)
	
	// delta = arcsin(sin(epsilon)*sin(lambda)) = 16.342193 degrees
	double delta = asin(sin(epsilon)*sin(lambda));

	// Final result

	// Right ascension is usually given in hours of time, and both
	// figures need to be rounded to a sensible number of decimal
	// places. 

	//alpha =   9.163 hrs      or   9h 09m 46s
	//delta = +16.34 degrees   or +16d 20' 32"

	//The Interactive Computer Ephemeris gives

	//alpha =   9h 09m 45.347s and
	//delta = +16d 20' 30.89"

}

