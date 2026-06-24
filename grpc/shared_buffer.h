#ifndef __SHARED_BUFFER_H__
#define __SHARED_BUFFER_H__

#include <vector>
#include <mutex>
#include <memory>
#include <cstring>

using namespace std;

class SharedDataBuffer
{
public:
    static shared_ptr<SharedDataBuffer> GetInstance()
    {
        static shared_ptr<SharedDataBuffer> instance(new SharedDataBuffer());
        return instance;
    }
    SharedDataBuffer(const SharedDataBuffer &) = delete;
    SharedDataBuffer &operator=(const SharedDataBuffer &) = delete;
    SharedDataBuffer(SharedDataBuffer &&) = delete;
    SharedDataBuffer &operator=(SharedDataBuffer &&) = delete;

public:
    size_t GetSize() const;
    void Resize(size_t new_size);
    void Clear();
    void WriteData(const uint8_t *data, size_t size, size_t offset = 0);
    void AppendData(const uint8_t *data, size_t size);
    size_t ReadData(uint8_t *out_data, size_t max_size, size_t offset = 0) const;
    // 获取原始缓冲区指针
    uint8_t *GetRawData();
    const uint8_t *GetRawData() const;

    // 提供锁的访问接口，用于需要长时间持有锁的复杂操作
    mutex &GetMutex();

private:
    SharedDataBuffer() = default;

    mutable mutex m_mutex;
    vector<uint8_t> m_buffer;
};

#endif
