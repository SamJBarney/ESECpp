#pragma once
#include "barrier.h"
#include "SystemContainer.h"
#include <atomic>
#include <thread>

namespace ESE
{
    struct Engine
    {
        enum EngineState {
            STOPPED,
            PAUSED,
            RUNNING
        };
        typedef std::function<bool(EngineState, uint64_t, double)> Callback;

        Engine(std::shared_ptr<SystemContainer> a_Systems, uint64_t a_ThreadCount, uint64_t a_CurrentTick = 0);
        ~Engine();

        void start(std::chrono::milliseconds a_TickSize, Callback a_Callback);
        void stop();

        std::shared_ptr<SystemContainer> getSystems();
        void print_status();

    private:
        void threadRun(uint64_t a_ThreadID);

        std::shared_ptr<SystemContainer> m_Systems;
        std::atomic<EngineState> m_State;
        uint64_t m_CurrentTick;
        std::atomic<double> m_DT;
        const uint64_t m_ThreadCount;
        std::vector<std::thread> m_Threads;
        std::mutex m_Mutex;
        barrier barrier;
    };
}