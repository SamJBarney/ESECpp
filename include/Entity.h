#pragma once

#include <cstdint>
#include <type_traits>

#include <ESE/Component.h>



namespace ESE {
    class Entity
    {
        uint32_t m_ID;
    public:

        static const Entity INVALID;
        Entity() {}
        Entity(uint32_t a_ID): m_ID(a_ID) {}

        bool operator==(const Entity & a_Other) { return m_ID == a_Other.m_ID; }
        bool operator!=(const Entity & a_Other) { return !(*this == a_Other); }
        Entity& operator++() { ++m_ID; return *this; }
        Entity& operator=(const Entity & a_Other) { m_ID = a_Other.m_ID; return *this; }

        Component * add(const std::string & a_SysName, Component * a_Component);

        Component * find(const std::string & a_SysName);

        void remove(const std::string & a_SysName);
        void removeAll();

        uint32_t id() { return m_ID; }
    };
}