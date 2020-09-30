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
/// Foobar is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with Foobar.  If not, see <https://www.gnu.org/licenses/>.

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

double ALocation::m_latitude = LAT;
double ALocation::m_longitude = LONG;
double ALocation::m_elevation = ELEV;


ALocation::ALocation()
{

}

ALocation::~ALocation()
{

}

