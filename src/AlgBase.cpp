/// @file

#include "pch.h"
#include <cstdio>
#include <iostream>
#include <math.h>
#include <string.h>

#include "AlgBase.h"
#include "ALocation.h"

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

// returns the local siderial time for
// the mjd and longitude specified
double AlgBase::localSiderialTime(double mjd, double glong)
{
	double ut = fpart(mjd) * 24.;
	double t = (floor(mjd) - 51544.5) / 36525.;
	double gmst = 6.697374558 + 1.0027379093 * ut;
	gmst = gmst + (8640184.812866 + (.093104 - .0000062 * t) * t) * t / 3600;

	return 24 * fpart((gmst - (glong / 15)) / 24);
}


double AlgBase::localAltitude(double instant, double ra, double dec)
{
	double glong = -ALocation::m_longitude;  // routines use east longitude negative convention
	double glat = ALocation::m_latitude;

	double sphi = sinDegrees(glat);
	double cphi = cosDegrees(glat);

	double tau = 15. * (localSiderialTime(instant, glong) - ra);   // 'hour angle of object

	return sphi * sinDegrees(dec) + cphi * cosDegrees(dec) * cosDegrees(tau);
}



