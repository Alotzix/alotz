#include "scheduler.h"
#include "log.h"
#include "macro.h"

namespace alotz {

static alotz::Logger::ptr g_logger = ALOTZ_LOG_NAME("system");

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) 
    : m_name(name) {
    ALOTZ_ASSERT(threads > 0);

    if (use_caller) {
        alotz::Fiber::GetThis();
        --threads;

        ALOTZ_ASSERT((GetThis() == nullptr));
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        alotz::Thread::SetName(m_name);
        t_fiber = m_rootFiber.get();
        m_rootThread = alotz::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    } else {
        m_rootThread = -1;
    }
    m_threadCount = threads;
}

Scheduler::~Scheduler() {
    ALOTZ_ASSERT(m_stopping);
    if (GetThis() == this) {
        t_scheduler = nullptr;
    }
}

Scheduler* Scheduler::GetThis() {
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber() {
    return t_fiber;
}

void Scheduler::start() {
    MutexType::Lock lock(m_mutex);
    if (!m_stopping) {
        return;
    }
    m_stopping = false;
    ALOTZ_ASSERT(m_threads.empty());

    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this)
                    , m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    lock.unlock();

    if (m_rootFiber) {
        m_rootFiber->call();
        ALOTZ_LOG_INFO(g_logger) << "call out" << m_rootFiber->getState();
    }
}

void Scheduler::stop() {
    m_autostop = true;
    if (m_rootFiber && m_threadCount == 0 && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)) {
        ALOTZ_LOG_INFO(g_logger) << this << "stopped";
        m_stopping = true;
        if (stopping()) {
            return;
        }
    }

    if (m_rootThread != -1) {
        ALOTZ_ASSERT((GetThis() == this));
    } else {
        ALOTZ_ASSERT((GetThis() != this));
    }

    m_stopping = true;
    for (size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    if (m_rootFiber) {
        tickle();
    }

    if (stopping()) {
        return;
    }
}

void Scheduler::setThis() {
    t_scheduler = this;
}

void Scheduler::run() {
    ALOTZ_LOG_INFO(g_logger) << "run";
    setThis();
    if (alotz::GetThreadId() != m_rootThread) {
        t_fiber = Fiber::GetThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;

    FiberAndThread ft;
    while (true) {
        ft.reset();
        bool tickle_me = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            while (it != m_fibers.end()) {
                if (it->thread != -1 && it->thread != alotz::GetThreadId()) {
                    ++it;
                    tickle_me = true;
                    continue;
                }

                ALOTZ_ASSERT(it->fiber || it->cb);
                if (it->fiber && it->fiber->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }

                ft = *it;
                m_fibers.erase(it);
                break;
            }
        }

        if (tickle_me) {
            tickle();
        }

        if (ft.fiber && (ft.fiber->getState() != Fiber::TERM && ft.fiber->getState() != Fiber::EXCEPT)) {
            ++m_activeThreadCount;
            ft.fiber->swapIn();
            --m_activeThreadCount;

            if (ft.fiber->getState() == Fiber::READY) {
                schedule(ft.fiber);
            } else if (ft.fiber->getState() != Fiber::TERM ||
                        ft.fiber->getState() != Fiber::EXCEPT) {
                ft.fiber->m_state = Fiber::HOLD;
            }
            ft.reset();
        } else if (ft.cb) {
            if (cb_fiber) {
                cb_fiber->reset(ft.cb);
            } else {
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.reset();
            ++m_activeThreadCount;
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if (cb_fiber->getState() == Fiber::EXCEPT || cb_fiber->getState() == Fiber::TERM) {
                cb_fiber->reset(nullptr);
            } else {
                cb_fiber->m_state = Fiber::HOLD;
                cb_fiber.reset();
            }
        } else {
            if (idle_fiber->getState() == Fiber::TERM) {
                ALOTZ_LOG_INFO(g_logger) << "idle fiber term";
                break;
            }

            ++m_activeThreadCount;
            idle_fiber->swapIn();
            --m_activeThreadCount;
            if (idle_fiber->getState() != Fiber::TERM && 
                    idle_fiber->getState() != Fiber::EXCEPT) {
                idle_fiber->m_state = Fiber::HOLD;
            }
        }
    }
}

void Scheduler::tickle() {
    ALOTZ_LOG_INFO(g_logger) << "tickle";
}

bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);
    return m_autostop && m_stopping && m_fibers.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle() {
    ALOTZ_LOG_INFO(g_logger) << "idle";
}


}