/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "server/net/impl/CAcceptorBoost.hpp"

#include "server/CConnection.hpp"

#include "CLogger.hpp"

using asio::ip::tcp;
using asio::error_code;

namespace vfrb::server::net
{
CTCONST            LOG_PREFIX = "(NetworkInterfaceImplBoost) ";
static auto const& logger     = CLogger::Instance();

CAcceptorBoost::CAcceptorBoost(u16 port_)
    : IAcceptor<CSocketBoost>(),
      m_ioCtx(),
      m_acceptor(m_ioCtx, tcp::endpoint(tcp::v4(), port_), tcp::acceptor::reuse_address(true)),
      m_socket(std::move(tcp::socket(m_ioCtx))) {}

CAcceptorBoost::~CAcceptorBoost() noexcept {
    Stop();
}

void
CAcceptorBoost::Run() {
    try {
        if (m_acceptor.is_open()) {
            m_ioCtx.run();
        }
    } catch (std::exception const& e) {
        logger.Error(LOG_PREFIX, ": ", e.what());
    } catch (...) {
        logger.Error(LOG_PREFIX, ": unknown error");
    }
}

void
CAcceptorBoost::Stop() {
    if (m_acceptor.is_open()) {
        m_acceptor.close();
    }
    if (!m_ioCtx.stopped()) {
        m_ioCtx.stop();
    }
    m_socket.Close();
}

void
CAcceptorBoost::OnAccept(Callback cb_) {
    if (m_acceptor.is_open()) {
        m_acceptor.async_accept(m_socket.Get(),
                                [this, cb{std::move(cb_)}](error_code err_) { handleAccept(err_, cb); });
    }
}

void
CAcceptorBoost::Close() {
    m_socket.Close();
}

void
CAcceptorBoost::handleAccept(error_code err_, Callback cb_) {
    if (err_) {
        logger.Debug(LOG_PREFIX, "accept: ", err_.message());
        cb_(Err());
    } else {
        cb_(Ok());
    }
}

auto
CAcceptorBoost::StartConnection() -> CConnection<CSocketBoost> {
    if (!m_socket.Get().is_open()) {
        throw error::CSocketError("cannot start connection on closed socket");
    }
    return CConnection<CSocketBoost>(std::move(m_socket));
}

auto
CAcceptorBoost::StagedAddress() const -> String {
    return m_socket.Address();
}
}  // namespace vfrb::server::net
