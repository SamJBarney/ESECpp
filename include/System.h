#pragma once

#include <vector>
#include <string>
#include "Entity.h"
#include "Component.h"

#include "Engine.h"

#include <iostream>


namespace ESE {
    class System_base
    {
        bool m_Tickable;

    protected:
        const std::string m_Name;
        std::vector<Entity> m_Entities;

        ptrdiff_t findIDX(Entity a_Entity)
        {
            auto tmp = std::find(m_Entities.begin(), m_Entities.end(), a_Entity);
            return std::distance(m_Entities.begin(), tmp);
        }

    public:
        System_base(bool is_Tickable, const char * a_Name): m_Tickable(is_Tickable), m_Name(a_Name) {}
        bool isTickable() { return m_Tickable; }

        const std::string & name() { return m_Name; };

        virtual size_t count() = 0;

        virtual Component * add(Entity, Component*) = 0;
        virtual Component * find(Entity) = 0;
        virtual void remove(Entity) = 0;

        virtual void resolve() = 0;

        virtual void tick(uint16_t a_ThreadCount, uint16_t a_ID, double a_DT) {};
    };

    template<typename T>
    class System : public System_base
    {
        std::vector<T> m_Components;
        std::vector<T> m_Add;
        std::vector<Entity> m_Delete;
    public:
        System(bool is_Tickable, const char * a_Name): System_base(is_Tickable, a_Name) {}

        virtual size_t count() override { return m_Components.size(); }

        virtual Component * add(Entity a_Entity, Component* a_Component) override
        {
            auto result = find(a_Entity);
            if (result == nullptr)
            {
                m_Add.push_back(*static_cast<T*>(a_Component));
                m_Entities.push_back(a_Entity);
                return &m_Add.back();
            }
            return result;
        }

        virtual Component * find(Entity a_Entity) override
        {
            auto idx = findIDX(a_Entity);
            if (idx < m_Entities.size())
            {
                return &m_Components[idx];
            }

            return nullptr;
        }

        virtual void remove(Entity a_Entity) override
        {
            m_Delete.push_back(a_Entity);
        }

        virtual void resolve() override
        {
            if (m_Add.size() > 0)
            {
                std::cerr << "Adding " << m_Add.size() << " Components\n";
                m_Components.insert(m_Components.end(), m_Add.begin(), m_Add.end());
                m_Add.clear();
            }

            if (m_Delete.size() > 0)
            {
                std::cerr << "Deleting " << m_Delete.size() << " out of " << m_Entities.size() << std::endl;
                auto rem = m_Delete.begin();
                auto entity = m_Entities.begin();
                for (int64_t i = 0; i < m_Delete.size(); ++ i)
                {
                    for (auto j = 0; j < m_Entities.size(); ++j)
                    {
                        if (m_Delete[i] != m_Entities[j]);
                        else
                        {
                            std::cerr << "Deleting entity #" << m_Entities[j].id() << std::endl;
                            m_Entities[j] = m_Entities.back();
                            m_Entities.pop_back();
                            m_Components[j] = m_Components.back();
                            m_Components.pop_back();
                            break;
                        }
                    }
                }
                m_Delete.clear();
                std::cerr << "Entity count: " << m_Entities.size() << std::endl;
            }
        }

        virtual void tick(uint16_t a_ThreadCount, uint16_t a_ID, double a_DT) override
        {
            if (isTickable())
            {
                size_t size = m_Components.size();

                size_t per_thread = size / (size_t)a_ThreadCount;
                if (per_thread == 0) per_thread = 1;

                size_t start = per_thread * a_ID;
                size_t end = start + per_thread;

                for (size_t i = start; i < end && i < size; ++i)
                {
                    m_Components[i].tick(m_Name, m_Entities[i], a_DT);
                }
            }
        }
    };
}
