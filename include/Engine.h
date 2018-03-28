#pragma once
#include "SystemContainer.h"
#include "EntityStore.h"
#include "barrier.h"
#include <atomic>
#include <thread>

namespace ESE {
    struct Engine
    {
        enum EngineState {
            STOPPED,
            PAUSED,
            RUNNING
        };
        typedef std::function<bool(EngineState, uint64_t, double)> Callback;


        static void setup(uint64_t a_ThreadCount, uint64_t a_CurrentTick = 0);
        static void start(std::chrono::milliseconds a_TickSize, Callback a_Callback);
        static void stop();
        static void cleanup();

        static SystemContainer * getSystems();
        static EntityStore * getEntityStore();
        static uint64_t getCurrentTick();

    private:
        static void threadRun(uint64_t a_ThreadID);

        static SystemContainer m_Systems;
        static EntityStore m_EntityStore;
        static std::atomic<EngineState> m_State;
        static uint64_t m_CurrentTick;
        static std::atomic<double> m_DT;
        static uint64_t m_ThreadCount;
        static std::vector<std::thread> m_Threads;
        static std::mutex m_Mutex;
        static barrier * m_Barrier;
    };
}