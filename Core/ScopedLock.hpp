/*
 * Tencent is pleased to support the open source community by making
 * MMKV available.
 *
 * Copyright (C) 2018 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 
 保证线程安全?
 lock模板，方便锁替换
 
 */

#ifndef MMKV_SCOPEDLOCK_HPP
#define MMKV_SCOPEDLOCK_HPP
#ifdef __cplusplus

namespace mmkv {

template <typename T>
class ScopedLock {
    T *m_lock;

    void lock() {
        if (m_lock) {
            m_lock->lock();
        }
    }

    void unlock() {
        if (m_lock) {
            m_lock->unlock();
        }
    }

public:
    explicit ScopedLock(T *oLock) : m_lock(oLock) {
        MMKV_ASSERT(m_lock);
        lock();
    }

    ~ScopedLock() {
        unlock();
        m_lock = nullptr;
    }

    // just forbid it for possibly misuse
    explicit ScopedLock(const ScopedLock<T> &other) = delete;
    ScopedLock &operator=(const ScopedLock<T> &other) = delete;
};

} // namespace mmkv

#include <type_traits>

// SCOPED_LOCK是一个宏展开后，得到是一个 ScopedLock 对象，通过 RAII 的方式实现在函数内上锁, 其中用到了一个特殊宏__COUNTER__

/*
 RAII 资源获取就是初始化，c++等变成语言常用的管理资源、避免内存泄漏的方法，保证在任何情况下，使用对象时先构造对象，最后析构对象
 抱枕资源管理时的异常安全
 
 __COUNTER__可以保证每次加锁都不一样
 */
#define SCOPED_LOCK(lock) _SCOPEDLOCK(lock, __COUNTER__)
#define _SCOPEDLOCK(lock, counter) __SCOPEDLOCK(lock, counter)
#define __SCOPEDLOCK(lock, counter)                                                                                    \
    mmkv::ScopedLock<std::remove_pointer<decltype(lock)>::type> __scopedLock##counter(lock)

#endif
#endif //MMKV_SCOPEDLOCK_HPP
