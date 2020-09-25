/// @file

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
