/// @file
///
/// @brief ADateTime class definitions.
///
/// ADateTime computes date and time for floating-point Julian dates.
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
#include <array>
#include <ctime>

using ParsedDate = std::array<int, 3>;
using DateString = std::string;

typedef struct _parsedTime {
	std::array<int, 3> m_Time;   // Hours[0], minutes[1], seconds[2]
	bool               m_24hr;
	bool               m_amPm;   // am=false, pm=true (regardless of 24hr time)
    bool               m_utc;    // true if UTC time is set (current)
} ParsedTime;

/// @brief Constants used for debug levels (m_verboseLevel) - bit fields (except <0)
constexpr int DebugFullDateTime{1};
constexpr int DebugJulianDate{2};
constexpr int DebugComputation{4};

class ADateTime
{
public:

    /// @brief Constructor to get current date/time
    ADateTime();

    /// @brief Copy Contructor
    ADateTime(const ADateTime& ref);

    /// @brief Destructor
    virtual ~ADateTime();

    /// @brief Assignment operator
    ADateTime& operator=(const ADateTime& ref);

    /// @brief Parses short date format: YYYY[0]-MM[1]-DD[2].
    /// NOTE: Does rudimentary date check, but nothing else
    ///
    /// @param[in] arg - string arument hopefull hte format YYYY-MM-DD
    ///
    /// @return bool - true if parsing was good (list is populated)
    bool parseDate(const DateString& arg);

	/// @brief Parses short time format: HH:MM:SS[a,p]
	///
	/// @param[in] arg = time string
	///
	/// @return true if time can be converted, false string format is bad
	bool parseTime(const std::string& arg);

    bool parseJulianTime(const std::string& arg);

    /// @brief Populates today(now) date and time (time exists)
    ///
    ///@param[in] bUTC - set true if the date/time is to be in UTC
    void todaysDate(const bool bUTC = false);

    /// @brief Converts to Julian Years (in double format)
    double getJulianYear() const;

    /// @brief Converts Julian decimal into time struct
    void convertJulianToTime(const double jd, struct tm& timeOnly);

    /// @brief Sets Date struct from Julian calendar
    void setFromJulian(const double& jd);

    /// Basic Uses strftime()
    std::string asString(const struct tm& tmStruct, const std::string& format) const;

    /// @brief Uses strftime() to get time string
    std::string asString(const std::string& format) const;

    void printASCII();

    /// @brief Sets print statement verbose mode
    /// @param[in] level - 0=quiet(results only) non-zero(prints debug info)
    void setVerboseMode(const int level);

    //--- Accessors ---
    int year() const;

    int month() const;

    int day() const;

    int dayOfYear() const;

    void setJulianDateTime(double jd);

    /// @brief Sets Year, Month, Day - returns remaining Time as double
    static double getJulianDate(const double jd, int& year, int& mon, int& day);

    /// @brief Gets hour:min:sec.sss for a given Julian day - return double seconds
    static double getJulianTime(const double jd, int& hour, int& min, int& sec);

    /// @brief Returns Julian date/time for a given UTC hour adjusted for local time-zone.
    ///
    ///  Returns modified julian date number of days since 1858 Nov 17 00:00h (UTC midnight)
    ///  valid for any date since 4713 BC assumes gregorian calendar after 1582 Oct 15,
    ///  Julian before Years BC assumed in calendar format, i.e. the year before 1 AD is 1 BC
    ///
    ///  NOTE: The date (set in instance) is assumed to be local date, this will return time at UTC
    ///
    /// @param[in] addTimeZone - add timezone variance to UTC midnite
    ///
    /// @return Julian date/time (described above) of UTC
    double modifiedJuiianDate(const bool addTimeZone = true);

    /// @brief Computes J2000 time given y,m,d,h
    /// @param[in] utcMidDay - true if conversion uses only mid-day date
    /// @return J2000 Julian date as double
    double computeJ2000(const bool utcMidDay = true);

	/// @brief Verbose level of 0 - is quiet mode
	static int    m_verboseLevel;

    /// @brief Default time zone if time-zone is not computed directly
    static double m_defaultTimeZone;

	//--- Date Members ---
    /// @brief default use of time-zone computation
    static bool m_autoComputeTimeZone;

	/// @brief Parsed correctly and needs to nodification
	bool       m_parsedCorrectly;

    /// @brief Julian Year Calndar - Full date format
	double     m_julian;

    /// @brief Computed, if not automatically, from longitude and struct tm
    double     m_timeZone;

	/// @brief an array containing the date-format used for most ephemis use.
	ParsedDate m_parsedDate;

	/// @brief Time component
	ParsedTime m_parsedTime;

	/// @brief Time struct for UTC time
	struct tm m_timeStruct;

	/// @brief Time struct used to keep track of local time, if needed
	struct tm m_localTimeStruct;

	/// @brief Time in integer format
	time_t    m_rawTime;

    /// @brief - Check parsed date
    /// @param[in] parsed - date array
    /// @return - true if correct, false if something went wrong
    bool isParsedDateOk(const ParsedDate& parsed) const;

    /// @brief Converts struct tm to Parsed arrays
    /// @param[in] timeStruct - time struct
    /// @param[in] utc - true if time in struct tm is in UTC already
    /// @param[in] hr24 - true if 24hr time array is needed
    void constructDateTimeArray(const struct tm& timeStruct, const bool utc, const bool hr24 = true);

    /// @brief Converts ParsedDate to struct tm
    void convertDateFromArray(bool doTime = false);

private:
    /// @brief Copies internals for copy constructor and assignment operator
    void copyHelper(const ADateTime& ref);

    /// @brief Computes DST and time zone from longitude (if automatic)
    void setTimeZone();

};

