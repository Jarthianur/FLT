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
#include <memory>
#include <string>
#include <boost/asio/signal_set.hpp>

#include "../Defines.h"
#include "../config/PropertyMap.h"

/// @namespace feed
namespace feed
{
/// @namespace client
namespace client
{
class Client;
}  // namespace client

/**
 * @class Feed
 * @brief Base class representing an input feed.
 */
class Feed
{
public:
    NON_COPYABLE(Feed)

    /**
     * @fn ~Feed
     * @brief Destructor
     */
    virtual ~Feed() noexcept;

    /**
     * @fn run
     * @brief Run a Feed.
     * @param rSigset The signal set to pass to the Client
     */
    void run(boost::asio::signal_set& rSigset) noexcept;

    /**
     * @fn process
     * @brief Handle Client's response.
     * @param crResponse The response
     */
    virtual void process(const std::string& crResponse) noexcept = 0;

    /**
     * Define and declare getters.
     */
    GETTER_CR(std::string, mName, Name)
    GETTER_V(std::uint32_t, mPriority, Priority)

protected:
    /**
     * @fn Feed
     * @brief Constructor
     * @param crName  The Feeds unique name
     * @param crKvMap The properties map
     * @throw std::logic_error if host or port are not given
     */
    Feed(const std::string& crName, const config::KeyValueMap& crKvMap);

    /// @var mName
    /// Unique name
    const std::string mName;

    /// @var mKvMap
    /// Key-value-map holding the properties.
    const config::KeyValueMap mKvMap;

    /// @var mpClient
    /// Client, later resolved in child class
    std::unique_ptr<client::Client> mpClient;

private:
    /**
     * @fn initPriority
     * @brief Initialize the priority from the given properties.
     */
    void initPriority() noexcept;

    /// @var mPriority
    /// The priority
    std::uint32_t mPriority;
};

}  // namespace feed
