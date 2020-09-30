/// @file
///
/// @brief Settings class definitions.
///
/// Settings gets and sets configuration for INI file.
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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <cstring>
#include <cmath>

#include "ADateTime.h"
#include "ALocation.h"

// namespace ...

class Settings
{
public:
	/// @brief Constructor
	Settings(const std::string& iniFile);

	/// @brief Destructor
	~Settings();

#if 0
	/// @brief retrieve string ini setting
	int getString(std::string& key_name);
#endif

	/// @brief Get Location info
	void get_location_info();

	/// @brief Get Date-Time information
	void get_date_time_info();

	/// @brief Saves Location information
	void save_location_info();

	/// @brief Saves Date-Time info
	void save_date_time_info();

	/// @brief Create a list of all the settings that should be in the INI file
	void write_default_settings();

private:

	/// @brief Initializes settings by reading existing or making new 'iniFile'
	void openIniHelper(const std::string& iniFile);

	/// @brief retrieves integer ini setting
	int getInt(const std::string& key_name);

	/// @brief retrieves float ini setting
	double getFloat(const std::string& key_name);

	/// @brief INI file (source)
	std::string                 m_iniFile;

	/// @brief INI file modified
	bool                        m_changed;

	/// @brief Property Tree
	boost::property_tree::ptree m_pt;

};