/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#ifndef SRC_TCP_CLIENT_CLIENT_H_
#define SRC_TCP_CLIENT_CLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <string>
#include "../../config/Parameters.h"

#define C_CON_WAIT_TIMEVAL CLIENT_CONNECT_WAIT_TIMEVAL

class Client
{
public:
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    virtual ~Client() noexcept;

    void run();

protected:
    Client(boost::asio::signal_set& /*sigset*/, const std::string& /*host*/,
           const std::string& /*port*/, const std::string& /*comp*/);

    void awaitStop();
    void timedConnect() noexcept;
    virtual void stop() noexcept;
    virtual void read() noexcept;
    virtual void process() noexcept = 0;
    virtual void connect() noexcept = 0;

    void handleTimedConnect(const boost::system::error_code& /*ec*/) noexcept;
    void handleRead(const boost::system::error_code& /*ec*/, std::size_t /*s*/) noexcept;

    virtual void handleResolve(const boost::system::error_code& /*ec*/,
                               boost::asio::ip::tcp::resolver::iterator /*it*/)
                                       noexcept = 0;
    virtual void handleConnect(const boost::system::error_code& /*ec*/,
                               boost::asio::ip::tcp::resolver::iterator /*it*/)
                                       noexcept = 0;

    boost::asio::io_service mIOservice;
    boost::asio::signal_set& mrSigSet;
    boost::asio::ip::tcp::socket mSocket;
    boost::asio::ip::tcp::resolver mResolver;

    std::string mResponse;
    boost::asio::streambuf mBuffer;
    const std::string mHost;
    const std::string mPort;
    const std::string mComponent;

private:
    boost::asio::deadline_timer mConnectTimer;
};

#endif /* SRC_TCP_CLIENT_CLIENT_H_ */