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

#include "server/net/Acceptor.hpp"
#include "util/class_utils.h"

namespace vfrb
{
namespace server
{
namespace net
{
class CSocketImplTest;

class CAcceptorImplTest : public IAcceptor<CSocketImplTest>
{
    NOT_COPYABLE(CAcceptorImplTest)

    std::function<void(bool)>                          m_onAcceptFn;
    Str                                                m_stagedAddress;
    bool                                               m_stopped = false;
    bool                                               m_fail    = false;
    std::vector<std::pair<CSocketImplTest, SPtr<Str>>> m_sockets;

public:
    explicit CAcceptorImplTest(bool failConnect_);
    ~CAcceptorImplTest() noexcept override = default;

    void Run() override;
    void Stop() override;
    void OnAccept(Callback&& cb_) override;
    void Close() override;

    CConnection<CSocketImplTest> StartConnection() override;
    Str                          StagedAddress() const override;

    usize Connect(Str const& addr_, bool failAccept_, bool failWrite_);
    auto  Socket(usize i_) const -> CSocketImplTest const&;
    auto  Buffer(usize i_) const -> Str const&;
    usize Sockets() const;
};
}  // namespace net
}  // namespace server
}  // namespace vfrb
