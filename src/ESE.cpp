#include "ESE.h"
#include <algorithm>
#include <iostream>

namespace ESE {
    Engine::Engine(std::shared_ptr<SystemContainer> a_Systems, uint64_t a_ThreadCount, uint64_t a_CurrentTick):
        m_Systems(a_Systems), m_State(PAUSED), m_CurrentTick(a_CurrentTick), m_ThreadCount(a_ThreadCount),
        m_DT(0), barrier(a_ThreadCount)
    {
        for( uint64_t i = 0; i < m_ThreadCount; ++i)
        {
            m_Threads.emplace_back(&Engine::threadRun, this, i);
        }
    }

    Engine::~Engine()
    {
        m_State.store(STOPPED);

        for(auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
        {
            it->join();
        }
        m_Threads.clear();
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
            if (a_Callback(state, m_CurrentTick, std::chrono::duration<double>(current_time - previous_time).count()) && state != STOPPED);
            else
            {
                break;
            }

            if (state == PAUSED)
            {
                // Update m_DT
                m_DT.store(std::chrono::duration<double>(current_time - previous_time).count(), std::memory_order_acquire);
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

    std::shared_ptr<SystemContainer> Engine::getSystems()
    {
        return m_Systems;
    }

    void Engine::threadRun(uint64_t a_ThreadID)
    {
        // m_Mutex.lock();
        // std::cerr << "Thread #" << a_ThreadID << " starting." << std::endl;
        // m_Mutex.unlock();

        while (m_State.load(std::memory_order_acq_rel) > STOPPED)
        {
            if (m_State.load(std::memory_order_acq_rel) == RUNNING)
            {
                for (auto itr = m_Systems->begin(); itr != m_Systems->end(); ++itr)
                {
                    if((*itr)->isTickable())
                    {
                        (*itr)->tick(m_ThreadCount, a_ThreadID, m_DT.load(std::memory_order_release));
                    }
                }

                // Wait until all threads are done to move onto the resolution phase
                // m_Mutex.lock();
                // m_TickStageCompleted.fetch_add(1, std::memory_order_release);
                // m_Mutex.unlock();
                // while(m_TickStageCompleted.load(std::memory_order_acquire) != m_ThreadCount);
                barrier.count_down_and_wait();

                auto total = m_Systems->size();
                size_t count = total / m_ThreadCount + 1; // How many per thread
                auto idx = count * a_ThreadID;
                auto end_idx = idx + count;

                for (; idx < total && idx < end_idx; ++idx)
                {
                    (*m_Systems)[idx]->resolve();
                }

                /**
                 * This section is super inefficient. I need to find a better way of doing this
                 **/
                // Wait until resolutions are complete to continue
                // m_Mutex.lock();
                // m_ResolutionStageCompleted.fetch_add(1, std::memory_order_release);
                // m_Mutex.unlock();
                // while(m_ResolutionStageCompleted.load(std::memory_order_acquire) != m_ThreadCount);
                m_State.store(PAUSED, std::memory_order_release);
                barrier.count_down_and_wait();

                // Signal the main thread that this run completed successfully
                // m_State.store(PAUSED, std::memory_order_release);

                // Reset everything
                // m_Mutex.lock();
                // m_TickStageCompleted.fetch_sub(1);
                // m_Mutex.unlock();
                // while(m_TickStageCompleted.load(std::memory_order_release) != 0);

                // m_Mutex.lock();
                // m_ResolutionStageCompleted.fetch_sub(1);
                // m_Mutex.unlock();
                // while(m_ResolutionStageCompleted.load(std::memory_order_release) != 0);
            }
        }
        // m_Mutex.lock();
        // std::cerr << "Thread #" << a_ThreadID << " ending." << std::endl;
        // m_Mutex.unlock();
    }

    void Engine::print_status()
    {
        // std::cout << "Threads completed Tick: " << m_TickStageCompleted.load(std::memory_order_acq_rel) << std::endl;
        // std::cout << "Threads completed Resolution: " << m_ResolutionStageCompleted.load(std::memory_order_acq_rel) << std::endl;
    }
}