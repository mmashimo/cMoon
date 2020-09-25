/// @file

#include "pch.h"
#include <cstdio>
#include <iostream>
#include <math.h>
#include <string.h>

#include "ADateTime.h"

using namespace std;

// Static class var:
int ADateTime::m_verboseLevel = 1;

// double ADateTime::m_timeZone = -4;   // EDT
double ADateTime::m_defaultTimeZone = -5;   // EST

bool ADateTime::m_autoComputeTimeZone = true;

constexpr std::array<int, 12> s_MonthDays{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// To use strftime(), refer to:
// http://www.cplusplus.com/reference/ctime/strftime/

ADateTime::ADateTime()
{
    todaysDate(true);
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

void ADateTime::printASCII()
{
    // convert now to string form
    char* dt = asctime(&m_timeStruct);
    std::cout << std::fixed;
    std::cout << "Date and time [UTC]: " << dt;
    if (m_verboseLevel & DebugJulianDate)
    {
        std::cout << "Julian = " << m_julian << " [" << year() << "-" << month() << "-" << day() << "]" << std::endl;
    }
}

int ADateTime::year() const
{
    // return m_parsedDate[0];
    return m_timeStruct.tm_year + 1900;
}

int ADateTime::month() const
{
    // return m_parsedDate[1];
    return m_timeStruct.tm_mon + 1;
}

int ADateTime::day() const
{
    // return m_parsedDate[2];
    return m_timeStruct.tm_mday;
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

    setTimeZone();

    m_julian = getJulianYear();

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

bool ADateTime::parseDate(const DateString& arg)
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
            convertDateFromArray();
            m_julian = getJulianYear();
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

double ADateTime::getJulianYear() const
{
    // Example of Julian Days is: 2017-3-1 should be 2457813.5
    // https://www.subsystems.us/uploads/9/8/9/4/98948044/moonphase.pdf

    // (367*y)-int(7*(y+int((m+9)/12))/4)+int(275*m/9)+d+1721013.5
    // 2001-12-05 should be 2452248.5 (734367 - 3506 + 366 + 5 + 1721013.5)
    int y = year();
    int m = month();
    int a = 367 * y;
    int b1 = (m + 9) / 12;
    int b = 7 * (y + b1) / 4;
    int c = 275 * m / 9;
    double jd = a - b + c + day() + 1721013.5;

    return jd;
}

int ADateTime::dayOfYear() const
{
    struct tm newDate { m_timeStruct };
    mktime(&newDate);
    return newDate.tm_yday;
}

void ADateTime::constructDateTimeArray(const struct tm& timeStruct, const bool utc, const bool hr24)
{
    m_parsedDate[0] = m_timeStruct.tm_year + 1900;
    m_parsedDate[1] = m_timeStruct.tm_mon + 1;
    m_parsedDate[2] = m_timeStruct.tm_mday;

    m_parsedTime.m_Time[1] = m_timeStruct.tm_min;
    m_parsedTime.m_Time[2] = m_timeStruct.tm_sec;

    m_parsedTime.m_utc = utc;
    m_parsedTime.m_amPm = (m_timeStruct.tm_hour < 12);
    m_parsedTime.m_24hr = hr24;
    if (hr24)
    {
        m_parsedTime.m_Time[0] = m_timeStruct.tm_hour;
    }
    else
    {
        m_parsedTime.m_Time[0] = (m_timeStruct.tm_hour <= 12)
            ? m_timeStruct.tm_hour : (m_timeStruct.tm_hour - 12);
        if (m_parsedTime.m_Time[0] == 0)
            m_parsedTime.m_Time[0] = 12; // 12am
    }

}

void ADateTime::convertDateFromArray(bool doTime)
{
    m_timeStruct.tm_year = m_parsedDate[0] - 1900;
    m_timeStruct.tm_mon = m_parsedDate[1] - 1;
    m_timeStruct.tm_mday = m_parsedDate[2];

    if (doTime)
    {
        // Converting UTC
        mktime(&m_timeStruct);

        if (m_parsedTime.m_24hr)
        {
            m_timeStruct.tm_hour = m_parsedTime.m_Time[0];
        }
        else if (m_parsedTime.m_amPm)
        {
            m_timeStruct.tm_hour = (m_parsedTime.m_Time[0] == 0) ? 12 : m_parsedTime.m_Time[0];
        }
        else
        {
            m_timeStruct.tm_hour = (m_parsedTime.m_Time[0] == 12) ? 12 : m_parsedTime.m_Time[0] + 12;
        }

        m_timeStruct.tm_min = m_parsedTime.m_Time[1];
        m_timeStruct.tm_sec = m_parsedTime.m_Time[2];

        m_timeStruct.tm_isdst = 0;
    }
    else
    {
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

    // Convert JD in AD
    int l = (int)(jd + 68569);
    int n = (4 * l) / 146097;
    l = l - (146097 * n + 3) / 4;
    int i = 4000 * (l + 1) / 1461001;
    l = l - 1461 * i / 4 + 31;
    int j = 80 * l / 2447;
    int k = l - 2447 * j / 80;
    l = j / 11;
    j = j + 2 - 12 * l;
    i = 100 * (n - 49) + i + l;
    m_parsedDate[0] = i;
    m_parsedDate[1] = j;
    m_parsedDate[2] = k;

    // Get time while at it
    convertJulianToTime(jd, m_timeStruct);

    convertDateFromArray();
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

void ADateTime::setJulianDateTime(double jd)
{
    double D = getJulianDate(jd, m_parsedDate[0], m_parsedDate[1], m_parsedDate[2]);
    double S = getJulianTime(D, m_parsedTime.m_Time[0], m_parsedTime.m_Time[1], m_parsedTime.m_Time[2]);

    //    KALENDERDATUM = DateSerial(Y, M, Int(D)) + TimeSerial(H, Mi, S)
    if (m_verboseLevel & DebugJulianDate)
    {
        std::cout << "Julian conversion of: " << jd << " as " << " D=" << D << " " << m_timeStruct.tm_hour << "hr : " << m_timeStruct.tm_min << " m : " << S << " s" << std::endl;
    }

    // Copy from parsed time to tm_struct - make sure the timesettings are correct
    m_parsedTime.m_24hr = true;
    m_parsedTime.m_utc = true;
	convertDateFromArray(true);
    m_julian = jd;
}

double ADateTime::getJulianDate(const double jd, int& yr, int& mon, int& day)
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

    if (E < 14)
    {
        mon = static_cast<int>(E - 1);
    }
    else
    {
        mon = static_cast<int>(E - 13);
    }

    if (mon > 2)
    {
        yr = static_cast<int>(C - 4716);
    }
    else
    {
        yr = static_cast<int>(C - 4715);
    }

    day = static_cast<int>(D);

    return D;
}

double ADateTime::getJulianTime(const double jd, int& hour, int& min, int& sec)
{
    double H = (jd - floor(jd)) * 24.;
    hour = static_cast<int>(floor(H));

    double M = (H - floor(H)) * 60.;
    min = static_cast<int>(floor(M));

    double fSeconds = (M - floor(M)) * 60.;
    sec = static_cast<int>(fSeconds);

    return fSeconds;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------
//   returns modified julian date
//   number of days since 1858 Nov 17 00:00h
//   valid for any date since 4713 BC
//   assumes gregorian calendar after 1582 Oct 15, Julian before
//   Years BC assumed in calendar format, i.e. the year before 1 AD is 1 BC
double ADateTime::modifiedJuiianDate(const bool addTimeZone)
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
        utTime = utTime - (m_timeZone / 24.);
        if (m_verboseLevel & DebugJulianDate)
        {
            std::cout << "Modified JD (uses local zone) = " << utTime << " using TZ=" << m_timeZone << std::endl;
        }
    }

    return utTime;
}

double ADateTime::computeJ2000(const bool utcMidDay)
{
    // Convert struct tm (as UTC) to J2000 date-time
    int y = year();
    int m = month();
    int d = day();

    // Add hours in decimal
    double h = 0.;
    if (!utcMidDay)
    {
        h = (double)m_timeStruct.tm_hour;
        h += (double)m_timeStruct.tm_min / 60.;
        h += (double)m_timeStruct.tm_sec / 3600.;
    }
    // Get the days to J2000
    // NOTE: Only works between 1901 to 2099 - see Meeus chapter 7
    // 367 * y - 7 * (y + (m + 9) \ 12) \ 4 + 275 * m \ 9 + d - 730531.5 + h / 24
    // int days = (367 * y) - ((7 * (y + (m + 9) / 12)) / 4) + (275 * m / 9) + d;
    // (367*y)-int(7*(y+int((m+9)/12))/4)+int(275*12/9)+d+1721013.5

    int yearDays = (367 * y);
    int months = 7 * (y + ((m + 9) / 12)) / 4;
    int monDays = (275 * m) / 9;
    int days = yearDays - months + monDays + d;

    // Add the decimal hours to return
    return (double)days - 730531.5 + (h / 24.);

}

//----------------------------------------------------------------------
// This is an example set
#ifdef TEST_DATE_TIME
struct tm {
    int tm_sec;   // seconds of minutes from 0 to 61
    int tm_min;   // minutes of hour from 0 to 59
    int tm_hour;  // hours of day from 0 to 24
    int tm_mday;  // day of month from 1 to 31
    int tm_mon;   // month of year from 0 to 11
    int tm_year;  // year since 1900
    int tm_wday;  // days since sunday
    int tm_yday;  // days since January 1st
    int tm_isdst; // hours of daylight savings time
}
int main() {
    // current date/time based on current system
    time_t now = time(0);

    // convert now to string form
    char* dt = ctime(&now);

    cout << "The local date and time is: " << dt << endl;

    // convert now to tm struct for UTC
    tm *gmtm = gmtime(&now);
    dt = asctime(gmtm);
    cout << "The UTC date and time is:" << dt << endl;
}
#endif
