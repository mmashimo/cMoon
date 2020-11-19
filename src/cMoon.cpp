/// @file
///
/// @brief This file is the main program entry for cMoon application.
///
/// @copyright 2019-2020 - M.Mashimo and all licensors. All rights reserved.
///
///  This program is free software: you can redistribute it and/or modify
///  it under the terms of the GNU General Public License as published by
///  the Free Software Foundation, either version 3 of the License, or
///  any later version.
///
///  This program is distributed in the hope that it will be useful,
///  but WITHOUT ANY WARRANTY; without even the implied warranty of
///  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///  GNU General Public License for more details.
///
///  You should have received a copy of the GNU General Public License
///  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Used for Visual Studio, does nothing for g++
#include "pch.h"

#include <iostream>
#include <cstdio>
#include <cstring>

#include <string>
#include <array>
#include <vector>
#include <ctime>

#include <sys/stat.h>

#include "cMoonConfig.h"

#include "AMoon.h"
#include "ASun.h"
#include "ALocation.h"
#include "APlanets.h"

#include "settings.hpp"

using namespace std;

// Header - if 0 - generic. If < 0, suppress
static int  s_HelpType = 0;
static bool s_processDate = true;

static bool s_computeSun = false;
static bool s_computeMoonPhase = false;
static bool s_computeMoonRise = false;
static bool s_computeNextMoon = false;
static bool s_computePlanets = false;

static int s_nextPhase = -1;
static int s_numberOfPhases = 4;
static int s_nextMoonCycle = 0;
static bool s_lockMoonPhase = false;

static int  s_planetType = -1; // Show all planets

// bit 0 - ADateTime
static constexpr unsigned dateTime{0x1};
// bit 1 - AMoon
static constexpr unsigned moon{0x2};
// bit 2 - ASun
static constexpr unsigned sun{0x4};
// bit 3 - APlanets
static constexpr unsigned planets{0x8};
// Set all bits
static constexpr unsigned all{0xF};

static void setDebugLevels(const unsigned setting, const int level)
{
	if (setting == all)
	{
		ADateTime::m_verboseLevel = level;
		AMoon::m_verboseLevel = level;
		ASun::m_verboseLevel = level;
		APlanets::m_verboseLevel = level;
		if (level == 0)
			std::cout << "Resetting All verbose modes to " << level << std::endl;
		else
			std::cout << "Setting All verbose modes to " << level << std::endl;
	}
	else
	{
		if (setting & dateTime)
		{
			ADateTime::m_verboseLevel = level;
			std::cout << "Setting DateTime verbose mode to " << level << std::endl;
		}
		if (setting & moon)
		{
			AMoon::m_verboseLevel = level;
			std::cout << "Setting Moon verbose mode to " << level << std::endl;
		}
		if (setting & sun)
		{
			ASun::m_verboseLevel = level;
			std::cout << "Setting Sun verbose mode to " << level << std::endl;
		}
		if (setting & planets)
		{
			APlanets::m_verboseLevel = level;
			std::cout << "Setting Planets verbose mode to " << level << std::endl;
		}
	}
}


void parseVerboseMode(char* arg)
{
	// It starts out '-v[...]' or '-q' all in one string
	char* options = arg + 1;
	int level = 1;

	// In "quiet mode", start with 0, then adds debug modes
	if ((*arg == 'q') || (*arg == 'Q'))
	{
		if (*options == '\0')
		{
			setDebugLevels(all, 0);
			return;
		}
		level = 0;
	}

	// Settings:
	unsigned setting = 0;

	if (*options == '\0')
	{
		// -v only - basic debug level
		level = 1;
		setting = all;
	}
	else if (isdigit(*options))
	{
		level = atoi(options);
		setting = all;
	}
	else
	{
		// -v#
		setting = 0;
		// Nothing gets set
		level = 0;

		bool levelQueued = false;
		bool settingQueued = false;

		while(*options != '\0')
		{
			if (isdigit(*options))
			{
				// Options is a number
				level = atoi(options);
				// If nothing set, set them all
				if (setting == 0)
				{
					setDebugLevels(all, level);
				}
				else
				{
					setDebugLevels(setting, level);

					// Make sure to clear bit field
					setting = 0;
				}
				levelQueued = false;
				settingQueued = false;
				options++;
				// Discard all other numbers
				while (isdigit(*options)) options++;
			}
			else
			{
				switch (options[0])
				{
				case 'a':
					// Set all objects
					setting = all;
					break;
				case 'd':
				case 't':	// Set date-time verbose mode`
					if (levelQueued && settingQueued)
					{
						setDebugLevels(setting, level);
						levelQueued = false;
						level = 0;
						setting = 0;
					}
					setting |= dateTime;
					settingQueued = true;
					break;
				case 'm':
					if (levelQueued && settingQueued)
					{
						setDebugLevels(setting, level);
						levelQueued = false;
						level = 0;
						setting = 0;
					}
					setting |= moon;
					settingQueued = true;
					break;
				case 's':
					if (levelQueued && settingQueued)
					{
						setDebugLevels(setting, level);
						levelQueued = false;
						level = 0;
						setting = 0;
					}
					setting |= sun;
					settingQueued = true;
					break;
				case 'p':
					if (levelQueued && settingQueued)
					{
						setDebugLevels(setting, level);
						levelQueued = false;
						level = 0;
						setting = 0;
					}
					setting |= planets;
					settingQueued = true;
					break;
				case 'v':
					setting = all;
					settingQueued = true;
				case 'A':	// Set all verbose mode levels
					level = DebugFullDateTime | DebugJulianDate | DebugComputation;
					levelQueued = true;
					break;
				case 'C':	// Sets the Computation Level
					level |= DebugComputation;
					levelQueued = true;
					break;
				case 'F':
					level |= DebugFullDateTime;
					levelQueued = true;
					break;
				case 'J':	// Show Julian dates in computation
					level |= DebugJulianDate;
					levelQueued = true;
					break;

				default:
					break;
				}
				options++;
			}
		}; // end while (options[0] != '\0')
	}

	setDebugLevels(setting, level);
}

static constexpr int helpQuiet{-1};
static constexpr int helpSuppressQuickHelp{1};
static constexpr int helpOptions{2};
static constexpr int helpDetails{4};
static constexpr int helpSuppressVersion{0x80};
static constexpr int versionOnly(0x100);


void printExtraHelp(const char* options)
{
#ifdef WIN32
	if ((_strcmp(options, "all") == 0)
#else
	if ((strcmp(options, "all") == 0) || (strncmp(options, "args", 3) == 0))
#endif
	{
		std::cout << "Use: cMoon [-<options>][now][yyyy-mm-dd][hh:mm:ss][JD.time][--latlong LAT LONG]" << std::endl;
		std::cout << "Options:" << std::endl;
		std::cout << "  [-h][-H][-?]         - Prints help and exits (-?? will show versions)" << std::endl;
		std::cout << "  [--help [details]]   - More detailed help ([details] yet-to-be build)" << std::endl;
		std::cout << "  [-mrsj]              - [m] Show moon-phase calculations" << std::endl;
		std::cout << "                         [r] Show moon-rise/sun-rise [r] calculations" << std::endl;
		std::cout << "                         [s] Show sunrise/set calculations" << std::endl;
		std::cout << "                         [j] Show Julian date, local sidereal calculations" << std::endl;
		std::cout << "  [-n[NXFN]][+-=[#]]   - Show next Moon phase: N=new, X=waxing, F=full, W=waning (default -nN)" << std::endl;
		std::cout << "                         [+[#]] - Next Moon cycle; [-[#]] - Previous Moon Cycle" << std::endl;
		std::cout << "                         [=[#]] - Shows phase set, [#] number of times (ex. -nF=4 shows next 4 Full Moons)" << std::endl;
		std::cout << "  [-p][#]              - Show planet calculations all or [#] M=Mercury, V=Venus, " << std::endl;
		std::cout << "                         R=Mars, J=Jupiter, S=Saturn, U=Uranus, N=Neptune, P=Pluto" << std::endl;
		std::cout << "  [-v[admps][ACFJ][#]] - Sets verbose mode (details in '--help -v')" << std::endl;
		std::cout << "  [-q]                 - Suppresses header" << std::endl;
		std::cout << "  [yyyy-mm-dd]         - Computes times using new date" << std::endl;
		std::cout << "  [hh:mm[:ss][ap][u]]  - Computes time assumes date set above [u=UTC]" << std::endl;
		std::cout << "  [JD.time]            - Computes time using Julian date and time (decimal needed)" << std::endl;
		std::cout << "  [--latlong LAT LONG] - sets the latitude and longitude for equations" << std::endl;
		std::cout << "  [--elev ELEVATION]   - sets the elevation for equations" << std::endl;
		std::cout << "  [--zone TIMEZONE]    - sets the timezone (float value - decimal not required)" << std::endl;
		std::cout << "  [--ini <ini_file>]   - Use configuration from <ini_file> (in/from executable directory)" << std::endl;
		std::cout << "  [--save[=<ini_file>]]- Save current configuration to INI or to <ini_file> (use '=' to set filename from exec-dir)" << std::endl;
	}
#ifdef WIN32
	else if ((_strcmp(options, "notes") == 0)
#else
	else if (strcmp(options, "notes") == 0)
#endif
	{
		std::cout << "NOTES:" << std::endl;
		std::cout << "- Moon phase, rise/set times and sun-rise/set times. Used for C++ modules." << std::endl;
		std::cout << "- Moon phase is computed from 29.53 day division and is a rough estimate." << std::endl;
		std::cout << "- Moon rise/set times use linear approximation is also rough estimate." << std::endl;
		std::cout << "- When adding date/time, make sure to set date first. The default time for a given date is noon" << std::endl;
		std::cout << "- Added Julian time assumes UTC at midnite. Make sure '.5' is added for noon" << std::endl;
	}
#ifdef WIN32
	else if (((_strnicmp(options, "-v", 2) == 0) || ((_strnicmp(options, "verbose", 4) == 0))
#else
	else if ((strncasecmp(options, "-v", 2) == 0) || (strncmp(options, "verb", 4) == 0))
#endif
	{
		std::cout << "Verbose mode options: -v[admpsv][ACFJ][#]" << std::endl;
		std::cout << "  [a,v][ACFJ#] - sets all modules to [ACFJ] (see below)" << std::endl;
		std::cout << "  [d,t][ACFJ#] - sets Date-Time module to [ACFJ] (see below)" << std::endl;
		std::cout << "  m[ACFJ]      - sets Moon module to [ACFJ] (see below)" << std::endl;
		std::cout << "  s[ACFJ]      - sets Sun module to [ACFJ] (see below)" << std::endl;
		std::cout << "  p[ACFJ]      - sets Planet module to [ACFJ] (see below)" << std::endl;
		std::cout << "  [A]          - Sets debug level to all levels" << std::endl;
		std::cout << "  [C]          - Sets debug level to show Computations" << std::endl;
		std::cout << "  [F]          - Sets debug level to show Full date/time for times" << std::endl;
		std::cout << "  [J]          - Sets debug level to show Julian Date values" << std::endl;
		std::cout << "  [#]          - Sets debug level as bit field: FullDateTime[F]=1 Julian[J]=2 Computation[C]=4" << std::endl;
	}
#ifdef WIN32
	else if (_strnicmp(options, "versions", 4) == 0)
#else
	else if (strncmp(options, "versions", 4) == 0)
#endif
	{
		std::cout << " Version 0.4 - INI file, GPL" << std::endl;
		std::cout << " - Command line improvements (-p, -n)" << std::endl;
		std::cout << " Version 0.3 - INI file, GPL" << std::endl;
		std::cout << " - Added INI file support" << std::endl;
		std::cout << " Version 0.2 - Planet version" << std::endl;
		std::cout << " - Added RA/DEC of Planets to 0.1-arc-minute accuracy" << std::endl;
		std::cout << std::endl;
		std::cout << " Version 0.1 - Initial version" << std::endl;
		std::cout << " - Moon percentage, days to new" << std::endl;
		std::cout << " - Next Moon phase (by phases)" << std::endl;
		std::cout << " - Moonrise/set, sunrise, sunset times" << std::endl;
		std::cout << " - Verbose modes, for date/time computation, moon and sun derivatives" << std::endl;
		std::cout << " - Input options for date/time/Julian, long-lat and verbose mode" << std::endl;
		std::cout << std::endl;
	}
	else
	{
		std::cout << "Help content: '" << options << "' not found." << std::endl;
		std::cout << "Available:" << std::endl;
		std::cout << "  'all' or 'args'   - prints command line arguments options" << std::endl;
		std::cout << "  'notes'           - app notes" << std::endl;
		std::cout << "  'verbose' or '-v' - verbose mode" << std::endl;
		std::cout << "  'version'         - past versions" << std::endl;
	}
	
}


void print_header(int details)
{
	// Options:
	// -1 - quiet mode (exits)
	//  0 - normal mode: prints version header, usage help.
	//  Bit fields
	//  1 - option help: prints version header, option help (bit field)
	//  2 - version help: prints version header, versions including bug fixes
	//  0x80 - suppress version header
	//  0x40 - suppress 

	if (details < 0)
	{
		// Quiet mode
		return;
	}

	if (!(details & helpSuppressVersion))
	{
		// Version header
		std::cout << "cMoon - Version " << cMoon_VERSION_MAJOR << "." << cMoon_VERSION_MINOR << std::endl;
	}

	std::cout << std::endl;

	if (details & helpOptions)
	{
		// Option help
		printExtraHelp("all");
		std::cout << std::endl;
		printExtraHelp("notes");
		details |= helpSuppressQuickHelp; 
	}

	if (details & helpDetails)
	{
		printExtraHelp("versions");
		details |= helpSuppressQuickHelp; 
	}

	if (!(details & helpSuppressQuickHelp))
	{
		std::cout << "Use: 'cMoon -h' or 'cMoon --help' to get help" << std::endl;
		std::cout << std::endl;
	}

}



int main(int argc, char** argv)
{
	ADateTime dateObj;
	AMoon moonObj;
	ASun  sunObj;
	APlanets planets;

	// Get default INI configuration
	Settings settings(true);

	ALocation location(settings.getLocation());

	bool bProcess = true;

	if (argc > 1)
	{
		// Parse arguments
		static char s_tmp[1024];
		bool bGoodDate = true;

		std::string iniFile;

		for (int i = 1; i < argc; i++)
		{
			char* ar = argv[i];
			size_t len = strlen(ar);
			strcpy(s_tmp, ar);
			bGoodDate = false;
			char* options = s_tmp;

			if (len > 0)
			{
				if (ar[0] == '-')
				{
					// Special options
					options = ar + 1;

					// Parse options
					if (len >= 2)
					{
						// Parse for "--<options>"
						if (options[0] == '-')
						{
							options++;
							// Process options
							// NOTE: decrecatedbut POSIX uses 'case'
	#ifdef WIN32
							if (_strnicmp(options, "help", 4) == 0)
	#else
							if (strncasecmp(options, "help", 4) == 0)
	#endif
							{
								// Look for next argument that is not zero
								if (((i + 2) <= argc) && !isdigit(argv[i + 1][0]))
								{
									// Get help on subject matter
									print_header(1);
									printExtraHelp(argv[i+1]);
									// Suppress header (it's printed above)
									s_HelpType = -1;
									i++;
								}
								else
								{
									s_HelpType |= helpOptions;
								}
								bProcess = false;
							}
	#ifdef WIN32
							else if (_strnicmp(options, "latlong", 8) == 0)
	#else
							else if (strncasecmp(options, "latlong", 8) == 0)
	#endif
							{
								if ((i + 3) <= argc)
								{
									// Set the latitude
									location.setLatitude( atof(argv[i + 1]) );
									location.setLongitude( atof(argv[i + 2]) );
									std::cout << "Setting Lat-Long: " << location.latitude() << " - " << location.longitude() << std::endl;
									i += 2;
								}
								else
								{
									std::cout << "Cannot set Lat-Long: Argument count " << argc << " is not " << i + 3 << std::endl;
								}
							}
	#ifdef WIN32
							else if (_strnicmp(options, "zone", 8) == 0)
	#else
							else if (strncasecmp(options, "zone", 4) == 0)
	#endif
							{
								if ((i + 2) <= argc)
								{
									// Set the latitude
									ADateTime::m_defaultTimeZone = atof(argv[i + 1]);
									std::cout << "Setting Timezone: " << ADateTime::m_defaultTimeZone << std::endl;
									i += 1;
								}
								else
								{
									std::cout << "Cannot set Timezone: Argument count " << argc << " is not " << i + 2 << std::endl;
								}
							}
	#ifdef WIN32
							else if (_strnicmp(options, "elevation", 4) == 0)
	#else
							else if (strncasecmp(options, "elevation", 4) == 0)
	#endif
							{
								if ((i + 2) <= argc)
								{
									// Set the latitude
									location.setElevation(atof(argv[i + 1]));
									std::cout << "Setting Elevation (in feet): " << location.elevation() << std::endl;
									i += 1;
								}
								else
								{
									std::cout << "Cannot set Eelvation: Argument count " << argc << " is not " << i + 2 << std::endl;
								}
							}
	#ifdef WIN32
							else if (_strnicmp(options, "ini", 3) == 0)
	#else
							else if (strncasecmp(options, "ini", 3) == 0)
	#endif
							{
								iniFile = argv[i + 1];
								// Get configuration from INI file other than "../cMoon.ini"
								bProcess = settings.setDefaultIniFile(iniFile);
							}
	#ifdef WIN32
							else if (_strnicmp(options, "save", 4) == 0)
	#else
							else if (strncasecmp(options, "save", 4) == 0)
	#endif
							{
								// Save configuration (set before arg) to INI file
								// Look for "--save=<ini_file> to save other than "cMoon.ini"
								if (options[4] == '=')
								{
									iniFile = options + 5;
									bProcess = settings.setDefaultIniFile(iniFile, true);
								}
								settings.save_location_info(location);
								settings.save_date_time_info();
								settings.save_debug_info();
							}
							else
							{
								std::cout << "Don't know how to resolve '" << ar << "' - use '--help all'" << std::endl;
							}
						}
						else
						{
							// Parse for "-<options>"
							switch (options[0])
							{
							case 'h':
							case 'H':
							case '?':
								// If ? or ^ as option, we don't process. '?' for help
								s_HelpType |= helpOptions;
								if (options[1] == '?')
								{
									s_HelpType |= helpDetails;
								}
								bProcess = false;
								break;

							case 'm':
							case 'M':
								s_computeMoonPhase = true;
								break;
							case 'n':
							case 'N':	// Next phase of number
								{
									bool breakLoop = false;
									int cycleIndex = 0;

									// Option 'n' ends without any arguments. Do Moon Phase to compute current phase
									// then compute NextPhase (TODO: only rather than all)
									s_computeMoonPhase = true;

									options++;

									while(!breakLoop && (*options != '\0'))
									{
										if (isdigit(*options))
										{
											int cycles = atoi(options);
											switch(cycleIndex)
											{
												default:
												case 0: // Number of phases
													s_numberOfPhases = (cycles <= 0) ? 4 : cycles;
													break;
												case 1: // Add to next cycle
													s_nextMoonCycle += cycles;
													break;
												case 2: // Subtract from this cycle
													s_nextMoonCycle -= cycles;
													break;
												case 3: // Update next phase
													s_nextMoonCycle = cycles;
													break;
											}
											// Advance until no longer a digit
											while(isdigit(*(options+1)))
												options++;
										}
										else
										{
											switch(options[0])
											{
												case '*':  // Print all - can be added anywhere
													cycleIndex = 0;
													break;
												case '+':  // forward moon cycles
													cycleIndex = 1;
													break;
												case '-':  // backward moon cycles
													cycleIndex = 2;
													break;
												case '#':  // Prints number of cycles = default 4 phases
													cycleIndex = 3;
													break;
												case '=':  // Lock the current phase (i nothing behind, next phase)
													s_lockMoonPhase = true;
													break;
												case 'f':
												case 'F':  // show next _F_ull moon
													s_nextPhase = 2;
													break;
												case 'n':
												case 'N':  // show next _N_ew moon
													s_nextPhase = 0;
													break;
												case 'w':  // show _W_aning quarter moon
												case 'W':
													s_nextPhase = 3;
													break;
												case 'x':
												case 'X':  // show Wa_x_ing quarter moon
													s_nextPhase = 1;
													break;
											}
										}
										++options;
									}
								}
								s_computeNextMoon = true;
								break;
								
							case 'r':
							case 'R':
								s_computeMoonRise = true;
								break;
							case 's':
							case 'S':
								s_computeSun = true;
								break;
							case 'p':
							case 'P':
								s_computePlanets = true;

								options++;

								if (*options == '\0')
								{
									// Get next arg
									ar = argv[i + 1];
									if ((ar != nullptr) && (ar[0] != '-'))
									{
										s_planetType = atoi(ar);
										i++;
									}
								}
								else if (isdigit(*options))
								{
									// Use index to planets
									s_planetType = atoi(options);
								}
								else
								{
									switch(*options)
									{
										default:
										case 'a':  // All planets 
										case 'A':
										case '*':
											s_planetType = -1;
											break;
										case 'j':
										case 'J':
											s_planetType = 4;
											break;
										case 'm':  // Could be Mercury or Mars. If 'M' alone, Mercury
										case 'M':
											if ((options[1] == '\0') || (options[1] == 'e') || (options[1] == 'E'))
											{
												s_planetType = 0;   // "M", "Me", "ME"
											}
											else
											{	// Any extension assume Mars
												s_planetType = 3;   // "Ma" or "MA"
											}
											break;
										case 'n':  // Neptune
										case 'N':
											s_planetType = 7;
											break;
										case 'p':  // Pluto
										case 'P':
											s_planetType = 8;
											break;
										case 'r':  // Mars if no "M"
										case 'R':
											s_planetType = 3;
											break;
										case 's':  // Saturrn
										case 'S':
											s_planetType = 5;
											break;
										case 'u':  // Uranus
										case 'U':
											s_planetType = 6;
											break;
										case 'v':  // Venus
										case 'V':
											s_planetType = 1;
											break;

									}
								}
								break;

							case 'Q':
							case 'q':
								// Quiets Verbose mode - NOTE '-q' suppresses, except # is referenced later
								parseVerboseMode(options);
								s_HelpType = helpQuiet;
								break;

							case 'v': // Verbal level (format: '-v#')
							case 'V':
								parseVerboseMode(options);
								break;
							}
						}
						bGoodDate = false;
					}
					else
					{
						std::cout << "!!! Cannot process '" << ar << "' arguments without the options" << std::endl;
						s_HelpType = helpOptions;
						bProcess = false;
					}
					bGoodDate = false;
				}
	#ifdef WIN32
				else if (_stricmp(ar, "now") == 0)
	#else
				else if (strcasecmp(options, "now") == 0)
	#endif
				{
					bGoodDate = true;
				}
				else if (isdigit(ar[0]))
				{
					// Date or time entry
					if (nullptr != strchr(ar, '-'))
					{
						ParsedDate tempDate;
						bGoodDate = dateObj.parseDate(ar, tempDate);
						if (!bGoodDate)
						{
							std::cout << "!!! Invalid date string: '" << ar << " <= found '-' but date not in YYYY-MM-DD format" << std::endl;
						}
					}
					else if (nullptr != strchr(ar, ':'))
					{
						bGoodDate = dateObj.parseTime(ar);
						if (!bGoodDate)
						{
							std::cout << "!!! Invalid time string: '" << ar << " <= found ':' but time not in HH:MM[:SS] format" << std::endl;
						}
					}
					else if ((nullptr != strchr(ar, '.')) || (isdigit(*ar)))
					{
						// Compute using Julian date/time
						bGoodDate = dateObj.parseJulianTime(ar);
						if (!bGoodDate)
						{
							std::cout << "!!! Invalid Julian decimal format: '" << ar << " <= found '.' but date not in J.d format" << std::endl;
						}
					}
					else
					{
						// Parse time by number of seconds from midnight
						bGoodDate = dateObj.parseTime(ar);
						if (!bGoodDate)
						{
							std::cout << "!!! Invalid time string: '" << ar << " <= trying number of seconds from midnight" << std::endl;
						}
					}
				}
	#ifdef WIN32
				else if (_stricmp(ar, "now") == 0)
	#else
				else if (strcasecmp(options, "now") == 0)
	#endif
				{
					bGoodDate = true;
				}
				else
				{
					std::cout << "!!! Invalid argument string: '" << ar << "'" << std::endl;
					s_HelpType = 1;
				}
			}
			else
			{
				std::cout << "Empty argument line at " << i << " - how did you do that?" << std::endl;
				s_HelpType = 1;
			}
		}  // end for (arg) loop

		if (s_HelpType == 0)
		{
			s_HelpType = versionOnly;
		}
	}  // end if arguments


	print_header(s_HelpType);

	std::cout << std::endl;

	if (bProcess && dateObj.isParsedCorrectly())
	{
		if (!s_computeSun && !s_computeMoonPhase && !s_computeMoonRise && !s_computeNextMoon && !s_computePlanets)
		{
			s_computeSun = true;
			s_computeMoonPhase = true;
			s_computeMoonRise = true;
			s_computeNextMoon = true;
			s_computePlanets = true;
		}

		dateObj.printASCII();

		if (s_computeMoonPhase)
		{
			int nextPhase = moonObj.moonPhase(dateObj);
			if (s_nextPhase < 0)
			{
				s_nextPhase = nextPhase;
			}
		}

		if (s_computeMoonRise)
		{
			moonObj.moonRise(location, dateObj);
		}

		if (s_computeNextMoon)
		{
			moonObj.nextMoonPhase(dateObj, s_nextPhase, s_lockMoonPhase, s_numberOfPhases, s_nextMoonCycle);
		}

		if (s_computeSun)
		{
			sunObj.showSun(location, dateObj);
		}

		if (s_computePlanets)
		{
			planets.computePlanets(location, dateObj, s_planetType);
		}
	}

}

