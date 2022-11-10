#include <atomic>
#include "fiber.h"
#include "config.h"
#include "macro.h"
#include "log.h"
#include "scheduler.h"

namespace alotz {

static alotz::Logger::ptr g_logger = ALOTZ_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
    Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    if (getcontext(&m_ctx)) {
        ALOTZ_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;

    ALOTZ_LOG_DEBUG(g_logger) << "Fiber::Fiber";
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller) 
    : m_id(++s_fiber_id) 
    , m_cb(cb) {
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if (getcontext(&m_ctx)) {
        ALOTZ_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    if (!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }

    ALOTZ_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {
        ALOTZ_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT || m_state == HOLD);

        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        ALOTZ_ASSERT(!m_cb);
        ALOTZ_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }
    ALOTZ_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id;
}

void Fiber::reset(std::function<void()> cb) {
    ALOTZ_ASSERT(m_stack);
    ALOTZ_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);
    m_cb = cb;
    if (getcontext(&m_ctx)) {
        ALOTZ_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    ALOTZ_LOG_ERROR(g_logger) << getId();
    if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        ALOTZ_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {
    SetThis(this);
    if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        ALOTZ_ASSERT2(false, "swapcontext");
    }
}

// 切换到当前fiber
void Fiber::swapIn() {
    SetThis(this);
    ALOTZ_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if (swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
        ALOTZ_ASSERT2(false, "swapcontext");
    }
}

// 切换回mainFiber
void Fiber::swapOut() {
    SetThis(Scheduler::GetMainFiber());

    if (swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
        ALOTZ_ASSERT2(false, "swapcontext");
    }
}

void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() {
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    ALOTZ_ASSERT((t_fiber == main_fiber.get()));
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    ALOTZ_ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    ALOTZ_ASSERT(cur->m_state == EXEC);
    // cur->m_state = HOLD;
    cur->swapOut();
}
 
uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

// 执行当前fiber,执行结束切回Scheduler::mainFiber
void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    ALOTZ_ASSERT(cur);
    // try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    // } catch (std::exception& e) {
    //     cur->m_state = EXCEPT;
    //     ALOTZ_LOG_ERROR(g_logger) << "Fiber except: " << e.what()
    //         << " fiber_id= " << cur->getId()
    //         << std::endl
    //         << alotz::BacktraceToString();
    // }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    ALOTZ_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

void Fiber::CallerMainFunc() {
    Fiber::ptr cur = GetThis();
    ALOTZ_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM; 
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        ALOTZ_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << "fiber_id= " << cur->getId()
            << std::endl
            << alotz::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        ALOTZ_LOG_ERROR(g_logger) << "Fiber Except: "
            << "fiber_id= " << cur->getId()
            << std::endl
            << alotz::BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();
    ALOTZ_ASSERT2(false, "never reach fiber id= " + std::to_string(raw_ptr->getId()));
}

}