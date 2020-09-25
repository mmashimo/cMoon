

#pragma once

#include <cmath>

enum class AltitudeType : int
{
	AtElevation,
	MoonObject,
	ActualSun,
	CivilSun,
	NauticalSun,
	AstronomicalSun
};

// Base object for all objects
class AObject
{
public:

	/// @brief Default constructor
	AObject(const char* name, const AltitudeType altType, bool brightObject);

	/// @brief Destructor
	~AObject();

	/// @brief Resets internal variables
	void resetSettings();

	/// @brief Prints Rise/Set times for objects
	void printRiseSetTimes();

	/// @brief Adjusts time hourly to check ends and resolved current
	void adjustForNext(double yPrior, double yCurr, double yNext, double hour);

	const char* m_Name;
	const char* m_strAbove;
	const char* m_strBelow;
	double      m_utRise;
	double      m_utSet;
	bool        m_rise;
	bool        m_sett;
	bool        m_above;

	double      m_sinHorizontal;

private:

	void initialize(const bool brightObject);

	void setAltitudeType(const AltitudeType type);

};



