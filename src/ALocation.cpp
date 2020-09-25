/// @file

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

