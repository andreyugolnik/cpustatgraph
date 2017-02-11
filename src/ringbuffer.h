/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <cstddef>

template<typename T, size_t Size>
class cRingBuffer
{
public:
    void addValue(const T& value)
    {
        m_last = (m_last + 1) % Size;
        m_buffer[m_last] = value;
    }

    const T& get(size_t idx) const
    {
        return m_buffer[(m_last + idx) % Size];
    }

    size_t size() const
    {
        return Size;
    }

private:
    size_t m_last = 0;
    T m_buffer[Size] = { static_cast<T>(0) };
};
