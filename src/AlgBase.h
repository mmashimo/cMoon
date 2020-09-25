/// @file

#pragma once

#include <cmath>

class AlgBase
{
public:
	AlgBase() {}

	static double fpart(const double x);

	static double roundDegrees(double degrees);

	static double degreeConvert(double radians);

	static double radianConvert(double degrees);

	static double cosDegrees(double x);

	static double sinDegrees(double x);

	static double deltaT(double Y);

	static double fnatn2(const double y, const double x);

	// Appromiation Methods
	/// @brief Finds zeros or extremes of three-points
	///
	/// Finds a parabola through three points and returns values of
	/// coordinates of extreme value (xe, ye) and zeros if any (z1, z2)
	/// assumes that the x values are -1, 0, +1
	///
	/// @param[in] ym, y0, yp - previous, current, next
	/// @param[out] xe        - coordniate of symmetry
	/// @param[out] ye        - extreme for interval
	///
	/// @return nz  - 0 - no solution; 1 - normal rise and set; 2 - one of set/rise has no solution
	static int quad(double ym, double y0, double yp, double& xe, double& ye, double& z1, double& z2);

	static double adjustJdForTau(double mjd0, double hour);

	// returns the local siderial time for
	// the mjd and longitude specified
	static double localSiderialTime(double mjd, double glong);

	static double localAltitude(double instant, double ra, double dec);

};