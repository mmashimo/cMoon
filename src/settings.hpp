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

#include "ALocation.h"

// namespace ...

class Settings
{
public:
	/// @brief Constructor
	/// @param[in] forceApply - if true, gets setting information when created
	Settings(const bool forceApply = false, const std::string& iniFile = s_iniFile);

	/// @brief Destructor
	~Settings();

	/// @brief Returns if any of configuration was changed
	/// @return true if 'save' was applied
	bool hasChanged() const;

	/// @brief Retrieve Location information for this setting.
	/// @return ALocation object retrieved from INI files
	const ALocation& getLocation() const;

	/// @brief Saves Location information
	/// @param[in] location - saves items in location object
	void save_location_info(const ALocation& location);

	/// @brief Saves Date-Time info
	void save_date_time_info();

	/// @brief Saves Debug Level info
	void save_debug_info();

	/// @brief Create a list of all the settings that should be in the INI file
	void write_default_settings();

	/// @brief Sets the Default INI file - name
	/// @param[in] defaultFilename
	/// @param[in] forceSave - forces current settings even if file not created
	/// @return true if File exists, otherwise, it will show error
	bool setDefaultIniFile(const std::string& defaultFileName, const bool forceSave = false);

private:

	/// @brief Initializes settings by reading existing or making new 'iniFile'
	void openIniHelper(const std::string& iniFile, const bool forceApply);

	/// @brief Applies configureation read in when opened
	void applyIni();

	/// @brief retrieves integer ini setting
	int getInt(const std::string& key_name, const int defValue = 0);

	/// @brief retrieves float ini setting
	double getFloat(const std::string& key_name, const double defValue = 0.);

	/// @brief retrieves string ini setting
	std::string getString(const std::string& key_name, const char* defValue = "");


	/// @brief Updates local Location information from INI
	void get_location_info();

	/// @brief Get Date-Time information
	void get_date_time_info();

	/// @brief Get Debug Level info
	void get_debug_info();



	/// @brief INI file (source)
	std::string                 m_iniFile;

	/// @brief INI was READ - this is false if the INI file is closed for some reason
	bool                        m_fileWasRead;

	/// @brief INI file modified
	bool                        m_changed;

	/// @brief Property Tree
	boost::property_tree::ptree m_pt;

	/// @brief Local copy of location
	ALocation m_location;

	/// @brief Default File name used to change
	static std::string s_iniFile;

};