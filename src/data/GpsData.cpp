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

#include "data/GpsData.h"

#include <stdexcept>

/// @def GPS_NR_SATS_GOOD
/// Good number of satellites
#define GPS_NR_SATS_GOOD 7

/// @def GPS_FIX_GOOD
/// Good fix quality
#define GPS_FIX_GOOD 1

/// @def GPS_HOR_DILUTION_GOOD
/// Good horizontal dilution
#define GPS_HOR_DILUTION_GOOD 1.0

using namespace object;

namespace data
{
GpsData::GpsData() : Data() {}

GpsData::GpsData(const GpsPosition& position, bool ground)
    : Data(), m_position(position), m_groundMode(ground)
{
    m_processor.process(m_position);
}

void GpsData::get_serialized(std::string& dest)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_processor.process(m_position);
    dest += (++m_position).get_serialized();
}

bool GpsData::update(Object&& position)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_positionLocked)
    {
        throw PositionAlreadyLocked();
    }
    bool updated = m_position.tryUpdate(std::move(position));
    if (updated)
    {
        m_processor.process(m_position);
        if (m_groundMode && isPositionGood())
        {
            throw ReceivedGoodPosition();
        }
    }
    return updated;
}

Position GpsData::get_position()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_position.get_position();
}

bool GpsData::isPositionGood()
{
    return m_position.get_nrOfSatellites() >= GPS_NR_SATS_GOOD &&
           m_position.get_fixQuality() >= GPS_FIX_GOOD &&
           m_position.get_dilution() <= GPS_HOR_DILUTION_GOOD;
}

PositionAlreadyLocked::PositionAlreadyLocked() : GpsDataException() {}

const char* PositionAlreadyLocked::what() const noexcept
{
    return "position was locked before";
}

ReceivedGoodPosition::ReceivedGoodPosition() : GpsDataException() {}

const char* ReceivedGoodPosition::what() const noexcept
{
    return "received good position";
}

}  // namespace data
