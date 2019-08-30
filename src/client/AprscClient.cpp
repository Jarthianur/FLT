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

#include "client/AprscClient.h"

#include <stdexcept>
#include <utility>

#include <boost/functional/hash.hpp>

#include "util/Logger.hpp"

using namespace vfrb::client::net;

namespace vfrb::client
{
constexpr auto     LOG_PREFIX = "(AprscClient) ";
static auto const& logger     = Logger::instance();

AprscClient::AprscClient(Endpoint const& endpoint, str const& login, s_ptr<Connector> connector)
    : Client(endpoint, connector), m_login(login + "\r\n")
{}

bool AprscClient::equals(Client const& other) const
{
    try
    {
        auto const& derived = dynamic_cast<AprscClient const&>(other);
        return Client::equals(other) && this->m_login == derived.m_login;
    }
    catch (std::bad_cast const&)
    {
        return false;
    }
}

usize AprscClient::hash() const
{
    usize seed = Client::hash();
    boost::hash_combine(seed, boost::hash_value(m_login));
    return seed;
}

void AprscClient::handleConnect(bool error)
{
    if (!error)
    {
        std::lock_guard lk(m_mutex);
        m_connector->onWrite(m_login, std::bind(&AprscClient::handleLogin, this, std::placeholders::_1));
        sendKeepAlive();
    }
    else
    {
        logger.warn(LOG_PREFIX, "failed to connect to ", m_endpoint.host, ":", m_endpoint.port);
        reconnect();
    }
}

void AprscClient::sendKeepAlive()
{
    m_connector->onTimeout(std::bind(&AprscClient::handleSendKeepAlive, this, std::placeholders::_1), 600);
}

void AprscClient::handleLogin(bool error)
{
    if (!error)
    {
        logger.info(LOG_PREFIX, "connected to ", m_endpoint.host, ":", m_endpoint.port);
        std::lock_guard lk(m_mutex);
        read();
    }
    else
    {
        logger.error(LOG_PREFIX, "send login failed");
    }
}

void AprscClient::handleSendKeepAlive(bool error)
{
    if (!error)
    {
        std::lock_guard lk(m_mutex);
        m_connector->onWrite("#keep-alive beacon\r\n", [this](bool error) {
            if (!error)
            {
                sendKeepAlive();
            }
            else
            {
                logger.error(LOG_PREFIX, "send keep-alive beacon failed");
                reconnect();
            }
        });
    }
}

char const* AprscClient::logPrefix() const
{
    return LOG_PREFIX;
}
}  // namespace vfrb::client
