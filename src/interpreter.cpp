
/// @file
///
/// @brief Interpreter interactive mode base class definitions.
///
/// This is Base class for parsing interactive commands
///
/// @copyright 2017-2020 M.Mashimo and licensors. All Right Reserved.
///
/// NOTE: This module is used for other projects - should be made into a library.
/// For cMoon, this interactive command processor is used to display and compute
/// cMoon objects one command at a time.
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

#include <iostream>

#include "interpreter.hpp"


char Interpreter::s_exitChar = 'q';

char Interpreter::s_executeChar = '\r';

char Interpreter::s_escapeChar = '\x1b';

Interpreter::Interpreter(const ADateTime& dateObj,
						const ALocation& location,
						const AMoon& moonObj,
						const ASun& sunObj,
						const APlanets& planets)
	: m_dateTime(dateObj)
	, m_location(location)
	, m_moon(moonObj)
	, m_sun(sunObj)
	, m_planets(planets)
{
	// Intentionally left blank
}

Interpreter::~Interpreter()
{
	// Intentionally left blank
}

void Interpreter::getOneLine(std::string& cmd)
{
	std::getline(std::cin, cmd);
}

int Interpreter::parse()
{
	bool bDone = false;
	std::string buffer;

	// Show current location and time
	m_location.displayCoordinates();
	m_dateTime.showDateTime();

	std::cout << std::endl;

	while (!bDone)
	{
		std::cout << "Prompt> ";

		// This should block:
		getOneLine(buffer);

		// Parse one line of commands
		bDone = parseOneLine(buffer);

	}

	return 0;
}


bool Interpreter::parseOneLine(const std::string& buffer)
{
	bool bDone = false;
	const char* options;

	if (isdigit(buffer[0]))
	{
		m_dateTime.parseDateTime(buffer.c_str());
		m_dateTime.showDateTime();
	}
	else
	{
		switch(buffer[0])
		{
			case 'd':
			case 'D':
				m_dateTime.showDateTime();
				break;
			case '?':
			case 'h':
				std::cout << "Help!" << std::endl;
				break;
			case 'l':	// Display location information
			case 'L':
				m_location.displayCoordinates();
				break;
			case 'm':  // Moon phase
			case 'M':
				m_moon.moonPhase(m_dateTime);
				break;
			case 'n':
			case 'N': // Next moon phase
				if (buffer[1] != '\0')
				{
					options = buffer.c_str() + 1;
					m_moon.parseNextPhase(options);
				}
				m_moon.nextMoonPhase(m_dateTime);
				break;
			case 'p':	// Planets
			case 'P':
				if (buffer[1] != '\0')
				{
					options = buffer.c_str() + 1;
					m_planets.parseArgs(options);
				}
				m_planets.computePlanets(m_location, m_dateTime);
				break;

			case 'q':	// Quit/end
			case 'Q':
				bDone = true;
				break;

			case 'r':	// Moonrise
			case 'R':
				m_moon.moonRise(m_location, m_dateTime);
				break;

			case 's':
			case 'S':
				m_sun.showSun(m_location, m_dateTime);
				break;

			case '\0':
				break;

			default:
				std::cout << "Command: '" << buffer << "' -- do not understand" << std::endl;
				break;
		}
	}

	return bDone;
}

