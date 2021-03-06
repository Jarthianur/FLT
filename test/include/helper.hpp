/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
 A detailed list of copyright holders can be found in the file "AUTHORS".

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License version 3
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 }
 */

#pragma once

#include <cstdint>
#include <iostream>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>

#include "object/Aircraft.h"
#include "util/utility.hpp"

#include "sctf.hpp"

#define TEST_FUNCTION(NAME) extern void NAME(test::TestSuitesRunner&);

#define syso(M) std::cout << M << std::endl

#define assertEqStr(V, E) assertT(V, EQUALS, E, std::string)

namespace sctf
{
namespace util
{
template<>
inline std::string
    serialize<object::Aircraft::TargetType>(const object::Aircraft::TargetType& crTargetType)
{
    return std::to_string(::util::raw_type(crTargetType));
}
}  // namespace util
}  // namespace sctf

namespace helper
{
static boost::regex pflauRe("\\$PFLAU,,,,1,0,([-]?\\d+?),0,(\\d+?),(\\d+?),(\\S{6})\\*(?:\\S{2})");
static boost::regex pflaaRe(
    "\\$PFLAA,0,([-]?\\d+?),([-]?\\d+?),([-]?\\d+?),(\\d+?),(\\S{6}),(\\d{3})?,,(\\d+?)?,([-]?\\d+?\\.\\d+?)?,([0-9A-F]{1,2})\\*(?:\\S{2})");
static boost::regex gpsRe(
    "(\\$GPRMC,\\d{6},A,0000\\.00,N,00000\\.00,E,0,0,\\d{6},001\\.0,W\\*[0-9a-fA-F]{2}\\s*)?(\\$GPGGA,\\d{6},0000\\.0000,N,00000\\.0000,E,1,00,1,0,M,0\\.0,M,,\\*[0-9a-fA-F]{2}\\s*)?");

static std::string timePlus(std::int32_t val)
{
    return boost::posix_time::to_iso_string(
        boost::posix_time::time_duration(
            boost::posix_time::second_clock::local_time().time_of_day()) +
        boost::posix_time::seconds(val));
}
}  // namespace helper
