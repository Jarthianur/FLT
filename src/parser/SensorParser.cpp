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

#include "SensorParser.h"

#include <stdexcept>

#include "../data/SensorData.h"
#include "../util/Math.hpp"
#include "../vfrb/VFRB.h"

SensorParser::SensorParser()
        : Parser()
{
}

SensorParser::~SensorParser() noexcept
{
}

std::int32_t SensorParser::unpack(const std::string& msg, Priority prio) noexcept
{
    try
    {
        std::int32_t csum = std::stoi(msg.substr(msg.rfind('*') + 1, 2), nullptr, 16);
        if (csum != Math::checksum(msg.c_str(), msg.length()))
        {
            return MSG_UNPACK_IGN;
        }
    }
    catch (const std::logic_error& e)
    {
        return MSG_UNPACK_ERR;
    }

    if (msg.find("MDA") != std::string::npos)
    {
        try
        {
            mtB = msg.find('B') - 1;
            mtS = msg.substr(0, mtB).find_last_of(',') + 1;
            mtSubLen = mtB - mtS;
            mtPress = std::stod(msg.substr(mtS, mtSubLen), &mtNumIdx) * 1000.0;
            if (mtNumIdx == mtSubLen)
            {
                VFRB::msSensorData.setPress(prio, mtPress);
            }
            else
            {
                return MSG_UNPACK_ERR;
            }
        }
        catch (std::logic_error& e)
        {
            return MSG_UNPACK_ERR;
        }
    }
    else if (msg.find("MWV") != std::string::npos)
    {
        VFRB::msSensorData.setWVstr(prio, msg);
    }
    else
    {
        return MSG_UNPACK_IGN;
    }
    return MSG_UNPACK_SUC;
}