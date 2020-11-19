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

#ifdef USE_TYPEDEF

typedef struct _moonPhase
{
	double julian;
	int    daysSince;
	double newMoons;
	double daysFromNew;
	double phasePercent;
} PhaseInfo;

#else

using PhaseInfo = struct _moonPhase
{
	double julian;
	int    daysSince;
	double newMoons;
	double daysFromNew;
	double phasePercent;
};

#endif

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

	/// @brief Computes Moon rise/set times
	/// @param[in] location
	/// @param[in] procTime - of the day
	void moonRise(const ALocation& location, const ADateTime& procTime);

	/// @brief Computes Next Moon Phase from give dateTime.
	/// @param[in] dateTime - Set date and time
	/// @param[in] startPhase - next phase from dateTime
	/// @param[in] lockPhase - Show only the phase in startPhase
	int nextMoonPhase(const ADateTime& dateTime, const int startPhase, const bool lockPhase, const int numOfPhases = 4, const int numOfCycles = 1);

    /// @brief Sets print statement verbose mode
    /// @param[in] level - 0=quiet(results only) non-zero(prints debug info)
    void setVerboseMode(const int level);

	static int m_verboseLevel;

private:

	void copyHelper(const AMoon& ref);

	int processPhase(const ADateTime& jd, PhaseInfo& phase);

	int printPhase(const PhaseInfo& phase) const;

	/// @brief Computes Moon Cycle for the phase - given year and day-of-year (from dateTime).
	/// @param[in] phase - [0=new, 1=waxing quarter, 2=full, 3=waning quarter]
	/// @param[in] dateTime - date and time with JDE already computed
	/// @param[out] Y - year of conversion
	/// @param[out] K - number of moon of phases since J2000
	/// @return Moon Cycle since J2000 (K)
	double computeKForNextPhase(const int phase, const ADateTime& dateTime);

	/// @brief Computes offset of the given phase for the cycle and the 100-year epoch.
	/// @param[in] phase - (0= new)
	/// @param[in] K - number of Moons plus shifted phase since J2000
	/// @param[in] T - Tau - the percent of cycle within 100 year epoch
	/// @param[in] year - current year - used to compute seconds offset per each year since 0AD
	/// @return adjusted (fine-tuned) JDE down to the second
	double fineTuneJdeForCycle(const int phase, const double& K, const int year);

	/// @brief Shows precise Moon phase date/time for a given since J2000 (K) - adjusts DateTime by Julian date.
	/// @param[in] K - Moon cycles since J2000
	/// @param[in,out] dateTime - DateTime to modify
	void displayMoonPhaseForK(const int phase, const double K, ADateTime& dateTime);

	PhaseInfo m_phase;
};
