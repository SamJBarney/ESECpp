#include "ESE/Engine.h"
#include <algorithm>
#include <iostream>

namespace ESE {


    SystemContainer Engine::m_Systems;
    EntityStore Engine::m_EntityStore;
    std::atomic<Engine::EngineState> Engine::m_State;
    uint64_t Engine::m_CurrentTick;
    std::atomic<double> Engine::m_DT;
    uint64_t Engine::m_ThreadCount;
    std::vector<std::thread> Engine::m_Threads;
    std::mutex Engine::m_Mutex;
    barrier * Engine::m_Barrier;

    void Engine::setup(uint64_t a_ThreadCount, uint64_t a_CurrentTick)
    {
        m_State = PAUSED;
        m_DT = 0;
        m_ThreadCount = a_ThreadCount;
        m_CurrentTick = a_CurrentTick;
        m_Barrier = new barrier(a_ThreadCount);

        for( uint64_t i = 0; i < m_ThreadCount; ++i)
        {
            m_Threads.emplace_back(&Engine::threadRun, i);
        }
    }

    void Engine::start(std::chrono::milliseconds a_TickDuration, Callback a_Callback)
    {
        m_DT.store(std::chrono::duration<double>(a_TickDuration).count(), std::memory_order_acquire);
        m_State.store(RUNNING, std::memory_order_release);
        auto previous_time = std::chrono::steady_clock::now();
        while(true)
        {
            // Sleep
            std::this_thread::sleep_for(a_TickDuration);
            auto current_time = std::chrono::steady_clock::now();
            auto state = m_State.load(std::memory_order_acquire);
            double dt = std::chrono::duration<double>(current_time - previous_time).count();
            if (a_Callback(state, m_CurrentTick, dt) && state != STOPPED);
            else
            {
                break;
            }

            if (state == PAUSED)
            {
                // Update m_DT
                m_DT.store(dt, std::memory_order_acquire);
                // Update current tick
                m_CurrentTick++;

                // Start the system going again
                m_State.store(RUNNING, std::memory_order_release);

                // Update the reference time for calculating m_DT
                previous_time = current_time;
            }
        }
    }

    void Engine::stop()
    {
        m_State.store(STOPPED, std::memory_order_release);
    }

    void Engine::cleanup()
    {
        m_State.store(STOPPED);

        for(auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
        {
            it->join();
        }
        m_Threads.clear();
    }

    SystemContainer * Engine::getSystems()
    {
        return &m_Systems;
    }

    EntityStore * Engine::getEntityStore()
    {
        return &m_EntityStore;
    }

    uint64_t Engine::getCurrentTick()
    {
        return m_CurrentTick;
    }

    void Engine::threadRun(uint64_t a_ThreadID)
    {
        while (m_State.load(std::memory_order_acq_rel) > STOPPED)
        {
            if (m_State.load(std::memory_order_acq_rel) == RUNNING)
            {
                for (auto itr = m_Systems.begin(); itr != m_Systems.end(); ++itr)
                {
                    if((*itr)->isTickable())
                    {
                        (*itr)->tick(m_ThreadCount, a_ThreadID, m_DT.load(std::memory_order_release));
                    }
                }

                m_Barrier->count_down_and_wait();

                auto total = m_Systems.size();
                size_t count = total / m_ThreadCount + 1; // How many per thread
                auto idx = count * a_ThreadID;
                auto end_idx = idx + count;

                for (; idx < total && idx < end_idx; ++idx)
                {
                    m_Systems[idx]->resolve();
                }

                m_State.store(PAUSED, std::memory_order_release);
                m_Barrier->count_down_and_wait();
            }
        }
    }
}