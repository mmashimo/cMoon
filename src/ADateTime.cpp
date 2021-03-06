/// @file
///
/// @brief ADateTime class implementation.
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

#include "pch.h"
#include <cstdio>
#include <iostream>
#include <math.h>
#include <string.h>

#include "ADateTime.h"

#include "AlgBase.h"

using namespace std;

// Static class var:
int ADateTime::m_verboseLevel = 1;

// double ADateTime::m_timeZone = -4;   // EDT
double ADateTime::m_defaultTimeZone = -5;   // EST
bool ADateTime::m_useDST = true;

bool ADateTime::m_autoComputeTimeZone = true;

constexpr std::array<int, 12> s_MonthDays{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// To use strftime(), refer to:
// http://www.cplusplus.com/reference/ctime/strftime/
static const char* DateTimeFormat24h = "%a %F %T";  // Generic Date-Time format: 'Wkd YYYY-MM-YY HH:MM:SS' 24-hour
static const char* DateTimeFormat12h = "%a %F %r";  // Generic Date-Time format: 'Wkd YYYY-MM-YY HH:MM:SS a/pm' 12-hour

ADateTime::ADateTime()
{
    todaysDate(false);
}

ADateTime::ADateTime(const ADateTime& ref)
{
    copyHelper(ref);
}

ADateTime::~ADateTime()
{
}

ADateTime& ADateTime::operator=(const ADateTime& ref)
{
    copyHelper(ref);
    return *this;
}

void ADateTime::copyHelper(const ADateTime& ref)
{
	m_parsedCorrectly     = ref.m_parsedCorrectly;
	m_julian              = ref.m_julian;
    m_timeZone            = ref.m_timeZone;
	m_parsedDate          = ref.m_parsedDate;
	m_parsedTime          = ref.m_parsedTime;
	m_timeStruct          = ref.m_timeStruct;
    m_localTimeStruct     = ref.m_localTimeStruct;
	m_rawTime             = m_rawTime;
}

void ADateTime::printASCII() const
{
    // convert now to string form
    char tmpStr[256];

    // Date-Time format: 'Wkd YYYY-MM-YY HH:MM:SS' 24-hour
    strftime(tmpStr, 256, DateTimeFormat24h, &m_timeStruct);
    std::cout << "Date and time [UTC]: " << tmpStr << std::endl;
    if (m_verboseLevel & DebugJulianDate)
    {
        std::cout << std::fixed;
        std::cout << "Julian = " << m_julian << " [" << year() << "-" << month() << "-" << day() << "]" << std::endl;
    }
}

void ADateTime::showTime(const int format) const
{
    char tmpStr[256];
}

void ADateTime::showDateTime(const int format) const
{
    char tmpStr[256];
    strftime(tmpStr, 256, DateTimeFormat24h, &m_timeStruct);
    std::cout << "UTC:    " << tmpStr << std::endl;
    strftime(tmpStr, 256, DateTimeFormat12h, &m_localTimeStruct);
    std::cout << "Local:  " << tmpStr << std::endl;

    // Julian at Noon
    double jd = julianDay(true);
    sprintf(tmpStr, "%f", jd);
    std::cout << "Julian: " << tmpStr << std::endl;

    jd = julianDay(false);
    sprintf(tmpStr, "%f", jd);
    std::cout << "JTime:  " << tmpStr << std::endl;

    jd = j2000Day(true);
    sprintf(tmpStr, "%f", jd);
    std::cout << "J2000D: " << tmpStr << std::endl;

    jd = j2000Day(false);
    sprintf(tmpStr, "%f", jd);
    std::cout << "J2000:  " << tmpStr << std::endl;
}

int ADateTime::year() const
{
    return m_timeStruct.tm_year + 1900;
}

int ADateTime::month() const
{
    return m_timeStruct.tm_mon + 1;
}

int ADateTime::day() const
{
    return m_timeStruct.tm_mday;
}

void ADateTime::addDays(const int days)
{
    time_t baseTime = mktime(&m_timeStruct);
    baseTime += (days * 86400);
    m_timeStruct = *gmtime(&baseTime);
    constructDateTimeArray(m_timeStruct, true);
}

const struct tm& ADateTime::getTimeStruct(const bool local) const
{
    if (local)
    {
        return m_localTimeStruct;
    }

    return m_timeStruct;
}


void ADateTime::setVerboseMode(const int level)
{
    m_verboseLevel = level;
}

void ADateTime::todaysDate(const bool bUTC)
{
    // current date/time based on current system
    m_rawTime = time(nullptr);

    m_timeStruct = *gmtime(&m_rawTime);

    setTimeZone();
    m_localTimeStruct = *localtime(&m_rawTime);

    if (bUTC)
    {
        // convert now to tm struct for UTC
        constructDateTimeArray(m_timeStruct, bUTC);
    }
    else
    {
        constructDateTimeArray(m_localTimeStruct, false);
    }

    m_julian = julianDay(true);

	m_parsedCorrectly = true;
}

void ADateTime::setTimeZone()
{
    // Check local time and see if we need to set the DST bit
    if (m_autoComputeTimeZone)
    {
        m_timeZone = m_defaultTimeZone;
    }
    else
    {
        m_timeZone = m_defaultTimeZone;
    }

}

bool ADateTime::parseDateTime(const char* ar)
{
    bool bGoodDate = false;

    // Date or time entry
    if (nullptr != strchr(ar, '-'))
    {
        bGoodDate = parseDate(ar, m_parsedDate);
        if (!bGoodDate)
        {
            std::cout << "!!! Invalid date string: '" << ar << " <= found '-' but date not in YYYY-MM-DD format" << std::endl;
        }
    }
    else if (nullptr != strchr(ar, ':'))
    {
        bGoodDate = parseTime(ar);
        if (!bGoodDate)
        {
            std::cout << "!!! Invalid time string: '" << ar << " <= found ':' but time not in HH:MM[:SS] format" << std::endl;
        }
    }
    else if ((nullptr != strchr(ar, '.')) || (isdigit(*ar)))
    {
        // Compute using Julian date/time
        bGoodDate = parseJulianTime(ar);
        if (!bGoodDate)
        {
            std::cout << "!!! Invalid Julian decimal format: '" << ar << " <= found '.' but date not in J.d format" << std::endl;
        }
    }
    else
    {
        // Parse time by number of seconds from midnight
        bGoodDate = parseTime(ar);
        if (!bGoodDate)
        {
            std::cout << "!!! Invalid time string: '" << ar << " <= trying number of seconds from midnight" << std::endl;
        }
    }

    return bGoodDate;
}


bool ADateTime::parseDate(const DateString& arg, ParsedDate& parsedDate)
{
    bool isOK{ false };

    char stmp[1024];
    size_t len = arg.size();
    strcpy(stmp, arg.c_str());

    // Check if we have a YY-MM-DD (dash delimiter) or decimal point (Julian)
    char* ptr = strchr(stmp, '-');
    if (ptr != nullptr)
    {
        int idx = 0;
        bool bContainsDelim = false;
        int dateIdx = 0;
        ptr = stmp;
        char* startPtr = stmp;

        // Check if we have a YY-MM-DD (dash delimiter) or decimal point (Julian)
        for (size_t istr = 0; istr < len; istr++, ptr++)
        {
            if (stmp[istr] == '-')
            {
                bContainsDelim = true;

                *ptr = '\0';
                m_parsedDate[dateIdx] = atoi(startPtr);
                dateIdx++;

                if ((istr + 1) < len)
                {
                    // If at end, this could throw exception using it
                    startPtr = ptr + 1;
                    if (dateIdx == 2)
                    {
                        m_parsedDate[dateIdx] = atoi(startPtr);
                        break;
                    }
                }
            }
        }

        isOK = (dateIdx == 2) && isParsedDateOk(m_parsedDate);
        if (isOK)
        {
            convertDateFromArray(m_parsedDate, m_parsedTime);
            m_julian = julianDay(true);
        }
    }
    else
    {
        double jd = atof(stmp);
        setFromJulian(jd);
        std::cout << "Date (" << stmp << "): " << asString("%c").c_str() << " UTC" << std::endl;
        isOK = true;
    }

	m_parsedCorrectly = isOK;

    return isOK;
}

bool ADateTime::parseTime(const std::string& arg)
{
	bool isOK{ true };
	char stmp[1024];
	size_t len = arg.size();
	strcpy(stmp, arg.c_str());

	char *p = stmp;
	char* ptr = strchr(stmp, ':');

	if (ptr == nullptr)
	{
        // Time string has no ":"
		// Treat the string as number of seconds from midnight
		uint32_t secs = atoi(stmp);
		// NOTE use of integer math - floors
		m_parsedTime.m_Time[2] = secs / 3600;
		uint32_t tmp = secs % 3600;
		m_parsedTime.m_Time[1] = tmp / 60;
		m_parsedTime.m_Time[0] = m_parsedTime.m_Time[1] % 60;
		// Always express in 24hr
		m_parsedTime.m_24hr = true;
		m_parsedTime.m_amPm = m_parsedTime.m_Time[2] >= 12;  // Doesn't matter but set, any way
	}
	else
	{
		// String needs parsing
		*ptr = '\0';
		m_parsedTime.m_Time[2] = atoi(p);
		p = ptr + 1;
		ptr = strchr(p, ':');
		if (ptr == nullptr)
		{
			// We have HH:MM, no seconds
			m_parsedTime.m_Time[1] = atoi(p);
			m_parsedTime.m_Time[0] = 0;
			while (isdigit(*p))
			{
				p++;
			}
			if (*p != '\0')
			{
				// Check if 'am' or 'pm' is used
				if ((*p == 'a') || (*p == 'A'))
				{
					m_parsedTime.m_24hr = false;
					m_parsedTime.m_amPm = false;
				}
				else if ((*p == 'p') || (*p == 'P'))
				{
					m_parsedTime.m_24hr = false;
					m_parsedTime.m_amPm = true;
				}
				else
				{
					m_parsedTime.m_24hr = true;
					m_parsedTime.m_amPm = m_parsedTime.m_Time[2] >= 12;
				}
			}
		}
	}
	
	m_parsedCorrectly = isOK;

	return true;
}

bool ADateTime::parseJulianTime(const std::string& arg)
{
    double jd = atof(arg.c_str());

    // Set Julian date-time and update this struct
    setJulianDateTime(jd);

    return jd != 0;
}

bool ADateTime::isParsedDateOk(const ParsedDate& parsed) const
{
    bool bOk = true;
    // No need to check year - possibly as 1760 Gregorian, but not in scope
    // Check month
    if (parsed[1] <= 0 || parsed[1] > 12)
    {
        // Return here, because we cannot access the month array
        return false;
    }

    // Check day, except for Feb (see below)
    if (parsed[2] <= 0 || parsed[2] > 31)
    {
        bOk = false;
    }
    else if (parsed[1] != 2)
    {
        // Check monthly contents
        if (parsed[2] > s_MonthDays[parsed[1] - 1])
        {
            bOk = false;
        }
    }
    else
    {
        // Check Feb
        int monthChk = s_MonthDays[1];
        if ((parsed[0] % 4) == 0)
        {
            // Every century, we keep lose the leap year
            if ((parsed[0] % 100) == 0)
            {
                // Except every 400yrs, we regain the leap year
                monthChk += ((parsed[0] % 400) == 0) ? 1 : 0;
            }
            else
            {
                monthChk++;
            }
            if (parsed[2] > monthChk)
            {
                bOk = false;
            }
        }
    }
    return bOk;
}


int ADateTime::dayOfYear() const
{
#ifdef RECOMPILE_TM
    mktime(&m_timeStruct);
#endif
    return m_timeStruct.tm_yday;
}

double ADateTime::julian() const
{
    return m_julian;
}

double ADateTime::timeZoneAsFractionOfDay() const
{
    return m_timeZone / 24.;
}

bool ADateTime::isParsedCorrectly() const
{
    return m_parsedCorrectly;
}

void ADateTime::constructDateTimeArray(const struct tm& timeStruct, const bool utc, const bool hr24)
{
    m_parsedDate[0] = timeStruct.tm_year + 1900;
    m_parsedDate[1] = timeStruct.tm_mon + 1;
    m_parsedDate[2] = timeStruct.tm_mday;

    m_parsedTime.m_Time[1] = timeStruct.tm_min;
    m_parsedTime.m_Time[2] = timeStruct.tm_sec;

    m_parsedTime.m_utc = utc;
    m_parsedTime.m_amPm = (timeStruct.tm_hour < 12);
    m_parsedTime.m_24hr = hr24;

    if (hr24)
    {
        m_parsedTime.m_Time[0] = timeStruct.tm_hour;
    }
    else
    {
        m_parsedTime.m_Time[0] = (timeStruct.tm_hour <= 12)
            ? timeStruct.tm_hour : (timeStruct.tm_hour - 12);
        if (m_parsedTime.m_Time[0] == 0)
            m_parsedTime.m_Time[0] = 12; // 12am
    }

}


void ADateTime::convertDateFromArray(const ParsedDate& parsedDate, const ParsedTime& parsedTime, const bool toUtc)
{
    m_timeStruct.tm_year = parsedDate[0] - 1900;
    m_timeStruct.tm_mon = parsedDate[1] - 1;
    m_timeStruct.tm_mday = parsedDate[2];

    if (toUtc)
    {
        // Converting local time to UTC
        mktime(&m_timeStruct);

        if (parsedTime.m_24hr)
        {
            m_timeStruct.tm_hour = parsedTime.m_Time[0];
        }
        else if (parsedTime.m_amPm)
        {
            m_timeStruct.tm_hour = (parsedTime.m_Time[0] == 0) ? 12 : parsedTime.m_Time[0];
        }
        else
        {
            m_timeStruct.tm_hour = (parsedTime.m_Time[0] == 12) ? 12 : parsedTime.m_Time[0] + 12;
        }

        m_timeStruct.tm_min = parsedTime.m_Time[1];
        m_timeStruct.tm_sec = parsedTime.m_Time[2];

        m_timeStruct.tm_isdst = 0;
    }
    else
    {
        // Convert to local time
        mktime(&m_timeStruct);
    }
}


void ADateTime::convertJulianToTime(const double jd, struct tm& timeOnly)
{
    // Get time while at it
    double dayDecimal = jd - floor(jd);
    double hr = dayDecimal * 24.;
    timeOnly.tm_hour = static_cast<int>(hr);
    double min = (hr - floor(hr)) * 60.;
    timeOnly.tm_min = static_cast<int>(min);
    double sec = (min - floor(min)) * 60.;
    timeOnly.tm_sec = static_cast<int>(sec);
    // UTC, has not DST
    timeOnly.tm_isdst = 0;

    if (m_verboseLevel & DebugJulianDate)
    {
        std::cout << "Julian conversion (" << jd << "): " << hr << " hr : " << min << " m : " << sec << " s" << std::endl;
    }
}


void ADateTime::setFromJulian(const double& jd)
{
    // https://aa.usno.navy.mil/faq/docs/JD_Formula.php
    m_julian = jd;

    AlgBase::convertJulianToDate(jd, m_parsedDate[0], m_parsedDate[1], m_parsedDate[2]);

    // Get time while at it
    convertJulianToTime(jd, m_timeStruct);

    convertDateFromArray(m_parsedDate, m_parsedTime);
}

std::string ADateTime::asString(const struct tm& tmStruct, const std::string& format) const
{
    char tmp[1024];
    size_t len = strftime(tmp, 1023, format.c_str(), &tmStruct);
    return std::string(tmp);
}

std::string ADateTime::asString(const std::string& format) const
{
    char tmp[1024];
    size_t len = strftime(tmp, 1023, format.c_str(), &m_timeStruct);
    return std::string(tmp);
}


void ADateTime::setJulianDateTime(const double jd)
{
    // Update other components of date-time if set
    m_julian = updateTimeStructFromJulianDateTime(jd, m_parsedDate, m_parsedTime);
}

double ADateTime::updateTimeStructFromJulianDateTime(const double jd, ParsedDate& parsedDate, ParsedTime& parsedTime)
{
    double D = getJulianDate(jd, parsedDate);

    // double S = getTimeFromJulian(D, parsedTime.m_Time[0], parsedTime.m_Time[1], parsedTime.m_Time[2]);
    double S = AlgBase::convertJulianToTime(D, parsedTime.m_Time[0], parsedTime.m_Time[1], parsedTime.m_Time[2]);

    //    KALENDERDATUM = DateSerial(Y, M, Int(D)) + TimeSerial(H, Mi, S)
    if (m_verboseLevel & DebugJulianDate)
    {
        std::cout << "Julian conversion of: " << jd << " as " << " D=" << D << " " << m_timeStruct.tm_hour << "hr : " << m_timeStruct.tm_min << " m : " << S << " s" << std::endl;
    }

    // Copy from parsed time to tm_struct - make sure the timesettings are correct
    parsedTime.m_24hr = true;
    parsedTime.m_utc = true;

	convertDateFromArray(parsedDate, parsedTime, true);

    return jd;
}


double ADateTime::getJulianDate(const double jd, ParsedDate& parsedDate)
{
    // Dim Z, F, Alpha, A, B, C, D, E, M, Y, H, Mi, S
    double JD = jd + 0.5;
    double Z = floor(JD);
    double F = JD - Z;

    // Need to convert this into integers
    double Alpha, A;
    if (Z >= 2299161.)
    {
        Alpha = floor((Z - 1867216.25) / 36524.25);
        A = Z + 1 + Alpha - floor(Alpha / 4);
    }
    else
    {
        A = Z;
    }

    double B = A + 1524;
    double C = floor((B - 122.1) / 365.25);
    double D = floor(365.25 * C);
    double E = floor((B - D) / 30.6001);
    D = B - D - floor(30.6001 * E) + F;

    int mon = static_cast<int>((E < 14) ? (E - 1) : (E - 13));

    parsedDate[0] = static_cast<int>( (mon > 2) ? (C - 4716) : (C - 4715));
    parsedDate[1] = mon;
    parsedDate[2] = static_cast<int>(D);

    return D;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------
//   returns modified julian date
//   number of days since 1858 Nov 17 00:00h
//   valid for any date since 4713 BC
//   assumes gregorian calendar after 1582 Oct 15, Julian before
//   Years BC assumed in calendar format, i.e. the year before 1 AD is 1 BC
double ADateTime::modifiedJuiianDate(const bool addTimeZone) const
{
    int y = year();
    int m = month();
    int d = day();

    double a = 10000 * y + 100 * m + d;
    double b;
    if (y < 0)
    {
        y = y + 1;
    }
    if (m <= 2)
    {
        m = m + 12;
        y = y - 1;
    }

    if (a <= 15821004.1)
    {
        b = -2 + (y + 4716) / 4 - 1179;
    }
    else
    {
        b = (y / 400) - (y / 100) + (y / 4);
    }

    a = (365. * y) - 679004;

    double utTime = a + b + static_cast<int>(30.6001 * (m + 1)) + d;

    if (m_verboseLevel & DebugJulianDate)
    {
        std::cout << "Modified JD: UTC (midnite) = " << utTime << " using [" << y << "-" << m << "-" << d << "]" << std::endl;
    }

    if (addTimeZone)
    {
        utTime = utTime - (timeZoneAsFractionOfDay());
        if (m_verboseLevel & DebugJulianDate)
        {
            std::cout << "Modified JD (uses local zone) = " << utTime << " using TZ=" << m_timeZone << std::endl;
        }
    }

    return utTime;
}

double ADateTime::julianDay(const bool noon) const
{
    double jd = AlgBase::convertDateToJulianNoon(year(), month(), day());

    if (noon)
    {
        return jd;
    }
    // Return Julian date at time - NOTE: Julian noon already is at noon, so subtract
    return jd + AlgBase::convertTimeToJulian(m_timeStruct.tm_hour, m_timeStruct.tm_min, m_timeStruct.tm_sec) - 0.5;
}

double ADateTime::j2000Day(const bool utcMidDay) const
{
    // Convert struct tm (as UTC) to J2000 date-time
    int y = year();
    int m = month();
    int d = day();

    // Get the days to J2000
    // NOTE: Only works between 1901 to 2099 - see Meeus chapter 7
    // Use of 730531.5 is 1901
    // 367 * y - 7 * (y + (m + 9) \ 12) \ 4 + 275 * m \ 9 + d - 730531.5 + h / 24
    // int days = (367 * y) - ((7 * (y + (m + 9) / 12)) / 4) + (275 * m / 9) + d;
    // (367*y)-int(7*(y+int((m+9)/12))/4)+int(275*12/9)+d+1721013.5

    int yearDays = (367 * y);
    int months = 7 * (y + ((m + 9) / 12)) / 4;
    int monDays = (275 * m) / 9;
    int days = yearDays - months + monDays + d;

    // Add the decimal hours to return
    double j2000 = (double)days - 730531.5;

    // Add hours in decimal
    double h = 0.;
    if (!utcMidDay)
    {
        h = (double)m_timeStruct.tm_hour;
        h += (double)m_timeStruct.tm_min / 60.;
        h += (double)m_timeStruct.tm_sec / 3600.;
        j2000 += ((h / 24.) - 0.5);
    }

    return j2000;
}

double ADateTime::j2000Noon() const
{
    return floor(m_julian) - 2451544.5;
}


#include <stdint.h>
// Conversion of FILETIME (input) to Unix time (return)
#define NANOSEC_PER_SECOND 10000000
#define EPOCH_DIFFERENCE 11644473600LL
time_t convertWindowsFileTimeToUnixTime(const uint64_t fileTime)
{
    uint64_t utime = fileTime / NANOSEC_PER_SECOND; // 100ns intervals to seconds;
    utime = utime - EPOCH_DIFFERENCE;                    // number of seconds between FILETIME 1601 to 1970 epochs
    return static_cast<time_t>(utime);
}

uint64_t convertUnixTimeToWindowsFileTime(const time_t utime)
{
    uint64_t ftime = utime + EPOCH_DIFFERENCE;
    return ftime * NANOSEC_PER_SECOND;
}

