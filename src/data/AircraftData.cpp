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

#include "data/AircraftData.h"

#include <functional>
#include <iterator>
#include <stdexcept>
#include <string>

using namespace object;

namespace data
{
AircraftData::AircraftData() : AircraftData(0) {}

AircraftData::AircraftData(std::int32_t maxDist) : Data(), m_processor(maxDist) {}

bool AircraftData::update(Object&& aircraft)
{
    Aircraft&& update = static_cast<Aircraft&&>(aircraft);
    auto       result = m_container.insert(std::hash<std::string>()(*update.id()),
                                     std::move(update));  // TODO: provide char* based hashing
    if (!result.second)
    {
        result.first->value.tryUpdate(std::move(update));
    }
    return true;
}

void AircraftData::environment(const Location& position, double atmPress)
{
    m_processor.referTo(position, atmPress);
}

void AircraftData::access(const accessor_fn& func)
{
    auto iter = m_container.begin();
    while (iter != m_container.end())
    {
        ++(iter->value);
        try
        {
            if (iter->value.updateAge() == NO_FLARM_THRESHOLD)
            {
                iter->value.targetType(Aircraft::TargetType::TRANSPONDER);
            }
            if (iter->value.updateAge() >= DELETE_THRESHOLD)
            {
                auto key = iter.key();
                ++iter;
                m_container.erase(key);
            }
            else
            {
                m_processor.process(iter->value);
                func(iter->value);
                ++iter;
            }
        }
        catch (const std::exception&)
        {}
    }
}

}  // namespace data