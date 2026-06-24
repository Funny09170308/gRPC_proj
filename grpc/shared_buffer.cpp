#include "shared_buffer.h"

size_t SharedDataBuffer::GetSize() const
{
    lock_guard<mutex> lock(m_mutex);
    return m_buffer.size();
}

void SharedDataBuffer::Resize(size_t new_size)
{
    lock_guard<mutex> lock(m_mutex);
    m_buffer.resize(new_size);
}

void SharedDataBuffer::Clear()
{
    lock_guard<mutex> lock(m_mutex);
    m_buffer.clear();
}

void SharedDataBuffer::WriteData(const uint8_t *data, size_t size, size_t offset)
{
    lock_guard<mutex> lock(m_mutex);
    if (offset + size > m_buffer.size())
    {
        m_buffer.resize(offset + size);
    }
    memcpy(m_buffer.data() + offset, data, size);
}

void SharedDataBuffer::AppendData(const uint8_t *data, size_t size)
{
    lock_guard<mutex> lock(m_mutex);
    size_t old_size = m_buffer.size();
    m_buffer.resize(old_size + size);
    memcpy(m_buffer.data() + old_size, data, size);
}

size_t SharedDataBuffer::ReadData(uint8_t *out_data, size_t max_size, size_t offset) const
{
    lock_guard<mutex> lock(m_mutex);
    if (offset >= m_buffer.size())
    {
        return 0;
    }
    size_t read_size = min(max_size, m_buffer.size() - offset);
    memcpy(out_data, m_buffer.data() + offset, read_size);
    return read_size;
}

uint8_t *SharedDataBuffer::GetRawData()
{
    return m_buffer.data();
}

const uint8_t *SharedDataBuffer::GetRawData() const
{
    return m_buffer.data();
}

mutex &SharedDataBuffer::GetMutex()
{
    return m_mutex;
}
