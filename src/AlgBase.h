/// @file
///
/// @brief AlgBase class definitions.
///
/// AlgBase has all the computation methods for positional algorithms.
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
///

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