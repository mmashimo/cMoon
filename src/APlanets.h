/// @file
///
/// @brief APlanets class definitions.
///
/// APlanets has computation methods for planetary algorithms.
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


#include "AlgBase.h"
#include "ADateTime.h"
#include "ALocation.h"

typedef struct structPlanet
{
	const char* planetName;
	double inclination;   // * i - inclination of plane of orbit
	double ascendingNode; // * o - longitude of ascending node at date
	double perihelion;    // * p - longitude of perihelion at date
	double semiMajorAxis; //   a
	double dailyMotion;   // * n - daily motion
	double eccentricity;  //   e - oblique eccentricity
	double meanLongitude; // * l - mean longitude at date (J2000 meredian)
} Planets;



class APlanets : public AlgBase
{
public:
	APlanets();

	// APlanets(const DateString& dateString);

	// APlanets(const APlanets& ref);

	virtual ~APlanets();

	// APlanets& operator=(const APlanets& ref);

	void computePlanets(const ALocation& location, const ADateTime& procTime, int type = -1);

	void computePlanetPos(const int type, const double j2000, double& ra, double& dec, double& dist);

	static int m_verboseLevel;

private:
	/// @brief Computes a planet's RA/DEC/Alt
	void computeAPlanet(const ALocation& location, const double j2000, const double md, int type);

	/// @brief Heliocentric Rectangular Coordinates of Earth (x = 0 is at vernal equinox)
	double m_xEarth;
	double m_yEarth;
	double m_zEarth;

};

