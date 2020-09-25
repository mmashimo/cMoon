
#include "AlgBase.h"
#include "ADateTime.h"

typedef struct structPlanet
{
	const char* planetName;
	double inclination;   // * i - inclination of plane of orbit
	double ascendingNode; // * o - longitude of ascending node at date
	double perihelion;    // * p - longitude of perihelion at date
	double semiMajorAxis; //   a
	double dailyMotion;   // * n - daily motion
	double eccentricity;  //   e - oblique eccentricity
	double meanLongitude; // * l - mean longitude at date (J2000 meredian)
} Planets;


class APlanets : public AlgBase
{
public:
	APlanets();

	// APlanets(const DateString& dateString);

	// APlanets(const APlanets& ref);

	virtual ~APlanets();

	// APlanets& operator=(const APlanets& ref);

	void computePlanets(const ADateTime& procTime, int type = -1);

	void computePlanetPos(const int type, const double j2000, double& ra, double& dec, double& dist);

	static int m_verboseLevel;

private:
	/// @brief Computes a planet's RA/DEC/Alt
	void computeAPlanet(const double j2000, const double md, int type);

	/// @brief Heliocentric Rectangular Coordinates of Earth (x = 0 is at vernal equinox)
	double m_xEarth;
	double m_yEarth;
	double m_zEarth;

};

