/// @file
///
/// @brief AObject class definitions.
///
/// AObject has computation methods for Altitude algorithms.
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
#include <array>


/// @brief Use to compute sunrise/set (mostly), determines altitude at which sun position time is used.
/// NOTE: NumberOfAltitudeTypes must match the number of altitude settings (below) since it is also used as an index
enum class AltitudeType : int
{
	AtElevation = 0,
	MoonObject,
	ActualSun,
	CivilSun,
	NauticalSun,
	AstronomicalSun
};


/// @brief Number of different types of Altitude computation for sunrise/set (Must match number of AltitudeTypes above).
constexpr int NumberOfAltTypes{6};


// Base object for all objects
class AObject
{
public:

	/// @brief Default constructor
	AObject(const char* name, const AltitudeType altType, bool brightObject);

	/// @brief Destructor
	~AObject();

	/// @brief Resets internal variables
	void resetSettings();

	/// @brief Prints Rise/Set times for objects
	void printRiseSetTimes();

	/// @brief Adjusts time hourly to check ends and resolved current
	void adjustForNext(double yPrior, double yCurr, double yNext, double hour);

	const char* m_Name;
	const char* m_strAbove;
	const char* m_strBelow;
	double      m_utRise;
	double      m_utSet;
	bool        m_rise;
	bool        m_sett;
	bool        m_above;

	double      m_sinHorizontal;

private:

	void initialize(const bool brightObject);

	void setAltitudeType(const AltitudeType type);

	// NOTE: Use of non-const since it may need to be re-computed.
	static std::array<double, NumberOfAltTypes> m_sinho;
};



