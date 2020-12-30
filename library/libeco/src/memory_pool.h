#ifndef MEMORY_POOL_H_
#define MEMORY_POOL_H_

#include <mutex>
#include <vector>
#include <memory>
#include "object.h"

NS_BEGIN(eco);
////////////////////////////////////////////////////////////////////////////////
template<typename Object>
class MemoryPool
{
public:
    inline void reserve(IN const uint32_t size)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_buffer.reserve(size);
    }

    inline std::shared_ptr<Object> create()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_buffer.size() == 0)    // add new buffer item.
        {
            m_buffer.push_back(new Object);
        }

        std::shared_ptr<Object> sp(m_buffer.back(),
            std::bind(&MemoryPool::recycle, this, std::placeholders::_1));
        m_buffer.pop_back();
        return sp;
    }

    template<typename BaseObject>
    inline std::shared_ptr<BaseObject> create()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_buffer.size() == 0)    // add new buffer item.
        {
            m_buffer.push_back(new Object);
        }

        std::shared_ptr<BaseObject> sp(m_buffer.back(),
            std::bind(&MemoryPool::recycle, this, std::placeholders::_1));
        m_buffer.pop_back();
        return sp;
    }

    inline void clear()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::for_each(m_buffer.begin(), m_buffer.end(),
            [](Object* heap_ptr) {
            delete heap_ptr;
        });
        m_buffer.clear();
    }

    inline ~MemoryPool()
    {
        clear();
    }

private:
    inline void recycle(IN Object* p)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_buffer.push_back(p);
    }

    std::mutex m_mutex;
    std::vector<Object*> m_buffer;
};


////////////////////////////////////////////////////////////////////////////////
template<typename Object>
inline MemoryPool<Object>& GetMemoryPool()
{
    return eco::Singleton<MemoryPool<Object>>::instance();
}

#define ECO_POOL_NEW(Object) eco::GetMemoryPool<Object>().create()
////////////////////////////////////////////////////////////////////////////////
NS_END(eco);
#endif
