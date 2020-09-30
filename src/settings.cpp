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
/// Foobar is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
///

#include <iostream>
#include <cstring>
#include <exception>

#include <sys/stat.h>

#include "settings.hpp"


const std::string s_LocationKey{"Location."};
const std::string s_TimeKey{"Time."};
const std::string s_DebugKey{"Debug."};


Settings::Settings(const std::string& iniFile)
	: m_iniFile(iniFile)
	, m_changed(false)
{
	openIniHelper(iniFile);
}

Settings::~Settings()
{
	if (m_changed)
	{
		// Write INI file
		write_ini(m_iniFile, m_pt);
	}
}

void Settings::openIniHelper(const std::string& iniFile)
{
	struct stat status;

	if(stat(iniFile.c_str(), &status) == 0)
	{
		try
		{
			boost::property_tree::ini_parser::read_ini(iniFile, m_pt);

			get_location_info();
			get_date_time_info();
		}
		catch(std::exception& e)
		{
			std::cout << e.what() << std::endl;
			std::cout << "Using file: " << iniFile << std::endl;
		}
	}
	else
	{
		std::cout << "INI file '" << iniFile << "' - not found. Writing default." << std::endl;
		write_default_settings();
	}
}

int Settings::getInt(const std::string& key_name)
{
	int value = 0;
	try
	{
		value = m_pt.get<int>(key_name);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return value;
}

double Settings::getFloat(const std::string& key_name)
{
	double value = 0.;
	try
	{
		value = m_pt.get<double>(key_name);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return value;

}

void Settings::get_location_info()
{
	ALocation::m_latitude = getFloat("Location.LAT");
	ALocation::m_longitude = getFloat("Location.LONG");
	ALocation::m_elevation = getFloat("Location.Elevation");
	std::cout << "Using Location: LAT=" << ALocation::m_latitude
		<< " LONG=" << ALocation::m_longitude
		<< " Elev=" << ALocation::m_elevation << "ft." << std::endl;
}

void Settings::get_date_time_info()
{
	ADateTime::m_defaultTimeZone = getInt("Time.Zone");
	// ADateTime::m_useDST = getInt("Time.DST");
	std::cout << "Using Date-Time Zone (from UTC): " << ADateTime::m_defaultTimeZone
		<< " DST-ON" << std::endl;
}

void Settings::save_location_info()
{
	m_pt.put("Location.LAT", ALocation::m_latitude);
	m_pt.put("Location.LONG", ALocation::m_longitude);
	m_pt.put("Location.Elevation", ALocation::m_elevation);
	m_pt.put("Location.ElevUnit", "f");
	m_changed = true;
	std::cout << "Saving Location: LAT=" << ALocation::m_latitude
		<< " LONG=" << ALocation::m_longitude
		<< " Elev=" << ALocation::m_elevation << "ft." << std::endl;
}

void Settings::save_date_time_info()
{
	m_pt.put("Time.Zone", ADateTime::m_defaultTimeZone);
	m_pt.put("Time.DST", 1);
	m_changed = true;
	std::cout << "Saving Date-Time Zone (from UTC): " << ADateTime::m_defaultTimeZone
		<< " DST-ON" << std::endl;
}

void Settings::write_default_settings()
{
	std::cout << "Writing to file: " << m_iniFile << std::endl;

	save_location_info();
	save_date_time_info();

	m_pt.put("Debug.Verbose", 0);
	m_pt.put("Debug.DateTime", ADateTime::m_verboseLevel);

	m_changed = true;
}

