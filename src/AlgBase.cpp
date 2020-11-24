/// @file
///
/// @brief Algbase class implementation.
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
/// cMoon is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with cMoon.  If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"
#include <cstdio>
#include <iostream>
#include <math.h>
#include <string.h>

#include "AlgBase.h"

double AlgBase::fpart(const double x)
{
    // returns fractional part of a number
    double part = x - floor(x);
    if (part < 0)
    {
        part = part + 1.;
    }
    return part;
}

double AlgBase::roundDegrees(double degrees)
{
    return degrees - (floor(degrees / 360.) * 360.);
}

double AlgBase::degreeConvert(double radians)
{
    double degrees = radians * 180. / (atan(1.) * 4.);
    return degrees;
}

double AlgBase::radianConvert(double degrees)
{
    double Winkel = degrees - (floor(degrees / 360.) * 360.);
    return Winkel * atan(1.) * 4. / 180.;
}

double AlgBase::cosDegrees(double x)
{
    return cos(x * .0174532925199433);
}

double AlgBase::sinDegrees(double x)
{
    return sin(x * .0174532925199433);
}

double AlgBase::deltaT(double Y)
{
    // Dim T, Tage
    double T = (Y - 2000.) / 100;
    double DELTAT, Tage;

    if (Y < 948)
    {
        DELTAT = 2715.6 + (573.36 * T) + (46.5 * pow(T, 2.));
    }
    else if ((Y >= 948) && (Y <= 1600))
    {
        DELTAT = 50.6 + (67.5 * T) + (22.5 * pow(T, 2.));
    }
    else if ((Y >= 1800) && (Y < 1900))
    {
        T = (Y - 1900) / 100;

        Tage = -0.000009 + (0.003844 * T) + (0.083563 * pow(T, 2.)) + (0.865736 * pow(T, 3.))
            + (4.867575 * pow(T, 4.)) + (15.845535 * pow(T, 5.)) + (31.332267 * pow(T, 6.))
            + (38.291999 * pow(T, 7.)) + (28.316289 * pow(T, 8.)) + (11.636204 * pow(T, 9.))
            + (2.043794 * pow(T, 10.));
        DELTAT = Tage * 86400;
    }
    else if ((Y >= 1900) && (Y < 1988))
    {
        T = (Y - 1900) / 100;
        Tage = -0.00002 + (0.000297 * T) + (0.025184 * pow(T, 2.)) - (0.181133 * pow(T, 3.))
            + (0.55304 * pow(T, 4.)) - (0.861938 * pow(T, 5.)) + (0.677066 * pow(T, 6.))
            - (0.212591 * pow(T, 7.));
        DELTAT = Tage * 86400;
    }
    else // if ((Y >= 1988) && (Y < 2051))
    {
        DELTAT = (Y - 1990) * 6.6 / 9 + 56.86;
    }

    return DELTAT;
}

int AlgBase::quad(double ym, double y0, double yp, double& xe, double& ye, double& z1, double& z2)
{
    bool computed{ false };

    // Clear output
    xe = 0;
    ye = 0;
    z1 = 0;
    z2 = 0;

    int nz = 0;

    double a = .5 * (yp + ym) - y0;
    double b = .5 * (yp - ym);
    double c = y0;

    xe = -b / (2 * a);               // x coord of symmetry line
    ye = (a * xe + b) * xe + c;      // extreme value for y in interval

    double dis = b * b - 4 * a * c;  // discriminant

    double dx;
    if (dis > 0)                     // intersects x axis!
    {
        dx = .5 * sqrt(dis) / fabs(a);
        z1 = xe - dx;
        z2 = xe + dx;
        if (fabs(z1) <= 1)
            nz++;
        if (fabs(z2) <= 1)
            nz++;
        if (z1 < -1)
            z1 = z2;
    }

    return nz;
}

double AlgBase::adjustJdForTau(double mjd0, double hour)
{
	double instant = mjd0 + hour / 24.;
	return (instant - 51544.5) / 36525.;
}

double AlgBase::fnatn2(const double y, const double x)
{
    // the atn2 function below returns an angle in the range 0 to two pi
    // depending on the signs of x and y.

    double a = atan(y / x);
    if (x < 0)
    {
        a = a + M_PI;
    }
    if ((y < 0) && (x > 0))
    {
        a = a + (M_PI * 2);
    }

    return a;
}

// returns the local siderial time for the mjd and longitude specified
double AlgBase::localSiderialTime(const double mjd, const ALocation& location)
{
	double ut = fpart(mjd) * 24.;
	double t = (floor(mjd) - 51544.5) / 36525.;
	double gmst = 6.697374558 + 1.0027379093 * ut;
	gmst = gmst + (8640184.812866 + (.093104 - .0000062 * t) * t) * t / 3600;

	double glong = -location.longitude();  // routines use east longitude negative convention

    double lst = 24 * fpart((gmst - (glong / 15)) / 24);

    // TODO: Need to print out LST

	return lst;
}


double AlgBase::localAltitude(const ALocation& location, const double instant, double ra, double dec)
{
	double glat = location.latitude();

	double sphi = sinDegrees(glat);
	double cphi = cosDegrees(glat);

	double tau = 15. * (localSiderialTime(instant, location) - ra);   // 'hour angle of object

    // TODO: Need to print out Altitude

	return sphi * sinDegrees(dec) + cphi * cosDegrees(dec) * cosDegrees(tau);
}

void AlgBase::convertJulianToDate(const double jd, int& yr, int& mon, int& day)
{
    // https://aa.usno.navy.mil/faq/docs/JD_Formula.php

    // Convert JD in AD
    int l = (int)(jd + 68569);
    int n = (4 * l) / 146097;
    l = l - (146097 * n + 3) / 4;
    int i = 4000 * (l + 1) / 1461001;
    l = l - 1461 * i / 4 + 31;
    int j = 80 * l / 2447;
    int k = l - 2447 * j / 80;
    l = j / 11;
    j = j + 2 - 12 * l;
    i = 100 * (n - 49) + i + l;

    yr = i;
    mon = j;
    day = k;
}

double AlgBase::convertJulianToTime(const double jd, int& hour, int& min, int& sec)
{
    double H = (jd - floor(jd)) * 24.;
    hour = static_cast<int>(floor(H));

    double M = (H - floor(H)) * 60.;
    min = static_cast<int>(floor(M));

    double fSeconds = (M - floor(M)) * 60.;
    sec = static_cast<int>(fSeconds);

    return fSeconds;
}

double AlgBase::convertTimeToJulian(const int hour, const int min, const int sec)
{
    double jd = 0.;

    jd += static_cast<double>(hour) / 24.;
    jd += static_cast<double>(min) / 1440.;
    jd += static_cast<double>(sec) / 86400.;

    return jd;
}

double AlgBase::convertDateToJulianNoon(const int Y, const int M, const int D)
{
    // Example of Julian Days is: 2017-3-1 should be 2457813.5
    // https://www.subsystems.us/uploads/9/8/9/4/98948044/moonphase.pdf

    // (367*y)-int(7*(y+int((m+9)/12))/4)+int(275*m/9)+d+1721013.5
    // 2001-12-05 should be 2452248.5 (734367 - 3506 + 366 + 5 + 1721013.5)
    int a = 367 * Y;
    int b1 = (M + 9) / 12;
    int b = 7 * (Y + b1) / 4;
    int c = 275 * M / 9;
    double jd = a - b + c + D + 1721013.5;

    return jd;
}
