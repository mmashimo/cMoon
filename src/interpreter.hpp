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

#pragma once

#include <string>
#include <vector>
#include <map>

#include "ADateTime.h"
#include "ALocation.h"
#include "AMoon.h"
#include "ASun.h"
#include "APlanets.h"

using ArgumentList = std::vector<std::string>;

using ExecutionList = std::map<int, ArgumentList>;

class Interpreter
{
public:
	/// @brief Constructor
	/// @param[in] dateObj
	/// @param[in] location
	/// @param[in] moonObj
	/// @param[in] sunObj
	/// @param[in] planets
	Interpreter(const ADateTime& dateObj,
				const ALocation& location,
				const AMoon& moonObj,
				const ASun& sunObj,
				const APlanets& planets);

	/// @brief Destructor
	virtual ~Interpreter();

	/// @brief Gets a single line of command, checks for special characters
	void getOneLine(std::string& buffer);

	/// @brief Parse Command line
	int parse();

	/// @brief Exit character if the first key (even if backspaced to being first)
	static char s_exitChar;

	/// @brief Command completion character
	static char s_executeChar;

	/// @brief Escape key
	static char s_escapeChar;

private:

	/// @brief parse one line
	bool parseOneLine(const std::string& buffer);

	/// @brief Date Time object
	ADateTime m_dateTime;
	ALocation m_location;
	AMoon     m_moon;
	ASun      m_sun;
	APlanets  m_planets;
};

