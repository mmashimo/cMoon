/// @file
///
/// @brief APlanet class implementation.
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


#include <iostream>

#include <cmath>
#include <vector>

#include "APlanets.h"

// static constexpr double pi{3.14159265358979323846};

static double twoPi = 2. * M_PI;
static double rads = M_PI / 180.;
static double degs = 180. / M_PI;

static constexpr double elementsDate{2450680.5};  // date of elements
static constexpr double eclipticDate{2451545.};   // date of mean ecliptic and equinox of

/// @brief Coefficients for computing Planet orbits
static std::vector<PlanetDescriptor> planetDescrip
{
	{"Mercury", 0, 7.00507 * rads, 48.3339 * rads, 77.45399999999999*rads, 0.3870978, 4.092353*rads, 0.2056324, 314.42369 * rads},
	{"Venus"  , 1, 3.39472 * rads, 76.6889 * rads,  131.761 * rads,  0.7233238, 1.602158 * rads,   0.0067933, 236.94045 * rads},
	{"Earth"  , 2, 0.00041 * rads, 349.2 * rads,    102.8517 * rads, 1.00002,   0.9855796 * rads,  0.0166967, 328.40353 * rads},
	{"Mars"   , 3, 1.84992 * rads, 49.5664 * rads,  336.0882 * rads, 1.5236365, 0.5240613 * rads,  0.0934231, 262.42784 * rads},
	{"Jupiter", 4, 1.30463 * rads, 100.4713 * rads, 15.6978 * rads,  5.202597,  0.08309618 * rads, 0.0484646, 322.55983 * rads},
	{"Saturn" , 5, 2.48524 * rads, 113.6358 * rads, 88.863 * rads,   9.571899999999999, 0.03328656 * rads, 0.0531651, 20.95759 * rads},
	{"Uranus" , 6, 0.77343 * rads, 74.0954 * rads,  175.6807 * rads, 19.30181,  0.01162295 * rads, 0.0428959, 303.18967 * rads},
	{"Neptune", 7, 1.7681 * rads,  131.7925 * rads, 7.206 * rads,    30.26664,  0.005919282 * rads,0.0102981, 299.8641 * rads },
	{"Pluto"  , 8, 17.12137 * rads,110.3833 * rads, 224.8025 * rads, 39.5804,   0.003958072 * rads,0.2501272, 235.7656 * rads }
};

int APlanets::m_verboseLevel = 0;

/// @brief Constructor
APlanets::APlanets(const int planetType)
	: AlgBase()
	, m_xEarth{0}
	, m_yEarth{0}
	, m_zEarth{0}
	, m_planetType(planetType)
{
	// Nothing here
}

/// @brief destructor
APlanets::~APlanets()
{
	// Nothing here
}

void APlanets::parseArgs(std::string options)
{
	if (isdigit(options[0]))
	{
		// Use index to planets
		m_planetType = atoi(options.c_str());
	}
	else
	{
		switch(options[0])
		{
			default:
			case 'a':  // All planets 
			case 'A':
			case '*':
				m_planetType = -1;
				break;
			case 'j':
			case 'J':
				m_planetType = 4;
				break;
			case 'm':  // Could be Mercury or Mars. If 'M' alone, Mercury
			case 'M':
				if ((options[1] == '\0') || (options[1] == 'e') || (options[1] == 'E'))
				{
					m_planetType = 0;   // "M", "Me", "ME"
				}
				else
				{	// Any extension assume Mars
					m_planetType = 3;   // "Ma" or "MA"
				}
				break;
			case 'n':  // Neptune
			case 'N':
				m_planetType = 7;
				break;
			case 'p':  // Pluto
			case 'P':
				m_planetType = 8;
				break;
			case 'r':  // Mars if no "M"
			case 'R':
				m_planetType = 3;
				break;
			case 's':  // Saturrn
			case 'S':
				m_planetType = 5;
				break;
			case 'u':  // Uranus
			case 'U':
				m_planetType = 6;
				break;
			case 'v':  // Venus
			case 'V':
				m_planetType = 1;
				break;

		}
	}
}


/// @brief Computes True Anomaly
///
/// @param[in] meanAnomaly - the mean anomaly in radians
/// @param[in] eccentricity - the eccentricity of the orbit
/// @param[in] eps - the convergence paramter (8 or 9 is usually fine, 12 or 14 for very accurate work)
///
/// @return double True Anomaly
static double computeTrueAnomaly(const double meanAnomaly, const double eccentricity, const double eps)
{

//   returns the true anomaly given
//   m - the mean anomaly in radians
//   ecc - the eccentricity of the orbit
//   eps - the convergence paramter (8 or 9 is usually fine
//   12 or 14 for very accurate work)

	double e = meanAnomaly;
	double delta = .05;
	double convergence = pow(10., -eps);
	while(fabs(delta) >= convergence)
	{
		// DO WHILE ABS(delta) >= 10 ^ -eps
		// delta = e - ecc * SIN(e) - m
		// e = e - delta / (1 - ecc * COS(e))
		// LOOP
		delta = e - (eccentricity * sin(e)) - meanAnomaly;
		e = e - (delta / (1. - (eccentricity * cos(e))));
	}

	// v = 2 * ATN(((1 + ecc) / (1 - ecc)) ^ .5 * TAN(.5 * e))
	double ec = (1. + eccentricity) / (1. - eccentricity);
	double v = 2 * atan(pow(ec, .5) * tan(.5 * e));

	// IF v < 0 THEN v = v + tpi
	if (v < 0)
	{
		v += twoPi;
	}

	if (APlanets::m_verboseLevel & DebugComputation)
	{
		std::cout << "Compute True Anomaly=" << v << " from: mean anomaly=" << meanAnomaly << " eccentricity=" << eccentricity << " eps=" << eps << std::endl;
	}

	return v;

}

static double angleInRange(const double x)
{
	double b = x / twoPi;
	double a = twoPi * AlgBase::fpart(b);
	if (a < 0.)
	{
		a += twoPi;
	}
	return a;
}

static void findPosition(const PlanetDescriptor& planet, const double d, double& xh, double& yh, double& zh)
{
	// Position of planet in its orbit
	double eldate = elementsDate - eclipticDate;
	double pp = planet.perihelion;

	double mp = angleInRange((planet.dailyMotion * (d - eldate)) + planet.meanLongitude - pp);

	double ep = planet.eccentricity;
	double op = planet.ascendingNode;
	double ap = planet.semiMajorAxis;
	double ip = planet.inclination;

	double vp = computeTrueAnomaly(mp, ep, 12);


	double rp = ap * (1 - (ep * ep)) / (1 + (ep * cos(vp)));
	double vep = vp + pp;

	if (planet.planetIndex == PlanetType::Earth)
	{
		// Heliocentric coords of earth
		// double vep = vp + pp;
		xh = rp * cos(vep);
		yh = rp * sin(vep);
		zh = 0.;

		std::cout << "Coord of Earth: X=" << xh << " Y=" << yh << std::endl;

		if (APlanets::m_verboseLevel & DebugComputation)
		{
			std::cout << "Earth mean anomaly= " << mp << " true anomaly=" << vp << " radius pos=" << rp << " radian vector=" << vep << std::endl;
		}
	}
	else
	{
		// heliocentric rectangular coordinates of planet
		// double vep = vp + pp - op;
		vep -= op;
		xh = rp * (cos(op) * cos(vep) - sin(op) * sin(vep) * cos(ip));
		yh = rp * (sin(op) * cos(vep) + cos(op) * sin(vep) * cos(ip));
		zh = rp * (sin(vep) * sin(ip));

		std::cout << "Coord of " << planet.planetName << ": X=" << xh << " Y=" << yh << " Z=" << zh << std::endl;

		if (APlanets::m_verboseLevel & DebugComputation)
		{
			std::cout << planet.planetName << " mean anomaly= " << mp << " true anomaly=" << vp << " radius pos=" << rp << " radian vector=" << vep << std::endl;
			std::cout << planet.planetName << " inclination= " << ip << " ascending node=" << ip << " perihelion=" << pp << " semi-Major axis=" << ap << std::endl;
		}
	}
}

static void printDegrees(char* str, const double deg)
{
	// cosmetic function returns angular values as a made up decimal
	// number  - ddd.mm - the digits after the decimal point are minutes.
	double x = (deg < 0.) ? -deg : deg;
	double a = floor(x);
	double b = 60. * (x - a);
	double e = floor(b);
	double s = floor((b - e) * 60);

	// deal with carry on minutes
	if (deg < 0.)
	{
		sprintf(str, "-%02d:%02d:%02d (%f)", (int)a, (int)e, (int)s, deg);
	}
	else
	{
		sprintf(str, "%02d:%02d:%02d (%f)", (int)a, (int)e, (int)s, deg);
	}
}

// RA, DEC are in degrees
void APlanets::computePlanetPos(const PlanetDescriptor& planet, const double j2000, double& ra, double& dec, double& dist)
{
	double x{0.};
	double y{0.};
	double z{0.};

	// Planet's position - use internal variables
	findPosition(planet, j2000, x, y, z);

	// convert to geocentric rectangular coordinates
	double xg = x - m_xEarth;
	double yg = y - m_yEarth;
	double zg = z;

	// rotate around x axis from ecliptic to equatorial coords

	double ecl = 23.429292 * rads;  //value for J2000.0 frame
	double xeq = xg;
	double yeq = (yg * cos(ecl)) - (zg * sin(ecl));
	double zeq = (yg * sin(ecl)) + (zg * cos(ecl));

	// find the RA and DEC from the rectangular equatorial coords
	ra = AlgBase::fnatn2(yeq, xeq);
	dec = atan(zeq / sqrt((xeq * xeq) + (yeq * yeq)));
	dist = sqrt((xeq * xeq) + (yeq * yeq) + (zeq * zeq));

	// Return in degrees
	ra = ra * degs / 15;
	dec = dec * degs;
}

static void showPositions(const PlanetDescriptor& planet, const double& ra, const double& dec, const double& dist)
{
	char raStr[100];
	char dclStr[100];
	printDegrees(raStr, ra);
	printDegrees(dclStr, dec);

	std::cout << "Equatorial coordinates of planet " << planet.planetName << std::endl;
	std::cout << "   RA = " << raStr << std::endl;
	std::cout << "  DEC = " << dclStr << std::endl;
	std::cout << " Dist = " << dist << std::endl;

}

void APlanets::computeAPlanet(const PlanetDescriptor& planet, const ALocation& location, const double j2000, const double md)
{
	double ra;
	double dec;
	double rvec;

	computePlanetPos(planet, j2000, ra, dec, rvec);

	showPositions(planet, ra, dec, rvec);

	// Crude method to see if it is above the horizon
	double alt = AlgBase::localAltitude(location, md, ra, dec);
	std::cout << "  Alt = " << alt << std::endl;
}

void APlanets::printAll(const ALocation& location, const ADateTime& dateTime)
{
	std::cout << "....................................." << std::endl;
	auto itr = planetDescrip.begin();
	for (; itr != planetDescrip.end(); itr++)
	{
		if (itr->planetIndex != PlanetType::Earth)
		{
			computeAPlanet(*itr, location, dateTime.j2000Day(), dateTime.modifiedJuiianDate(true));

			std::cout << "....................................." << std::endl;
		}
	}
}

void APlanets::computePlanets(const ALocation& location, const ADateTime& procTime, int type)
{
	ADateTime dateTime(procTime);

	// Get Earth info
	double d = dateTime.j2000Day();
	// Use with example (see QBasicCode.txt)
	// 0h 21 June 1997
	// double d = -924.50;

	if (m_verboseLevel & DebugJulianDate)
	{
		dateTime.printASCII();
		std::cout << "J2000 date: " << d << std::endl;
	}

	double md = dateTime.modifiedJuiianDate(true);

	std::cout << "....................................." << std::endl;

	// Earth's position needs to be computed first to figure out the vectors
	findPosition(planetDescrip[PlanetType::Earth], d, m_xEarth, m_yEarth, m_zEarth);

	// Compute all the planets except Earth
	std::vector<int>list{0,1,3,4,5,6,7,8};

	// After search, if not found, print all
	if ((type < 0) || (type == PlanetType::Earth))
	{
		printAll(location, dateTime);
	}
	else
	{
		// Find specific Planet
		bool found = false;
		auto itr = planetDescrip.begin();
		for (; itr != planetDescrip.end(); itr++)
		{
			if (itr->planetIndex == type)
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			std::cout << "....................................." << std::endl;
			computeAPlanet(*itr, location, d, md);
			std::cout << "....................................." << std::endl;
			return;
		}
		else
		{
			printAll(location, dateTime);
		}
	}
}

void APlanets::computePlanets(const ALocation& location, const ADateTime& procTime)
{
	computePlanets(location, procTime, m_planetType);
}