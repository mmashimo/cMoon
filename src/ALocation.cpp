/// @file
///
/// @brief ALocation class implementation.
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

#include "ALocation.h"

// Needs: LAT, LONG in decimal degrees
constexpr double LAT = 42.81751;  // Home
constexpr double LONG = -71.2781;
// constexpr double ELEV = 547.;     // feet
constexpr double ELEV = 400.;     // feet

// double LAT = 42.35;   // Boston
// double LONG = -71.05;
// double ELEV = 0.;     // feet

ALocation::ALocation()
	: m_latitude{LAT}
	, m_longitude{LONG}
	, m_elevation{ELEV}
{
	// Set default from constexpr
}

ALocation::ALocation(const ALocation& ref)
{
	copyHelper(ref);
}

ALocation::~ALocation()
{
	// Nothing to do
}

ALocation& ALocation::operator =(const ALocation& ref)
{
	copyHelper(ref);
	return *this;
}

void ALocation::copyHelper(const ALocation& ref)
{
	m_latitude  = ref.m_latitude;
	m_longitude = ref.m_longitude;
	m_elevation = ref.m_elevation;
}

void ALocation::getCoordinates(double& latitude, double& longitude) const
{
	latitude = m_latitude;
	longitude = m_longitude;
}

void ALocation::displayCoordinates(const int format) const
{
	if (format == 0)
	{
		std::cout << "LAT:    " << m_latitude << std::endl;
		std::cout << "LONG:   " << m_longitude << std::endl;
		std::cout << "ELEV:   " << m_elevation << "ft" << std::endl;
	}
	else
	{
		std::cout << "Latitude: " << m_latitude << " Longitude: " << m_longitude << " Elevation: " << m_elevation << "ft" << std::endl;
	}
	
}
