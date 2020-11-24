/// @file
///
/// @brief ALocation class definitions.
///
/// ALocation has computation methods for geo-positional algorithms.
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


class ALocation
{
public:
	/// Default constructor.
	ALocation();

	/// Destructor
	~ALocation();

	/// @brief Copy contructor
	/// @param[in] ref - object to copy
	ALocation(const ALocation& ref);

	/// @brief Assignment operator
	/// @param[in] ref - this object is assigned to ref
	ALocation& operator =(const ALocation& ref);

	/// @brief Get Coordinates (LAT/LONG) both or individually
	/// @param[out] latitude
	/// @param[out] longitude
	void getCoordinates(double& latitude, double& longitude) const;

	/// @brief Displays LAT/LONG in display format
	/// @param[in] format
	void displayCoordinates(const int format = 0) const;

	// In-line accessors
	double latitude() const  { return m_latitude; }
	double longitude() const { return m_longitude; }
	double elevation() const { return m_elevation; }

	void setLatitude(const double latitude) { m_latitude = latitude; }
	void setLongitude(const double longitude) { m_longitude = longitude; }
	void setElevation(const double elevation) { m_elevation = elevation; }

private:
	/// @brief Copies content for copy and assignmen
	/// @param[in] ref - copies object to this
	void copyHelper(const ALocation& ref);

	// Public access (as structs)
	double m_latitude;
	double m_longitude;
	double m_elevation;

};
