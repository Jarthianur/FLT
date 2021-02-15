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

#include "data/CWindData.hpp"

#include "concurrent/Mutex.hpp"
#include "object/CObject.hpp"
#include "object/CWind.hpp"

using vfrb::object::CObject;
using vfrb::object::CWind;
using vfrb::concurrent::LockGuard;

namespace vfrb::data
{
CWindData::CWindData(AccessFn&& fn_) : IData(std::move(fn_)) {}

CWindData::CWindData(AccessFn&& fn_, object::CWind&& wind_)
    : IData(std::move(fn_)), m_wind(std::move(wind_)) {}

auto
CWindData::Update(CObject&& wind_) -> bool {
    LockGuard lk(m_mutex);
    return m_wind.TryUpdate(std::move(wind_));
}

void
CWindData::Access() {
    LockGuard lk(m_mutex);
    m_accessFn(SAccessor{++m_wind, StringView{m_wind.Nmea()}});
    m_wind.Clear();
}

auto
CWindData::Size() const -> usize {
    LockGuard lk(m_mutex);
    return m_wind.Nmea().empty() ? 0 : 1;
}
}  // namespace vfrb::data