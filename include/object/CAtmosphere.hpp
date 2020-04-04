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

#include "util/class_utils.hpp"

#include "CObject.hpp"

namespace vfrb::object
{
struct SClimate;

/// Atmospheric information object
class CAtmosphere : public CObject
{
    CTCONST ICAO_STD     = 1013.25;  ///< ICAO standard atmospheric pressure at MSL
    CTCONST MAX_PRESSURE = 2000.0;
    CTCONST MIN_PRESSURE = 0.0;

    f64    m_pressure = ICAO_STD;  ///< The atmospheric pressure
    String m_nmea;                 ///< The NMEA string

    void assign(CObject&& other_) override;

public:
    MOVABLE_BUT_NOT_COPYABLE(CAtmosphere)

    CAtmosphere() = default;

    /// @param prio_ The initial priority
    CAtmosphere(u32 prio_, String&& nmea_);

    /// @param prio_ The initial priority
    CAtmosphere(u32 prio_, f64 press_, String&& nmea_);
    ~CAtmosphere() noexcept override = default;

    GETTER_REF(Nmea, m_nmea)
    GETTER(Pressure, m_pressure)
};
}  // namespace vfrb::object
