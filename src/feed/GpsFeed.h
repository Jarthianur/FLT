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

#include <cstddef>
#include <memory>
#include <string>

#include "../Defines.h"
#include "../config/PropertyMap.h"
#include "parser/GpsParser.h"

#include "Feed.h"

/// @namespace data
namespace data
{
class GpsData;
} /* namespace data */

/// @namespace feed
namespace feed
{
/**
 * @class GpsFeed
 * @brief GPS input feed.
 * @extends Feed
 */
class GpsFeed : public Feed
{
public:
    NON_COPYABLE(GpsFeed)

    /**
     * @fn GpsFeed
     * @brief Constructor
     * @param crName   The unique name
     * @param crKvMap  The properties map
     * @param pData    The GpsData pointer
     * @param vGndMode The ground mode state
     * @throw std::logic_error from parent constructor
     */
    GpsFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
            std::shared_ptr<data::GpsData> pData, bool vGndMode);

    /**
     * @fn ~GpsFeed
     * @brief Destructor
     */
    virtual ~GpsFeed() noexcept;

    /**
     * @see Feed#process
     */
    void process(const std::string& crResponse) noexcept override;

private:
    /// @var mParser
    /// Parser to unpack response from Client
    parser::GpsParser mParser;

    /// @var mDataSlot
    /// Data attempt slot
    std::size_t mDataSlot;

    /// @var mpData
    /// GpsData to update for input
    std::shared_ptr<data::GpsData> mpData;

    /// @var mGndModeEnabled
    /// Ground mode state
    bool mGndModeEnabled;
};

}  // namespace feed
