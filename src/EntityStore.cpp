#include "ESE/EntityStore.h"

#include <algorithm>

namespace ESE
{
    const Entity Entity::INVALID(UINT32_MAX);

    Entity EntityStore::create()
    {
        Entity entity;

        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Reusable.size())
        {
            entity = m_Reusable.back();
            m_Reusable.pop_back();
        }
        else
        {
            entity = m_Last;
            ++entity;
            while((entity != m_Last) && (entity == Entity::INVALID || std::find(m_Used.begin(), m_Used.end(), entity) != m_Used.end()))
            {
                ++entity;
            }

            if (entity != m_Last)
            {
                m_Last = entity;
            }
            else
            {
                entity = Entity::INVALID;
            }
        }
        return entity;
    }


    void EntityStore::remove(Entity a_Entity)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto pos = std::find(m_Used.begin(), m_Used.end(), a_Entity);
        if (pos != m_Used.end())
        {
            m_Used.emplace(pos, m_Used.back());
            m_Used.pop_back();
            m_Reusable.emplace_back(a_Entity);
        }
    }

    bool EntityStore::exists(Entity a_Entity)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return std::find(m_Used.begin(), m_Used.end(), a_Entity) != m_Used.end();
    }
}