#pragma once

#include <string>

namespace ESE
{
    class Entity;
    class Component
    {
    public:
        virtual void tick(const std::string &, Entity, double) = 0;
    };
}