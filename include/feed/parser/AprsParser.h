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
#include <string>

#include <boost/regex.hpp>

#include "object/Aircraft.h"
#include "util/defines.h"

#include "Parser.hpp"

namespace feed
{
namespace parser
{
/**
 * @brief Implement Parser for APRS sentences.
 */
class AprsParser : public Parser<object::Aircraft>
{
public:
    DEFAULT_DTOR(AprsParser)

    AprsParser();

    /**
     * @brief Unpack into Aircraft.
     * @param sentence The string to unpack
     * @param aircraft The Aircraft to unpack into
     * @return true on success, else false
     */
    bool unpack(const std::string& sentence, object::Aircraft& aircraft) noexcept override;

    /// The max height filter
    static std::int32_t s_maxHeight;

private:
    /**
     * @brief Parse a Position.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parsePosition(const boost::smatch& match, object::Aircraft& aircraft) noexcept;

    /**
     * @brief Parse the APRS comment.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parseComment(const boost::smatch& match, object::Aircraft& aircraft) noexcept;

    /**
     * @brief Parse the Movement information.
     * @param match    The regex match
     * @param comMatch The comment regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parseMovement(const boost::smatch& match, const boost::smatch& comMatch,
                       object::Aircraft& aircraft) noexcept;

    /**
     * @brief Parse the TimeStamp information.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parseTimeStamp(const boost::smatch& match, object::Aircraft& aircraft) noexcept;

    /// Regular expression for APRS protocol
    static const boost::regex s_APRS_RE;

    /// Regular expression for OGN specific APRS extension
    static const boost::regex s_APRSExtRE;
};
}  // namespace parser
}  // namespace feed
