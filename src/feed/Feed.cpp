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

#include "Feed.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <unordered_map>

#include "../Logger.hpp"
#include "../config/Configuration.h"
#include "../data/Data.h"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(Feed)"

namespace feed
{
Feed::Feed(const std::string& crName, const config::KeyValueMap& crKvMap,
           std::shared_ptr<data::Data> pData)
    : mName(crName), mKvMap(crKvMap), mpData(pData)
{
    initPriority();
    if(mKvMap.find(KV_KEY_HOST) == mKvMap.end())
    {
        logger.warn(COMPONENT " could not find: ", mName, "." KV_KEY_HOST);
        throw std::logic_error("No host given");
    }
    if(mKvMap.find(KV_KEY_PORT) == mKvMap.end())
    {
        logger.warn(COMPONENT " could not find: ", mName, "." KV_KEY_PORT);
        throw std::logic_error("No port given");
    }
}

Feed::~Feed() noexcept
{}

void Feed::initPriority() noexcept
{
    try
    {
        mPriority = static_cast<std::uint32_t>(std::max<std::uint64_t>(
            0, std::min<std::uint64_t>(std::stoul(mKvMap.at(KV_KEY_PRIORITY)),
                                       std::numeric_limits<std::uint32_t>::max())));
    }
    catch(const std::logic_error&)
    {
        logger.warn(COMPONENT " create ", mName, ": Invalid priority given.");
    }
    if(mPriority == 0)
    {
        logger.warn(COMPONENT " create ", mName,
                    ": Priority is 0; this feed cannot update higher ones.");
    }
}

client::Endpoint Feed::getEndpoint() const
{
    return {mKvMap.find(KV_KEY_HOST)->second, mKvMap.find(KV_KEY_PORT)->second};
}

}  // namespace feed
