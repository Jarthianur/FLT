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

#include "Feed.h"

namespace vfrb::feed::parser
{
class GpsParser;
}  // namespace vfrb::feed::parser

namespace vfrb::data
{
class GpsData;
}  // namespace vfrb::data

namespace vfrb::feed
{
/**
 * @brief Extend Feed for GPS input.
 */
class GpsFeed : public Feed
{
    NOT_COPYABLE(GpsFeed)

    static parser::GpsParser s_parser;  ///< Parser to unpack response from Client

public:
    /**
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The GpsData container
     * @throw std::logic_error from parent constructor
     */
    GpsFeed(str const& m_name, config::Properties const& m_properties, s_ptr<data::GpsData> data);
    ~GpsFeed() noexcept override = default;

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::GPS
     */
    Protocol protocol() const override;

    /**
     * @brief Implement Feed::process.
     */
    bool process(str const& response) override;
};
}  // namespace vfrb::feed
