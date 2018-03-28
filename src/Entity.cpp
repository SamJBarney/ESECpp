#include <ESE/Entity.h>
#include <ESE/Engine.h>


namespace ESE {
    Component * Entity::add(const std::string & a_SysName, Component * a_Component)
    {
        auto system = Engine::getSystems()->getSystem(a_SysName);
        if (system != nullptr)
        {

            return system->add(*this, a_Component);
        }
        return nullptr;
    }

    Component * Entity::find(const std::string & a_SysName)
    {
        auto system = Engine::getSystems()->getSystem(a_SysName);
        if (system != nullptr)
        {

            return system->find(*this);
        }
        return nullptr;
    }

    void Entity::remove(const std::string & a_SysName)
    {
        auto system = Engine::getSystems()->getSystem(a_SysName);
        if (system != nullptr)
        {
            system->remove(*this);
        }
    }

    void Entity::removeAll()
    {
        auto system = Engine::getSystems();

    }
}

