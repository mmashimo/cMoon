/// @file
///
/// @brief AMoon class implementation.
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

#include "pch.h"

#include <iostream>
#include <cstdio>
#include <iomanip>
#include <stdlib.h>
#include <array>

#include "AMoon.h"
#include "ALocation.h"

#include "AObject.h"


constexpr double MoonDays{ 29.53058770576 };
constexpr double FullDays{ 14.765 };
constexpr double DiffMoonPercent{ 3.386 };
constexpr double MilleniumJD{ 2451544.5 };

// Example of Julian Days is: 2017-3-1 should be 2457813.5
// https://www.subsystems.us/uploads/9/8/9/4/98948044/moonphase.pdf

// Conversion of sunrise/sunset from:
// https://en.wikipedia.org/wiki/Sunrise_equation

int AMoon::m_verboseLevel = 1;

AMoon::AMoon()
{
}


AMoon::AMoon(const AMoon& ref)
{
	copyHelper(ref);
}

AMoon::~AMoon()
{

}

AMoon& AMoon::operator=(const AMoon& ref)
{
	copyHelper(ref);
	return *this;
}

void AMoon::copyHelper(const AMoon& ref)
{
	m_phase = ref.m_phase;
}

void AMoon::setVerboseMode(const int level)
{
    m_verboseLevel = level;
}


int AMoon::moonPhase(const ADateTime& dateTime)
{
	ADateTime newTime = dateTime;
	PhaseInfo phase;

	processPhase(dateTime, phase);

	return printPhase(phase);
}

int AMoon::processPhase(const ADateTime& dateTime, PhaseInfo& phase)
{
	int nextPhase = 0;	// start with "new"
	// Example of Julian Days is: 2017-3-1 should be 2457813.5
	// https://www.subsystems.us/uploads/9/8/9/4/98948044/moonphase.pdf
	double jd = dateTime.m_julian;
	phase.julian = jd;
	phase.daysSince = static_cast<int>(jd - 2451549.5);
	phase.newMoons = phase.daysSince / MoonDays;

	if (jd > 2451549.5)
	{
		phase.daysFromNew = (phase.newMoons - int(phase.newMoons)) * MoonDays;
	}
	else
	{
		phase.daysFromNew = MoonDays + ((phase.newMoons - int(phase.newMoons)) * MoonDays);
	}

	phase.phasePercent = (phase.daysFromNew / MoonDays) * 100.;

	if (phase.phasePercent < 25.)
	{
		nextPhase = 1;
	}
	else if (phase.phasePercent < 50.)
	{
		nextPhase = 2;
	}
	else if (phase.phasePercent < 75.)
	{
		nextPhase = 3;
	}
	// Otherwise, new Moon

	m_phase = phase;

	return nextPhase;
}

int AMoon::printPhase(const PhaseInfo& phase) const
{
	int nextPhase = 0;

	double julian;
	int    daysSince;
	double newMoons;
	double daysFromNew;
	double phasePercent;

	if (m_verboseLevel & DebugJulianDate)
	{
		std::cout << "Julian day = " << phase.julian << " Days (of new) since 2000-01-06 = " 
			<< phase.daysSince << " New moons = " << phase.newMoons << std::endl;
	}

	std::cout << std::endl;

	if (m_verboseLevel & DebugFullDateTime)
	{
		std::cout << "Phase: " << phase.daysFromNew << " days of New Moon" << std::endl;
	}
	else
	{
		std::cout << "Phase: " << static_cast<int>(phase.daysFromNew) << " days of New Moon" << std::endl;
	}
	

	if ((phase.daysFromNew < 1.0) || (phase.daysFromNew > 28.53))
	{
		if (m_verboseLevel & DebugFullDateTime)
		{
			std::cout << "New Moon " << phase.phasePercent << "%" << std::endl;
		}
		else
		{
			std::cout << "New Moon " << static_cast<int>(phase.phasePercent) << "%" << std::endl;
		}
		nextPhase = 1;	
	}
	else if ((phase.daysFromNew > (FullDays - 0.5)) && (phase.daysFromNew < (FullDays + 0.5)))
	{
		if (phase.phasePercent < 50.)
		{
			if (m_verboseLevel & DebugFullDateTime)
			{
				std::cout << "Full Moon " << (phase.phasePercent * 2.) << "%" << std::endl;
			}
			else
			{
				std::cout << "Full Moon " << static_cast<int>(phase.phasePercent * 2.) << "%" << std::endl;
			}
		}
		else
		{
			if (m_verboseLevel & DebugFullDateTime)
			{
				std::cout << "Full Moon " << (100. - ((phase.phasePercent - 50.) * 2.)) << "%" << std::endl;
			}
			else
			{
				std::cout << "Full Moon " << static_cast<int>(100. - ((phase.phasePercent - 50.) * 2.)) << "%" << std::endl;
			}
		}
		nextPhase = 3;
	}
	else if (phase.phasePercent < 50.)
	{
		if (m_verboseLevel & DebugFullDateTime)
		{
			std::cout << "Waxing " << (phase.phasePercent * 2.) << "% - "
				<< (FullDays - phase.daysFromNew) << " days to Full" << std::endl;
		}
		else
		{
			std::cout << "Waxing " << static_cast<int>(phase.phasePercent * 2.) << "% - "
				<< static_cast<int>(FullDays - phase.daysFromNew) << " days to Full" << std::endl;
		}
		nextPhase = 2;
	}
	else
	{
		if (m_verboseLevel & DebugFullDateTime)
		{
			std::cout << "Waning " << (100. - ((phase.phasePercent - 50.) * 2.)) << "% - "
				<< (MoonDays - phase.daysFromNew) << " days to New" << std::endl;
		}
		else
		{
			std::cout << "Waning " << static_cast<int>(100. - ((phase.phasePercent - 50.) * 2.)) << "% - "
				<< static_cast<int>(MoonDays - phase.daysFromNew) << " days to New" << std::endl;
		}
		nextPhase = 0;
	}

	return nextPhase;
}

constexpr double MoonsPerYear = 12.3685;

std::array<const char*, 4> s_phaseName{"New Moon", "Waxing Quarter", "Full Moon", "Waning Quarter"};

static double lastJDEForPhase(int phase, const ADateTime& dateTime, double& Y, double& K, double& T)
{
	// Computes from the year and the year-day
	Y = static_cast<double>(dateTime.year()) + (static_cast<double>(dateTime.m_timeStruct.tm_yday) / 365.25);

	double K1 = (Y - 2000.) * MoonsPerYear;

	K = floor(K1) + (phase * 0.25);    // Phase is full (2)*0.25

	T = K / 1236.85;

	double JDE = 2451550.09765 + (29.530588853 * K) + (0.0001337 * pow(T, 2.)) - (0.00000015 * pow(T, 3.)) + (0.00000000073 * pow(T, 4.));

	if (AMoon::m_verboseLevel & DebugComputation)
	{
		std::cout << "--> Converting " << dateTime.year() << "-" << dateTime.month() << "-" << dateTime.day()
			<< " and [" << dateTime.m_timeStruct.tm_yday << " days]=" << dateTime.m_julian << " as " << Y << " years" << std::endl;
		std::cout << "--> Last JDE for " << s_phaseName[phase] << " = " << JDE << " Number of Moons (since 2000) = " << K1 << std::endl;
	}

	return JDE;
}

void AMoon::computeNextPhase(int phase, ADateTime& dateTime)
{
	double Y = 0;
	double K = 0;
	double T = 0;
	double JDE = lastJDEForPhase(phase, dateTime, Y, K, T);

	// Look for next phase
	while (JDE < dateTime.m_julian)
	{
		K += 1.;
		T = K / 1236.85;
		JDE = 2451550.09765 + (29.530588853 * K) + (0.0001337 * pow(T, 2.)) - (0.00000015 * pow(T, 3.)) + (0.00000000073 * pow(T, 4.));
	}

	if (m_verboseLevel & DebugComputation)
	{
		std::cout << "--> Date Conversion Y = " << Y << "; K phase = " << K << "; T phase = " << T << "; JDE = " << JDE << std::endl;
	}

	double T2 = T * T;
	double T3 = T2 * T;
	double T4 = T3 * T;

#if 0
	double E = 1. - (0.002516 * T) - (0.0000074 * pow(T, 2.));
	double M = 2.5534 + (29.10535669 * K) - (0.0000218 * pow(T, 2.)) - (0.00000011 * pow(T, 3.));
	double MS = 201.5643 + (385.81693528 * K) + (0.0107438 * pow(T, 2.)) + (0.00001239 * pow(T, 3.)) - (0.000000058 * pow(T, 4.));
	double F = 160.7108 + (390.67050274 * K) - (0.0016341 * pow(T, 2.)) - (0.00000227 * pow(T, 3.)) + (0.000000011 * pow(T, 4.));
	double Omega = 124.7746 - (1.5637558 * K) + (0.0020691 * pow(T, 2.)) + (0.00000215 * pow(T, 3.));
#endif
	double E = 1. - (0.002516 * T) - (0.0000074 * T2);
	double M = 2.5534 + (29.10535669 * K) - (0.0000218 * T2) - (0.00000011 * T3);
	double MS = 201.5643 + (385.81693528 * K) + (0.0107438 * T2) + (0.00001239 * T3) - (0.000000058 * T4);
	double F = 160.7108 + (390.67050274 * K) - (0.0016341 * T2) - (0.00000227 * T3) + (0.000000011 * T4);
	double Omega = 124.7746 - (1.5637558 * K) + (0.0020691 * T2) + (0.00000215 * T3);

	M = radianConvert(M);
	MS = radianConvert(MS);
	F = radianConvert(F);
	Omega = radianConvert(Omega);

	// Argumente der Planeten
#if 0
	double A1 = radianConvert(299.77 + (0.107408 * K) - (0.009173 * pow(T, 2.)));
#endif
	double A1 = radianConvert(299.77 + (0.107408 * K) - (0.009173 * T2));
	double A2 = radianConvert(251.88 + (0.016321 * K));
	double A3 = radianConvert(251.83 + (26.651886 * K));
	double A4 = radianConvert(349.42 + (36.412478 * K));
	double A5 = radianConvert(84.66 + (18.206239 * K));
	double A6 = radianConvert(141.74 + (53.303771 * K));
	double A7 = radianConvert(207.14 + (2.453732 * K));
	double A8 = radianConvert(154.84 + (7.30686 * K));
	double A9 = radianConvert(34.52 + (27.261239 * K));
	double A10 = radianConvert(207.19 + (0.121824 * K));
	double A11 = radianConvert(291.34 + (1.844379 * K));
	double A12 = radianConvert(161.72 + (24.198154 * K));
	double A13 = radianConvert(239.56 + (25.513099 * K));
	double A14 = radianConvert(331.55 + (3.592518 * K));

	double PK = (0.000325 * sin(A1)) + (0.000165 * sin(A2)) + (0.000164 * sin(A3))
		+ (0.000126 * sin(A4)) + (0.00011 * sin(A5)) + (0.000062 * sin(A6))
		+ (0.00006 * sin(A7)) + (0.000056 * sin(A8)) + (0.000047 * sin(A9))
		+ (0.000042 * sin(A10)) + (0.00004 * sin(A11)) + (0.000037 * sin(A12))
		+ (0.000035 * sin(A13)) + (0.000023 * sin(A14));

	double PT, W{ 0 };

	switch (phase)
	{
	default:
	case 0: // Periodische Terme Neumond (New Moon)
		PT = (-0.4072 * sin(MS))
			+ (0.17241 * E * sin(M))
			+ (0.01608 * sin(2 * MS))
			+ (0.01039 * sin(2 * F))
			+ (0.00739 * E * sin(MS - M))
			- (0.00514 * E * sin(MS + M))
			+ (0.00208 * pow(E, 2.) * sin(2 * M))
			- (0.00111 * sin(MS - (2 * F)))
			- (0.00057 * sin(MS + 2 * F))
			+ (0.00056 * E * sin(2 * MS + M))
			- (0.00042 * sin(3 * MS))
			+ (0.00042 * E * sin(M + 2 * F))
			+ (0.00038 * E * sin(M - 2 * F))
			- (0.00024 * E * sin(2 * MS - M))
			- (0.00017 * sin(Omega))
			- (0.00007 * sin(MS + 2 * M))
			+ (0.00004 * sin(2 * MS - 2 * F))
			+ (0.00004 * sin(3 * M))
			+ (0.00003 * sin(MS + M - 2 * F))
			+ (0.00003 * sin(2 * MS + 2 * F))
			- (0.00003 * sin(MS + M + 2 * F))
			+ (0.00003 * sin(MS - M + 2 * F))
			- (0.00002 * sin(MS - M - 2 * F))
			- (0.00002 * sin(3 * MS + M))
			+ (0.00002 * sin(4 * MS));

		break;

	case 2: // Periodische Terme Vollmond (Full Moon)
		PT = (-0.40614 * sin(MS))
			+ (0.17302 * E * sin(M))
			+ (0.01614 * sin(2 * MS))
			+ (0.01043 * sin(2 * F))
			+ (0.00734 * E * sin(MS - M))
			- (0.00515 * E * sin(MS + M))
			+ (0.00209 * pow(E, 2.) * sin(2 * M))
			- (0.00111 * sin(MS - 2 * F))
			- (0.00057 * sin(MS + 2 * F))
			+ (0.00056 * E * sin(2 * MS + M))
			- (0.00042 * sin(3 * MS))
			+ (0.00042 * E * sin(M + 2 * F))
			+ (0.00038 * E * sin(M - 2 * F))
			- (0.00024 * E * sin(2 * MS - M))
			- (0.00017 * sin(Omega))
			- (0.00007 * sin(MS + 2 * M))
			+ (0.00004 * sin(2 * MS - 2 * F))
			+ (0.00004 * sin(3 * M))
			+ (0.00003 * sin(MS + M - 2 * F))
			+ (0.00003 * sin(2 * MS + 2 * F))
			- (0.00003 * sin(MS + M + 2 * F))
			+ (0.00003 * sin(MS - M + 2 * F))
			- (0.00002 * sin(MS - M - 2 * F))
			- (0.00002 * sin(3 * MS + M))
			+ (0.00002 * sin(4 * MS));

		break;

	case 1:
	case 3: // Periodische Terme erstes und letztes Phase
		PT = (-0.62801 * sin(MS))
			+ (0.17172 * E * sin(M))
			- (0.01183 * E * sin(MS + M))
			+ (0.00862 * sin(2 * MS))
			+ (0.00804 * sin(2 * F))
			+ (0.00454 * E * sin(MS - M))
			+ (0.00204 * pow(E, 2.) * sin(2 * M))
			- (0.0018 * sin(MS - 2 * F))
			- (0.0007 * sin(MS + 2 * F))
			- (0.0004 * sin(3 * MS))
			- (0.00034 * E * sin(2 * MS - M))
			+ (0.00032 * E * sin(M + 2 * F))
			+ (0.00032 * E * sin(M - 2 * F))
			- (0.00028 * pow(E, 2.) * sin(MS + 2 * M))
			+ (0.00027 * E * sin(2 * MS + M))
			- (0.00017 * sin(Omega))
			- (0.00005 * sin(MS - M - 2 * F))
			+ (0.00004 * sin(2 * MS + 2 * F))
			- (0.00004 * sin(MS + M + 2 * F))
			+ (0.00004 * sin(MS - 2 * M))
			+ (0.00003 * sin(MS + M - 2 * F))
			+ (0.00003 * sin(3 * M))
			+ (0.00002 * sin(2 * MS - 2 * F))
			+ (0.00002 * sin(MS - M + 2 * F))
			- (0.00002 * sin(3 * MS + M));

		W = 0.00306 - (0.00038 * E * cos(M)) + (0.00026 * cos(MS))
			- (0.00002 * cos(MS - M)) + (0.00002 * cos(MS + M)) + (0.00002 * cos(2 * F));

		if (phase == 3)
		{
			W = -W;
		}

		break;
	}  // End switch

	double delta = deltaT(static_cast<double>(dateTime.year())) / 86400;

	double total = (JDE + PK + PT + W - delta);

	if (m_verboseLevel & DebugComputation)
	{
		// return KALENDERDATUM(JDE + PK + PT + W + (m_timeZone / 24) - deltaT(Year(ThisDate)) / 86400);
		// std::cout << "PK = " << PK << "; PT = " << PT << "; W = " << W << "; delta = " << delta << " <== " << tmpBuf << std::endl;
		std::cout << "PK = " << PK << "; PT = " << PT << "; W = " << W << "; delta = " << delta << " <== " << total << std::endl;
	}

	dateTime.setJulianDateTime(total);

	std::cout << "Next " << s_phaseName[phase] << ": " << dateTime.asString("%F %T [UTC]") << std::endl;
}




int AMoon::nextMoonPhase(const ADateTime& dateTime, int phase)
{
	AMoon nowPhase(*this);
	ADateTime newDate = dateTime;

	// default new moon = 0 - passed in

	std::cout << "---------------------------------------" << std::endl;
	nowPhase.computeNextPhase(phase, newDate);
	std::cout << "---------------------------------------" << std::endl;
	phase = (++phase) % 4;
	nowPhase = *this;
	nowPhase.computeNextPhase(phase, newDate);
	std::cout << "---------------------------------------" << std::endl;
	phase = (++phase) % 4;
	nowPhase = *this;
	nowPhase.computeNextPhase(phase, newDate);
	std::cout << "---------------------------------------" << std::endl;
	phase = (++phase) % 4;
	nowPhase = *this;
	nowPhase.computeNextPhase(phase, newDate);
	std::cout << "---------------------------------------" << std::endl;

	return phase;
}

//==========================================================================
// From http://www.stargazing.net/kepler/
//==========================================================================

// static constexpr double Pi2 = 6.283185307;
static constexpr double Pi2 = 2 * M_PI;
static constexpr double ARC = 206264.8062;
static constexpr double COSEPS = 0.91748;
static constexpr double SINEPS = 0.39778;

// returns ra and dec of Moon to 5 arc min (ra) and 1 arc min (dec)
// for a few centuries either side of J2000.0
// Predicts rise and set times to within minutes for about 500 years
// in past - TDT and UT time diference may become significant for long
// times
void moon(double t, double &ra, double &dec)
{

	double L0 = AlgBase::fpart(.606433 + 1336.855225 * t);     // mean long Moon in revs
	double L = Pi2 * AlgBase::fpart(.374897 + 1325.55241 * t);  // mean anomaly of Moon
	double LS = Pi2 * AlgBase::fpart(.993133 + 99.997361 * t);  // mean anomaly of Sun
	double d = Pi2 * AlgBase::fpart(.827361 + 1236.853086 * t); // diff longitude sun and moon
	double F = Pi2 * AlgBase::fpart(.259086 + 1342.227825 * t); // mean arg latitude

	// longitude correction terms
	double dL = 22640 * sin(L) - 4586 * sin(L - 2 * d);
	dL = dL + 2370 * sin(2 * d) + 769 * sin(2 * L);
	dL = dL - 668 * sin(LS) - 412 * sin(2 * F);
	dL = dL - 212 * sin(2 * L - 2 * d) - 206 * sin(L + LS - 2 * d);
	dL = dL + 192 * sin(L + 2 * d) - 165 * sin(LS - 2 * d);
	dL = dL - 125 * sin(d) - 110 * sin(L + LS);
	dL = dL + 148 * sin(L - LS) - 55 * sin(2 * F - 2 * d);

	// latitude arguments
	double S = F + (dL + 412 * sin(2 * F) + 541 * sin(LS)) / ARC;
	double h = F - 2 * d;

	// latitude correction terms
	double N = -526 * sin(h) + 44 * sin(L + h) - 31 * sin(h - L) - 23 * sin(LS + h);
	N = N + 11 * sin(h - LS) - 25 * sin(F - 2 * L) + 21 * sin(F - L);

	double lmoon = Pi2 * AlgBase::fpart(L0 + dL / 1296000.);  // Lat in rads
	double bmoon = (18520. * sin(S) + N) / ARC;     // long in rads

	// convert to equatorial coords using a fixed ecliptic
	double CB = cos(bmoon);
	double x = CB * cos(lmoon);

	double V = CB * sin(lmoon);
	double W = sin(bmoon);
	double y = COSEPS * V - SINEPS * W;
	double Z = SINEPS * V + COSEPS * W;
	double rho = sqrt(1. - Z * Z);

	dec = (360. / Pi2) * atan(Z / rho);
	ra = (48. / Pi2) * atan(y / (x + rho));

	if (ra < 0)
	{
		ra = ra + 24.;
	}
}

// Returns RA and DEC of Sun to roughly 1 arcmin for few hundred
// years either side of J2000.0
void sun(double t, double &ra, double &dec)
{
	double m = Pi2 * AlgBase::fpart(0.993133 + 99.997361 * t);        // Mean anomaly
	double dL = 6893. * sin(m) + 72. * sin(2 * m);          // Eq centre

	double L = Pi2 * AlgBase::fpart(0.7859453 + m / Pi2 + (6191.2 * t + dL) / 1296000.);

	// convert to RA and DEC - ecliptic latitude of Sun taken as zero
	double sl = sin(L);
	double x = cos(L);
	double y = COSEPS * sl;
	double Z = SINEPS * sl;
	double rho = sqrt(1. - Z * Z);

	dec = (360. / Pi2) * atan(Z / rho);
	ra = (48. / Pi2) * atan(y / (x + rho));
	if (ra < 0)
	{
		ra = ra + 24.;
	}
}


static double sinalt(int iobj, double mjd0, double hour)
{

	// ' returns sine of the altitude of either the sun or the moon given the
	// ' modified julian day number at midnight UT and the hour of the UT day,
	// ' the longitude of the observer, and the sine and cosine of the latitude
	// ' of the observer
	double ra = 0;
	double dec = 0;

	double instant = mjd0 + hour / 24.;
	double t = (instant - 51544.5) / 36525.;

	double altitude = -1;

	if (ADateTime::m_verboseLevel & DebugJulianDate)
	{
		std::cout << "Converting Julian " << instant << " to local " << static_cast<int>(hour) << " hour sidereal: " << t << std::endl;
	}
 
	if (iobj == 0)
	{
		moon(t, ra, dec);
		altitude = AlgBase::localAltitude(instant, ra, dec);
		if (AMoon::m_verboseLevel & DebugComputation)
		{
			std::cout << "Moon (" << hour << "hr): RA = " << ra << " Decl = " << dec << " Alt = " << altitude << std::endl;
		}
	}
	else
	{
		sun(t, ra, dec);
		altitude = AlgBase::localAltitude(instant, ra, dec);
		if (AMoon::m_verboseLevel & DebugComputation)
		{
			std::cout << "Sun (" << hour << "hr): RA = " << ra << " Decl = " << dec << " Alt = " << altitude << std::endl;
		}
	}

	return altitude;
}


static AObject objects[3]
{
	{"Moon", AltitudeType::MoonObject, true},
	{"Sun",  AltitudeType::ActualSun, false},
	{"Nautical twilight", AltitudeType::NauticalSun, false}
};


// Approximation Method
void AMoon::moonRise(const ADateTime& procTime)
{
	ADateTime dateTime = procTime;
	std::cout << std::endl << "-------------Moon-Sun-Rise/Set-------------------" << std::endl;

	// UTC with time-zone adjusted - midnight local time
	double date = dateTime.modifiedJuiianDate(true);

	if (m_verboseLevel & DebugJulianDate)
	{
		std::cout << "Modified Julian (for moonRise): " << date << std::endl;
	}

	// define the altitudes for each object
	// treat twilight as a separate object 3, so sinalt routine
	// falls through to finding Sun altitude again

	for (int iobj = 0; iobj < 3; iobj++)
	{
		AObject& obj = objects[iobj];

		obj.resetSettings();

		double hour = 1.;
		double sinho = obj.m_sinHorizontal;

		double yPrior = sinalt(iobj, date, hour - 1) - sinho;

		double yCurr = 0.;
		double yNext = 0.;

		// Check if object already above the horizon
		obj.m_above  = (yPrior > 0);

		// used later to classify non-risings
		do {
			yCurr = sinalt(iobj, date, hour) - sinho;
			yNext = sinalt(iobj, date, hour + 1) - sinho;

			// Approximate for zero and compute for next
			obj.adjustForNext(yPrior, yCurr, yNext, hour);

			yPrior = yNext;     // 'reuse the ordinate in the next interval

			hour = hour + 2;    // Skip to get next block

			if (m_verboseLevel & DebugComputation)
			{
				std::cout << "------ solution: prev = " << yPrior << " curr = " << yCurr << " next = " << yNext << std::endl;
			}
		} while ((hour < 25) && (!(obj.m_rise && obj.m_sett)));

		if (m_verboseLevel & DebugComputation)
		{
			std::cout << "-Final solution: prev = " << yPrior << " curr = " << yCurr << " next = " << yNext << std::endl;
		}

		if (iobj != 0)
		{
			std::cout << std::endl;
		}

		// Just note, utrise and utset times are actually local time UTC time
		obj.printRiseSetTimes();

	}   // End for objects

	std::cout << "-------------------------------------------------" << std::endl << std::endl;
}

