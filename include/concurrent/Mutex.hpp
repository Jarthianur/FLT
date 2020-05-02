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

/// @see https://clang.llvm.org/docs/ThreadSafetyAnalysis.html#mutexheader

#pragma once

#include <mutex>

#include "util/ClassUtils.hpp"

#if defined(__clang__) && (!defined(SWIG))
#    define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#    define THREAD_ANNOTATION_ATTRIBUTE__(x)  // no-op
#endif

/// A capability is basically a lockable type.
#define CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

/// A capability with RAII style.
#define SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

/// A resource is guarded by the given capability.
#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

/// The resource behind a pointer is guarded by the given capability.
#define PT_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

/// A function requires the given capabilities to be (not) locked.
#define REQUIRES(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

/// A function aquires a lock.
#define ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

/// A function tries to aquire a lock.
#define TRY_ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

/// A function releases a lock.
#define RELEASE(...) THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

/// A function does not need the given capabilities to be locked.
#define EXCLUDES(...) THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

/// Do not perform thread safety analysis.
#define NO_THREAD_SAFETY_ANALYSIS THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

namespace vfrb::concurrent
{
#if defined(__clang__) && (!defined(SWIG))

/// Mutex wrapper for thread safety analysis
class CAPABILITY("mutex") Mutex
{
    NOT_COPYABLE(Mutex)
    NOT_MOVABLE(Mutex)

    std::mutex m_mutex;

public:
    Mutex()           = default;
    ~Mutex() noexcept = default;

    void lock() ACQUIRE() {
        m_mutex.lock();
    }

    void unlock() RELEASE() {
        m_mutex.unlock();
    }

    auto try_lock() -> bool TRY_ACQUIRE(true) {
        return m_mutex.try_lock();
    }

    auto operator!() const -> Mutex const& {
        return *this;
    }
};

/// Lock guard wrapper for thread safety analysis
class SCOPED_CAPABILITY LockGuard
{
    NOT_COPYABLE(LockGuard)
    NOT_MOVABLE(LockGuard)

    std::lock_guard<Mutex> m_lock;

public:
    explicit LockGuard(Mutex& mu_) ACQUIRE(mu_) : m_lock(mu_) {}
    ~LockGuard() noexcept RELEASE() = default;
};

#else

using Mutex     = std::mutex;
using LockGuard = std::lock_guard<std::mutex>;

#endif

using UniqueLock = std::unique_lock<Mutex>;
}  // namespace vfrb::concurrent
