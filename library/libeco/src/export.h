#ifndef EXPORT_H_
#define EXPORT_H_

#include <cstdint>
#include <memory>

#ifdef WIN32

#ifdef ECO_EXPORT
#define ECO_API  __declspec(dllexport)
#else
#define ECO_API  __declspec(dllimport)
#endif

#else

#define ECO_API

#endif



/*@ eco namespace.*/
namespace eco {}
#define NS_BEGIN(ns) namespace ns{
#define NS_END(ns) }

/*@ c++ 11.*/
#ifdef ECO_NO_CXX11
#define nullptr (NULL)
#define override
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef TODO
#define TODO(something)
#endif

#ifndef NOTE
#define NOTE(something)
#endif

NS_BEGIN(eco);
// for create shared object.
struct Heap {};
struct Null {};
const Heap heap;
const Null null;

template<typename type_t = int>
class TypeCount
{
public:
    static uint32_t s_type_count;
};
template<typename type_t>
uint32_t TypeCount<type_t>::s_type_count = 0;

template<typename type_t>
class TypeId
{
public:
    inline operator const uint32_t() const
    {
        return value;
    }
    static uint32_t value;
};
template<typename type_t>
uint32_t TypeId<type_t>::value = TypeCount<>::s_type_count++;

template<typename type_t>
inline const char* get_type()
{
    static const char* s_v = typeid(type_t).name();
    return s_v;
}

NS_END(eco);

#define ECO_TYPE(object_t)\
public:\
    inline static const uint32_t type_id()\
    {\
        return eco::TypeId<object_t>::value;\
    }\
    virtual const uint32_t get_type_id() const\
    {\
        return type_id();\
    }\
    inline static const char* type()\
    {\
        return #object_t;\
    }\
    virtual const char* get_type() const\
    {\
        return type();\
    }

#define ECO_NONCOPYABLE(object_t) \
private:\
    object_t(IN const object_t& );\
    object_t& operator=(IN const object_t& );

#define ECO_IMPL_API() \
public:\
    friend class Impl;\
    class Impl;\
    inline Impl& impl();\
    inline const Impl& impl() const;\
protected:\
    Impl* m_impl;

#define ECO_TYPE_API(type_t) \
public:\
    ~type_t();\
    type_t();\
    ECO_IMPL_API()

#define ECO_VALUE_API(value_t) \
public:\
    value_t(IN const value_t&); \
    value_t& operator=(IN const value_t&);\
    ECO_TYPE_API(value_t)

#define ECO_OBJECT_API(object_t)\
public:\
    typedef std::weak_ptr<object_t> wptr;\
    typedef std::shared_ptr<object_t> ptr;\
    object_t(IN eco::Null);\
    object_t& operator=(IN eco::Heap);\
    object_t& operator=(IN eco::Null);\
    bool null() const;\
    void reset();\
    ECO_TYPE_API(object_t)\
    ECO_NONCOPYABLE(object_t)

#define ECO_MOVABLE_API(object_t)\
public:\
    object_t(IN object_t&& moved);\
    object_t& operator=(IN object_t&& moved);\
    ECO_OBJECT_API(object_t)

#define ECO_SHARED_API(object_t) \
public:\
    object_t();\
    ~object_t();\
    object_t(IN eco::Null);\
    object_t(IN object_t&& obj);\
    object_t(IN const object_t& obj);\
    object_t& operator=(IN object_t&& obj);\
    object_t& operator=(IN const object_t& obj);\
    object_t& operator=(IN eco::Heap);\
    object_t& operator=(IN eco::Null);\
    bool operator==(IN const object_t& obj) const;\
    bool operator!=(IN const object_t& obj) const;\
    bool null() const;\
    void swap(IN object_t& obj);\
    void reset();\
    void reserve();\
public:\
    class Proxy;\
    class Impl;\
    inline Impl& impl() const;\
protected:\
    Proxy* m_proxy;

#define ECO_IMPL_INIT(type_t)\
public:\
    inline void init(IN type_t&) {}

////////////////////////////////////////////////////////////////////////////////
#define ECO_IMPL(type_t)\
type_t::Impl& type_t::impl()\
{\
    return (type_t::Impl&)(*m_impl);\
}\
const type_t::Impl& type_t::impl() const\
{\
    return (const type_t::Impl&)(*m_impl);\
}

////////////////////////////////////////////////////////////////////////////////
#define ECO_TYPE_IMPL(type_t)\
type_t::type_t()\
{\
    m_impl = new Impl;\
    m_impl->init(*this);\
}\
type_t::~type_t()\
{\
    delete m_impl;\
    m_impl = nullptr;\
    std::cout << __FUNCTION__ << std::endl;\
}\
ECO_IMPL(type_t)


////////////////////////////////////////////////////////////////////////////////
#define ECO_VALUE_IMPL(value_t) \
ECO_TYPE_IMPL(value_t);\
value_t::value_t(IN const value_t& val)\
{\
    m_impl = new Impl(val.impl());\
}\
value_t& value_t::operator=(IN const value_t& val)\
{\
    impl() = val.impl();\
    return *this;\
}


////////////////////////////////////////////////////////////////////////////////
#define ECO_OBJECT_IMPL(object_t) \
ECO_TYPE_IMPL(object_t);\
object_t::object_t(IN eco::Null)\
{\
    m_impl = nullptr;\
}\
object_t& object_t::operator=(IN eco::Null)\
{\
    reset();\
    return *this;\
}\
object_t& object_t::operator=(IN eco::Heap)\
{\
    m_impl = new Impl;\
    m_impl->init(*this);\
    return *this;\
}\
bool object_t::null() const\
{\
    return m_impl == nullptr;\
}\
void object_t::reset()\
{\
    delete m_impl;\
    m_impl = nullptr;\
}


////////////////////////////////////////////////////////////////////////////////
#define ECO_MOVABLE_IMPL(object_t) \
ECO_OBJECT_IMPL(object_t);\
object_t::object_t(IN object_t&& obj) : m_impl(obj.m_impl)\
{\
    obj.reset();\
}\
object_t& object_t::operator=(IN object_t&& obj)\
{\
    reset();\
    m_impl = obj.m_impl;\
    obj.reset();\
    return *this;\
}


////////////////////////////////////////////////////////////////////////////////
#define ECO_SHARED_PROXY(object_t) \
class object_t::Proxy\
{\
public:\
    inline Proxy(IN bool heap)\
    {\
        if (heap)\
            m_impl.reset(new object_t::Impl);\
    }\
    inline object_t::Impl& impl() const\
    {\
        return *m_impl;\
    }\
    std::shared_ptr<object_t::Impl> m_impl;\
};

#define ECO_SHARED_IMPL__(object_t)\
object_t::Impl& object_t::impl() const\
{\
    return m_proxy->impl();\
}\
object_t::object_t()\
{\
    m_proxy = new Proxy(true);\
    m_proxy->m_impl->init(*this);\
}\
object_t::~object_t()\
{\
    delete m_proxy;\
    m_proxy = nullptr;\
}\
object_t::object_t(IN eco::Null)\
{\
    m_proxy = new Proxy(false);\
}\
object_t::object_t(IN object_t&& obj)\
{\
    m_proxy = new Proxy(false);\
    m_proxy->m_impl = std::move(obj.m_proxy->m_impl);\
}\
object_t::object_t(IN const object_t& obj)\
{\
    m_proxy = new Proxy(false);\
    m_proxy->m_impl = obj.m_proxy->m_impl;\
}\
object_t& object_t::operator=(IN object_t&& obj)\
{\
    m_proxy->m_impl = std::move(obj.m_proxy->m_impl);\
    return *this;\
}\
object_t& object_t::operator=(IN const object_t& obj)\
{\
    m_proxy->m_impl = obj.m_proxy->m_impl;\
    return *this;\
}\
object_t& object_t::operator=(IN eco::Null)\
{\
    reset();\
    return *this;\
}\
object_t& object_t::operator=(IN eco::Heap)\
{\
    m_proxy->m_impl.reset(new Impl);\
    m_proxy->m_impl->init(*this);\
    return *this;\
}\
bool object_t::operator==(IN const object_t& obj) const\
{\
    return m_proxy->m_impl == obj.m_proxy->m_impl;\
}\
bool object_t::operator!=(IN const object_t& obj) const\
{\
    return m_proxy->m_impl != obj.m_proxy->m_impl;\
}\
bool object_t::null() const\
{\
    return (m_proxy->m_impl.get() == nullptr);\
}\
void object_t::swap(IN object_t& obj)\
{\
    m_proxy->m_impl.swap(obj.m_proxy->m_impl);\
}\
void object_t::reset()\
{\
    m_proxy->m_impl.reset();\
}\
void object_t::reserve()\
{\
    if ((m_proxy->m_impl.get() == nullptr)) {\
        m_proxy->m_impl.reset(new Impl);\
        m_proxy->m_impl->init(*this);\
    }\
}

#define ECO_SHARED_IMPL(object_t)\
ECO_SHARED_PROXY(object_t) \
ECO_SHARED_IMPL__(object_t)
///////////////////////////////////////////////////////////////////////////////

NS_BEGIN(eco);
// empty vlaue, using for init.
enum { value_none = 0 };

template<typename ObjectT>
inline void add(ObjectT& obj, uint32_t v)
{
    obj |= v;
}
template<typename ObjectT>
inline void del(ObjectT& obj, uint32_t v)
{
    obj &= ~v;
}
template<typename ObjectT>
inline void toggle(ObjectT& obj, uint32_t v)
{
    obj ^= v;
}
template<typename ObjectT>
inline void set(ObjectT& obj, uint32_t v, bool is)
{
    obj = is ? (obj | v) : (obj & ~v);
}
template<typename ObjectT>
inline void set_v(ObjectT& obj, const uint32_t add_v, const uint32_t del_v)
{
    eco::add(obj, add_v);
    eco::del(obj, del_v);
}
template<typename ObjectT>
inline bool has(const ObjectT& obj, uint32_t v)
{
    return (obj & v) > 0;
}

/*@ export property set. it can be reference in export header, because it's has
only one member "object_t*" which just like export object or value.
* @ iterator: property set iterator.
*/
template<class object_t>
class iterator
{
public:
    // iterator traits.
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef object_t value_type;
    typedef object_t* difference_type;
    typedef object_t* pointer;
    typedef object_t& reference;
    typedef difference_type distance_type;

public:
    explicit iterator(object_t* first_obj = 0) : m_cur(first_obj)
    {}

    inline iterator<object_t> operator++(void)
    {
        return iterator(++m_cur);
    }

    inline iterator<object_t> operator++(int)
    {
        object_t* temp = m_cur;
        ++m_cur;
        return iterator(temp);
    }
    inline iterator<object_t> operator--()
    {
        return iterator(--m_cur);
    }
    inline iterator<object_t> operator--(int)
    {
        object_t* temp = m_cur;
        --m_cur;
        return iterator(temp);
    }
    inline int operator-(const iterator<object_t>& it) const
    {
        return (int)(m_cur - it.m_cur);
    }
    inline object_t* operator->()
    {
        return m_cur;
    }
    inline object_t& operator*()
    {
        return *m_cur;
    }
    inline bool operator==(const iterator<object_t>& it) const
    {
        return m_cur == it.m_cur;
    }
    inline bool operator!=(const iterator<object_t>& it) const
    {
        return m_cur != it.m_cur;
    }

private:
    object_t* m_cur;
};

// object id 1: global object.
typedef uint64_t ObjectId;

// object id 2: attached object.
class AspectId
{
public:
    uint64_t object_id;
    uint32_t aspect_id;
};
NS_END(eco);

#endif