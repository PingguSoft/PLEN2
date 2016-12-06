/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#include <Arduino.h>

#include "Profiler.h"


namespace
{
    namespace Shared
    {
        uint16_t m_nest = 0;
    }
}


void Utility::Profiler::m_tabbing()
{
    for (uint16_t i = 0; i < Shared::m_nest; i++)
    {
        Serial1.write('\t');
    }
}


Utility::Profiler::Profiler(const __FlashStringHelper* fsh_ptr)
{
    m_tabbing();
    Serial1.print(F(">>> pushed : "));
    Serial1.println(fsh_ptr);

    m_tabbing();
    Serial1.print(F("+++ stack ptr : "));
    Serial1.println(reinterpret_cast<uint16_t>(this), HEX);

    Shared::m_nest++;
    m_begin = micros();
}


Utility::Profiler::~Profiler()
{
    m_end = micros();
    Shared::m_nest--;

    m_tabbing();
    Serial1.print(F("+++ nest      : "));
    Serial1.println(Shared::m_nest);

    m_tabbing();
    Serial1.print(F("+++ exec time : "));
    Serial1.print(m_end - m_begin);
    Serial1.println(F(" [usec]"));

    m_tabbing();
    Serial1.println(F("<<< popped"));
}
