#pragma once

#include <vector>
#include <mutex>

#include "Entity.h"

namespace ESE
{

    class EntityStore
    {
        std::mutex m_Mutex;
        std::vector<Entity> m_Used;
        std::vector<Entity> m_Reusable;
        Entity m_Last;
    public:
        Entity create();
        void remove(Entity);
        bool exists(Entity);
    };
}