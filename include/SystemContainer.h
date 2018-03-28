#pragma once

#include <map>
#include <string>
#include "System.h"


namespace ESE {
    class SystemContainer
    {
        std::vector<std::string> m_SystemNames;
        std::vector<std::shared_ptr<System_base> > m_Systems;

        ptrdiff_t findIDX(const std::string & a_Name)
        {
            return std::find(m_SystemNames.begin(), m_SystemNames.end(), a_Name) - m_SystemNames.begin();

        }
    public:
        void addSystem(const std::string & a_Name, std::shared_ptr<System_base> a_System)
        {
            if (findIDX(a_Name) >= m_SystemNames.size())
            {
                std::cerr << "Adding system '" << a_Name << "'\n";
                m_Systems.push_back(a_System);
                m_SystemNames.push_back(a_Name);
            }
        }

        void removeSystem(const std::string & a_Name)
        {
            auto idx = findIDX(a_Name);
            if (idx < m_SystemNames.size())
            {
                m_SystemNames[idx] = m_SystemNames.back();
                m_SystemNames.pop_back();
                m_Systems[idx] = m_Systems.back();
                m_Systems.pop_back();
            }
        }

        std::shared_ptr<System_base> getSystem(const std::string & a_Name)
        {
            auto idx = findIDX(a_Name);
            if (idx < m_SystemNames.size())
            {
                return m_Systems[idx];
            }
            std::shared_ptr<System_base> fake;
            return fake;
        }

        std::shared_ptr<System_base> operator[](size_t a_Idx)
        {
            return m_Systems[a_Idx];
        }

        size_t size()
        {
            return m_SystemNames.size();
        }

        typedef std::vector<std::shared_ptr<System_base> >::iterator iterator;
        iterator begin() { return m_Systems.begin(); }
        iterator end() { return m_Systems.end(); }
    };
}