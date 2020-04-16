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

#include "feed/parser/CAtmosphereParser.hpp"

#include "util/string_utils.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::feed::parser
{
CAtmosphere CAtmosphereParser::Parse(String&& str_, u32 prio_) const {
    try {
        if (MatchChecksum({str_.c_str(), str_.length()}) && str_.find("MDA") != String::npos) {
            usize tmpB;
            if ((tmpB = str_.find('B')) != String::npos) {
                --tmpB;
            } else {
                throw error::CParseError();
            }
            usize tmpS;
            if ((tmpS = StringView(str_.c_str(), tmpB).find_last_of(',')) != String::npos) {
                ++tmpS;
            } else {
                throw error::CParseError();
            }
            if (auto [v, ec] = Convert<f64>(str_.c_str() + tmpS, str_.c_str() + tmpB); ec == EErrc::OK) {
                return {prio_, v * 1000.0, std::move(str_)};
            }
        }
    } catch ([[maybe_unused]] str_util::error::CConversionError const&) {
    }
    throw error::CParseError();
}
}  // namespace vfrb::feed::parser
