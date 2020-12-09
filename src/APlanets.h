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
/// cMoon is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with cMoon.  If not, see <https://www.gnu.org/licenses/>.
///
#pragma once

#include "AlgBase.h"
#include "ADateTime.h"
#include "ALocation.h"

enum PlanetType : int
{
	All     = -1,
	Mercury = 0,
	Venus,
	Earth,
	Mars,
	Jupiter,
	Saturn,
	Uranus,
	Neptune,
	Pluto
};

/// @brief Elements (coefficients and such) for computing Planetary positions
using PlanetDescriptor = struct structPlanet
{
	const char* planetName;
	const int planetIndex;
	double inclination;   // * i - inclination of plane of orbit
	double ascendingNode; // * o - longitude of ascending node at date
	double perihelion;    // * p - longitude of perihelion at date
	double semiMajorAxis; //   a
	double dailyMotion;   // * n - daily motion
	double eccentricity;  //   e - oblique eccentricity
	double meanLongitude; // * l - mean longitude at date (J2000 meredian)
};

/// @brief Used for general planet/object computation orbiting the Sun
using OrbitPos = struct structOrbit
{
	// Planet/object description
	PlanetDescriptor description;

	// Planet coordinates
	double m_X;
	double m_Y;
	double m_Z;
};


class APlanets : public AlgBase
{
public:
	APlanets(const int planetType = PlanetType::All);

	// APlanets(const DateString& dateString);

	// APlanets(const APlanets& ref);

	virtual ~APlanets();

	// APlanets& operator=(const APlanets& ref);

	/// @brief Compute and displays planet positions from settings in this object
	void computePlanets(const ALocation& location, const ADateTime& procTime);

	void computePlanetPos(const PlanetDescriptor& planet, const double j2000, double& ra, double& dec, double& dist);

	void parseArgs(std::string args);

	static int m_verboseLevel;

private:
	/// @brief Computes a planet's RA/DEC/Alt
	void computeAPlanet(const PlanetDescriptor& planet, const ALocation& location, const double j2000, const double md);

	/// @brief Displays planets of type
	void computePlanets(const ALocation& location, const ADateTime& procTime, int type);

	/// @brief Prints all Planet information
	void printAll(const ALocation& location, const ADateTime& dateTime);

	/// @brief Heliocentric Rectangular Coordinates of Earth (x = 0 is at vernal equinox)
	OrbitPos m_viewPos;
#if 0
	double m_xEarth;
	double m_yEarth;
	double m_zEarth;
#endif
	/// @brief Compute for the planet type
	int m_planetType;
};

