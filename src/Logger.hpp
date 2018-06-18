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

#include <iostream>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

#include "Defines.h"

/**
 * @class Logger
 * @brief Provides static functions for threadsafe logging.
 */
class Logger
{
public:
    DEFAULT_CTOR_DTOR(Logger)

    /**
     * @fn info
     * @brief Log on INFO level to stdout.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    static void info(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "\r[INFO]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void info(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "\r[INFO]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg));
    }

    /**
     * @fn debug
     * @brief Log on DEBUG level to stdout.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    static void debug(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "\r[DEBUG] " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void debug(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "\r[DEBUG] " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg));
    }

    /**
     * @fn warn
     * @brief Log on WARN level to stdout.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    static void warn(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "\r[WARN]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void warn(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "\r[WARN]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg));
    }

    /**
     * @fn error
     * @brief Log on ERROR level to stderr.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    static void error(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpErrStream << "\r[ERROR] " << getTime() << ":: ";
        log(mpErrStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void error(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpErrStream << "\r[ERROR] " << getTime() << ":: ";
        log(mpErrStream, std::forward<T>(crMsg));
    }

private:
    /// @var mMutex
    /// Mutex for threadsafe logging
    static boost::mutex mMutex;

    /// @var mpOutStream
    /// Stream to log INFO,DEBUG,WARN
    static std::ostream* mpOutStream;

    /// @var mpErrStream
    /// Stream to log ERROR
    static std::ostream* mpErrStream;

    /**
     * @fn getTime
     * @brief Get current date-time as string.
     * @return the date-time-string
     */
    static std::string getTime();

    /**
     * @fn log
     * @brief Write to the given stream.
     * @param pOut   The stream
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     */
    template<typename T, typename... TRest>
    static void log(std::ostream* pOut, T head, TRest... tail)
    {
        *pOut << std::forward<T>(head);
        log(pOut, std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void log(std::ostream* pOut, T last)
    {
        *pOut << std::forward<T>(last) << std::endl;
    }
};
