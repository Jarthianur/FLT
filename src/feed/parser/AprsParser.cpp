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

#include "AprsParser.h"

#include <limits>
#include <stdexcept>

#include "../../Math.hpp"
#include "../../data/object/Position.h"

/// Define regex match groups for APRS
//#define RE_APRS_TIME 1
#define RE_APRS_LAT 1
#define RE_APRS_LAT_DIR 2
#define RE_APRS_LONG 3
#define RE_APRS_LONG_DIR 4
#define RE_APRS_HEAD 5
#define RE_APRS_GND_SPD 6
#define RE_APRS_ALT 7
#define RE_APRS_COM 8
#define RE_APRS_COM_TYPE 1
#define RE_APRS_COM_ID 2
#define RE_APRS_COM_CR 3
#define RE_APRS_COM_TR 4

namespace feed
{
using namespace data::object;

namespace parser
{
const boost::regex AprsParser::msAprsRe(
    //"^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/(\\d{6})h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
    "^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/\\d{6}h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
    boost::regex::optimize | boost::regex::icase);

const boost::regex AprsParser::msAprsComRe(
    "^(?:[\\S\\s]+?)?id([0-9A-F]{2})([0-9A-F]{6})\\s?(?:([\\+-]\\d{3})fpm\\s+?)?(?:([\\+-]\\d+?\\.\\d+?)rot)?(?:[\\S\\s]+?)?$",
    boost::regex::optimize | boost::regex::icase);

AprsParser::AprsParser() : AprsParser(std::numeric_limits<std::int32_t>::max())
{}

AprsParser::AprsParser(std::int32_t vMaxHeight)
    : Parser<data::object::Aircraft>(), mMaxHeight(vMaxHeight)
{}

AprsParser::~AprsParser() noexcept
{}

bool AprsParser::unpack(const std::string& crStr, Aircraft& rAircraft) noexcept
{
    boost::smatch match, com_match;

    if((!crStr.empty() && crStr.front() == '#')
       || !(boost::regex_match(crStr, match, msAprsRe)
            && parsePosition(match, rAircraft)))
    {
        return false;
    }
    std::string comm(match.str(RE_APRS_COM));

    if(!(boost::regex_match(comm, com_match, msAprsComRe)
         && parseComment(com_match, rAircraft)))
    {
        return false;
    }
    rAircraft.setFullInfoAvailable(parseMovement(match, com_match, rAircraft));
    rAircraft.setTargetType(Aircraft::TargetType::FLARM);
    return true;
}

bool AprsParser::parsePosition(const boost::smatch& crMatch,
                               data::object::Aircraft& rAircraft)
{
    bool valid = false;
    try
    {
        Position pos;
        pos.latitude = math::dmToDeg(std::stod(crMatch.str(RE_APRS_LAT)));
        if(crMatch.str(RE_APRS_LAT_DIR).compare("S") == 0)
        {
            pos.latitude = -pos.latitude;
        }
        pos.longitude = math::dmToDeg(std::stod(crMatch.str(RE_APRS_LONG)));
        if(crMatch.str(RE_APRS_LONG_DIR).compare("W") == 0)
        {
            pos.longitude = -pos.longitude;
        }
        pos.altitude
            = math::doubleToInt(std::stod(crMatch.str(RE_APRS_ALT)) * math::FEET_2_M);

        rAircraft.setPosition(pos);
        valid = pos.altitude <= mMaxHeight;
    }
    catch(const std::logic_error&)
    {
    }
    return valid;
}

bool AprsParser::parseComment(const boost::smatch& crMatch,
                              data::object::Aircraft& rAircraft)
{
    bool valid = true;
    rAircraft.setId(crMatch.str(RE_APRS_COM_ID));
    try
    {
        rAircraft.setIdType(static_cast<Aircraft::IdType>(
            std::stoi(crMatch.str(RE_APRS_COM_TYPE), nullptr, 16) & 0x03));
        rAircraft.setAircraftType(static_cast<Aircraft::AircraftType>(
            (std::stoi(crMatch.str(RE_APRS_COM_TYPE), nullptr, 16) & 0x7C) >> 2));
    }
    catch(const std::logic_error&)
    {
        valid = false;
    }
    return valid;
}

bool AprsParser::parseMovement(const boost::smatch& crMatch,
                               const boost::smatch& crCommMatch,
                               data::object::Aircraft& rAircraft)
{
    Movement move;
    bool valid = true;
    // This needs to be split later to parse independently.
    try
    {
        move.heading   = std::stod(crMatch.str(RE_APRS_HEAD));
        move.gndSpeed  = std::stod(crMatch.str(RE_APRS_GND_SPD)) * math::KTS_2_MS;
        move.climbRate = std::stod(crCommMatch.str(RE_APRS_COM_CR)) * math::FPM_2_MS;
    }
    catch(const std::logic_error&)
    {
        valid = false;
    }
    rAircraft.setMovement(move);
    return valid;
}
}
}  // namespace parser
