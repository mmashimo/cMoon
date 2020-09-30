/// @file
///
/// @brief AObject class implementation.
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


#include <iostream>

#include "AObject.h"

#include "AlgBase.h"
#include "ADateTime.h"

static double sinho[6];


AObject::AObject(const char* name, const AltitudeType altType, bool brightObject)
	: m_Name(name)
{
	initialize(brightObject);
	setAltitudeType(altType);
	resetSettings();
}

AObject::~AObject()
{

}

void AObject::initialize(const bool brightObject)
{
	if (brightObject)
	{
		m_strAbove = "bright";
		m_strBelow = "dark";
	}
	else
	{
		m_strAbove = "above horizon";
		m_strBelow = "below horizon";
	}
}

void AObject::resetSettings()
{
	m_utRise = 0.;
	m_utSet  = 0.;
	m_rise   = false;
	m_sett   = false;
	m_above  = false;

}


void AObject::setAltitudeType(const AltitudeType type)
{
	// Set the object refraction deviations
	static bool bFirst = true;
	if (bFirst)
	{
		sinho[0] = 0;
		sinho[1] = AlgBase::sinDegrees(8. / 60.);   // moonrise - average diameter used
		sinho[2] = AlgBase::sinDegrees(-50. / 60.); // sunrise - classic value for refraction
		sinho[3] = AlgBase::sinDegrees(-6.);        // snrise/set - civil (6degrees) twilight
		sinho[4] = AlgBase::sinDegrees(-12.);       // nautical twilight
		sinho[5] = AlgBase::sinDegrees(-18.);       // astronomical twilight
		bFirst = false;
	}
	m_sinHorizontal = sinho[static_cast<int>(type)];
}

// returns number containing the time written in hours and minutes
// rounded to the nearest minute
// NOTE: 'str' size must be at least 10
static int hm(double jdHour, char* str)
{
	// !'round ut to nearest minute
#if 0
	int h = static_cast<int>(floor(ut));
	int m = static_cast<int>(floor(60 * (ut - h)));
#endif
	double jd = jdHour / 24.;
	int h, m, s;
	ADateTime::getJulianTime(jd, h, m, s);
	sprintf(str, "%02d:%02d:%02d", h, m, s);

	return (100 * h) + m;
}



void AObject::printRiseSetTimes()
{
	char strRiseTime[12];
	char strSetTime[12];

	int utRiseTime = hm(m_utRise, strRiseTime);
	int utSetTime = hm(m_utSet, strSetTime);

	const char *strObj = m_Name;

	// ' logic to sort the various rise and set states
	if (m_rise || m_sett)    // 'current object rises and sets today
	{
		std::cout << strObj;
		if (m_rise)
		{
			std::cout << " UTC-rise (" << m_utRise << "): " << strRiseTime << std::endl; // PRINT USING p$; utrise
		}
		else
		{
			std::cout << "    ----" << std::endl;
		}

		std::cout << strObj;
		if (m_sett )
		{
			std::cout << " UTC-set (" << m_utSet << "): " << strSetTime << std::endl; // PRINT USING p$; utset
		}
		else
		{
			std::cout << "    ----" << std::endl;
		}
	}
	else              // 'current object not so simple
	{
		if (m_above)
		{
			std::cout << "    always " << m_strAbove << std::endl;
		}
		else
		{
			std::cout << "    always " << m_strBelow << std::endl;
		}
	}
}


void AObject::adjustForNext(double yPrior, double yCurr, double yNext, double hour)
{
	double z1, z2;
	double xe, ye;

	int nz = AlgBase::quad(yPrior, yCurr, yNext, xe, ye, z1, z2);

	switch (nz)
	{
	case 0: // 'nothing  - go to next time slot
		break;

	case 1:                      // ' simple rise / set event
		if (yPrior < 0)       // ' must be a rising event
		{
			m_utRise = hour + z1;
			m_rise = true;
		}
		else
		{
			// ' must be setting
			m_utSet = hour + z1;
			m_sett = true;
		}
		break;

	case 2:                      // ' rises and sets within interval
		if (ye < 0)       // ' minimum - so set then rise
		{
			m_utRise = hour + z2;
			m_utSet = hour + z1;
		}
		else
		{
			// ' maximum - so rise then set
			m_utRise = hour + z1;
			m_utSet = hour + z2;
		}

		m_rise = true;
		m_sett = true;
		break;
	}
}


