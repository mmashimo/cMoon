/// @file
///
/// @brief Settings class implementation.
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

#include <iostream>

#include <cstring>
#include <exception>

#include <sys/stat.h>

#include "settings.hpp"

#include "ADateTime.h"
#include "AMoon.h"
#include "ASun.h"
#include "APlanets.h"

const std::string s_LocationKey{"Location."};
const std::string s_TimeKey{"Time."};
const std::string s_DebugKey{"Debug."};


std::string Settings::s_iniFile = "../cMoon.ini";


Settings::Settings(const bool forceApply, const std::string& iniFile)
	: m_iniFile(iniFile)
	, m_fileWasRead(false)
	, m_changed(false)
{
	openIniHelper(iniFile, forceApply);
}

Settings::~Settings()
{
	if (m_changed)
	{
		// Write INI file
		write_ini(m_iniFile, m_pt);
	}
}

bool Settings::hasChanged() const
{
	return m_changed;
}

const ALocation& Settings::getLocation() const
{
	return m_location;
}

bool Settings::setDefaultIniFile(const std::string& defaultFileName, const bool forceSave)
{
	bool success = true;
	struct stat status;

	if (stat(defaultFileName.c_str(), &status) == 0)
	{
		s_iniFile = defaultFileName;
		std::cout << "Using '" << defaultFileName << "' for configuration." << std::endl;
	}
	else
	{
		std::cout << "Cannot INI file '" << defaultFileName << "' -- not found" << std::endl;
		success = false;
	}

	return success;
}


void Settings::openIniHelper(const std::string& iniFile, const bool forceApply)
{
	struct stat status;

	if(stat(iniFile.c_str(), &status) == 0)
	{
		try
		{
			boost::property_tree::ini_parser::read_ini(iniFile, m_pt);
			m_fileWasRead = true;
			if (forceApply)
			{
				// Get all the configuration in INI file
				applyIni();
			}
		}
		catch(std::exception& e)
		{
			std::cout << e.what() << std::endl;
			std::cout << "Using file: '" << iniFile << "' -- exception caught at contructor" << std::endl;
		}
	}
	else
	{
		std::cout << "INI file '" << iniFile << "' - not found. Writing default." << std::endl;
		write_default_settings();
	}
}

void Settings::applyIni()
{
	get_location_info();
	get_date_time_info();
	get_debug_info();
}

int Settings::getInt(const std::string& key_name, const int defValue)
{
	int value = 0;
	try
	{
		value = m_pt.get<int>(key_name);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		value = defValue;
	}

	return value;
}

double Settings::getFloat(const std::string& key_name, const double defValue)
{
	double value = 0.;
	try
	{
		value = m_pt.get<double>(key_name);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		value = defValue;
	}

	return value;
}


std::string Settings::getString(const std::string& key_name, const char* defValue)
{
	std::string value = defValue;

	try
	{
		value = m_pt.get<std::string>(key_name);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return value;
}

void Settings::get_location_info()
{
	double lat = getFloat("Location.LAT");
	double lng = getFloat("Location.LONG");
	double elev = getFloat("Location.Elevation");

	if (m_changed)
	{
		std::cout << "Settings: Overwriting Location: LAT=" << m_location.latitude() << " to " << lat <<
			" LONG=" << m_location.longitude() << " to " << lng <<
			" Elev=" << m_location.elevation() << " to " << elev << "(ft.)" << std::endl;
	}

	m_location.setLatitude( lat );
	m_location.setLongitude( lng );
	m_location.setElevation( elev );

}

void Settings::get_date_time_info()
{
	int tz = getInt("Time.Zone");
	int dst = getInt("Time.DST") != 0;

	if (m_changed)
	{
		std::cout << "Overwriting Date-Time Zone (from UTC): " << ADateTime::m_defaultTimeZone << " to " << tz <<
			" DST-ON: " << ADateTime::m_useDST << " to " << dst << std::endl;
	}

	ADateTime::m_defaultTimeZone = tz;
	ADateTime::m_useDST = dst != 0;
}

void Settings::get_debug_info()
{
	std::string verbose = getString("Debug.Verbose");
	ADateTime::m_verboseLevel = getInt("Debug.DateTime");
	AMoon::m_verboseLevel = getInt("Debug.Moon");
	ASun::m_verboseLevel = getInt("Debug.Sun");
}

void Settings::save_location_info(const ALocation& location)
{
	m_pt.put("Location.LAT", location.latitude());
	m_pt.put("Location.LONG", location.longitude());
	m_pt.put("Location.Elevation", location.elevation());
	m_pt.put("Location.ElevUnit", "f");

	m_changed = true;
	m_location = location;

	std::cout << "Saving Location: LAT=" << location.latitude()
		<< " LONG=" << location.longitude()
		<< " Elev=" << location.elevation() << "ft." << std::endl;
}

void Settings::save_date_time_info()
{
	m_pt.put("Time.Zone", ADateTime::m_defaultTimeZone);
	m_pt.put("Time.DST", 1);
	m_changed = true;
	std::cout << "Saving Date-Time Zone (from UTC): " << ADateTime::m_defaultTimeZone
		<< " DST-ON" << std::endl;
}

void Settings::save_debug_info()
{
	m_pt.put("Debug.Verbose", "vA");
	m_pt.put("Debug.DateTime", ADateTime::m_verboseLevel);
	m_pt.put("Debug.Moon", AMoon::m_verboseLevel);
	m_pt.put("Debug.Sun", ASun::m_verboseLevel);
	m_changed = true;
}

void Settings::write_default_settings()
{
	std::cout << "Writing to file: " << m_iniFile << std::endl;

	save_location_info(m_location);
	save_date_time_info();
	save_debug_info();

	m_changed = true;
}

