/// @file
///
/// @brief AMoon class definitions.
///
/// AMoon has computation methods for moon positions.
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

#include <string>
#include <cmath>

#include "AlgBase.h"

#include "ADateTime.h"

using DateString = std::string;

using PhaseInfo = struct _moonPhase
{
	double julian;
	int    daysSince;
	double newMoons;
	double daysFromNew;
	double phasePercent;
};


class AMoon : public AlgBase
{
public:
	AMoon();

	AMoon(const AMoon& ref);

	virtual ~AMoon();

	AMoon& operator=(const AMoon& ref);

	/// @brief Compute the current Moon Phase.
	/// @param[in] dateTime
	/// @return "Next Phase" value
	int moonPhase(const ADateTime& dateTime);

	void moonRise(const ADateTime& procTime);

	int nextMoonPhase(const ADateTime& dateTime, int phase);

    /// @brief Sets print statement verbose mode
    /// @param[in] level - 0=quiet(results only) non-zero(prints debug info)
    void setVerboseMode(const int level);

	static int m_verboseLevel;

private:
	PhaseInfo m_phase;

	void copyHelper(const AMoon& ref);

	int processPhase(const ADateTime& jd, PhaseInfo& phase);

	int printPhase(const PhaseInfo& phase) const;

	void computeNextPhase(int phase, ADateTime& dateTime);

};
