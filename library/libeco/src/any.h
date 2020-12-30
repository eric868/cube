#ifndef ANY_H_
#define ANY_H_

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <typeindex>
#include <exception>

namespace eco{;

struct Any
{
    Any(void) : m_typeindex(std::type_index(typeid(void))) {}
    Any(const Any& that) : m_ptr(that.Clone()), m_typeindex(that.m_typeindex) {}
    Any(Any && that) : m_ptr(std::move(that.m_ptr)), m_typeindex(that.m_typeindex) {}

    //��������ָ��ʱ������һ������ͣ�ͨ��std::decay���Ƴ����ú�cv�����Ӷ���ȡԭʼ����
    template<typename U>
    Any(U && value)
        : m_ptr(new Derived < typename std::decay<U>::type>(std::forward<U>(value)))
        , m_typeindex(std::type_index(typeid(typename std::decay<U>::type))) 
    {
    }

    bool IsNull() const { return !bool(m_ptr); }

    template<class U> bool Is() const
    {
        return m_typeindex == std::type_index(typeid(U));
    }

    //��Anyת��Ϊʵ�ʵ�����
    template<class U>
    U& AnyCast()
    {
        if (!Is<U>())
        {
            std::cout << "can not cast " << typeid(U).name() << " to " << m_typeindex.name() << std::endl;
            throw std::bad_cast();
        }

        auto derived = dynamic_cast<Derived<U>*> (m_ptr.get());
        return derived->m_value;
    }

    Any& operator=(const Any& a)
    {
        if (m_ptr == a.m_ptr)
            return *this;

        m_ptr = a.Clone();
        m_typeindex = a.m_typeindex;
        return *this;
    }

private:
    struct Base;
    typedef std::unique_ptr<Base> BasePtr;

    struct Base
    {
        virtual ~Base() {}
        virtual BasePtr Clone() const = 0;
    };

    template<typename T>
    struct Derived : Base
    {
        template<typename U>
        Derived(U && value) : m_value(std::forward<U>(value)) { }

        BasePtr Clone() const
        {
            return BasePtr(new Derived<T>(m_value));
        }

        T m_value;
    };

    BasePtr Clone() const
    {
        if (m_ptr != nullptr)
            return m_ptr->Clone();

        return nullptr;
    }

    BasePtr m_ptr;
    std::type_index m_typeindex;
};

}// 
#endif