/// @file
///
/// @brief ASun class computes sunrise and sunset.
///
/// ASun has computation methods for Sun-location in the Earth sky.
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

/// NOTE: Need location information in decimal degrees:
/// lat, long
#pragma once

#include "pch.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>

#include "ADateTime.h"

#include "AlgBase.h"

class ASun : public AlgBase
{
public:
	ASun();

    /// @brief Sets print statement verbose mode
    /// @param[in] level - 0=quiet(results only) non-zero(prints debug info)
    void setVerboseMode(const int level);

	void showSun(const ADateTime& procTime);

	static int m_verboseLevel;

private:

};
