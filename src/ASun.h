/// @file
///
/// @brief Computes sunrise and sunset.
///
/// NOTE: Need location information in decimal degrees:
/// lat, long
#pragma once

#include "pch.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>

#include "ADateTime.h"

#include "AlgBase.h"

class ASun : public AlgBase
{
public:
	ASun();

    /// @brief Sets print statement verbose mode
    /// @param[in] level - 0=quiet(results only) non-zero(prints debug info)
    void setVerboseMode(const int level);

	void showSun(const ADateTime& procTime);

	static int m_verboseLevel;

private:

};
