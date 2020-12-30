/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: singleton.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_SINGLETON_H__
#define __UTILITY_SINGLETON_H__
#include <stdio.h>
#include <mutex>
#include "nocopyable.h"
#include <memory>
//#include "mutex.h"
namespace utility {
    template <typename T>
    class singleton : public nocopyable
    {
    public:
        static T* create_instance()
        {
            if (m_instance == NULL) {
                m_instance = new T();
            }
            return m_instance;
        }

        static T * get_instance()
        {
            if (m_instance == NULL) {
                printf("m_instance is NULL\n");
            }
            return m_instance;
        }

        static void destory_instance()
        {
            if (m_instance != NULL) {
                delete m_instance;
                m_instance = NULL;
            }
        }

    private:
        static T *m_instance;
    };

    template<typename T>
    T* singleton<T>::m_instance = NULL;

///////////////////////////////////////////////////////////////////////////////
    template <typename T>
    class singleton_meyers : public nocopyable
    {
    public:
        static T& get_instance()
        {
            //scope_mutex locker(&m_mutex_); // not needed after C++0x
            static T instance_;
            return instance_;
        }
    private:
        //static mutex m_mutex_;
    };
    //template <typename T>
    //mutex singleton_meyers<T>::m_mutex_;

///////////////////////////////////////////////////////////////////////////////
    //������main����֮ǰ��ʼ��������û���̰߳�ȫ�����⣬����Ǳ����������no - local static����
    //���������static�����ڲ�ͬ���뵥Ԫ�������Ϊcpp�ļ����������ͷ�ļ����еĳ�ʼ��˳����δ����ġ�
    
    template<typename T>
    class singleton_eager : public nocopyable
    {
    public:
        inline static T& get_instance()
        {
            return instance_;
        }

    private:
        static T instance_;
    };
    template<typename T>
    T singleton_eager<T>::instance_;


///////////////////////////////////////////////////////////////////////////////
    template<typename T>
    class singleton_once : public nocopyable
    {
    public:
        static inline T& get_instance()
        {
            std::call_once(flag_, std::bind(&singleton_once<T>::init));
            return *instance_;
        }

    private:
        static inline void init()
        {
            instance_.reset(new T());
        }

        static std::auto_ptr<T> instance_;
        static std::once_flag flag_;
    };

    template<typename T>
    std::once_flag singleton_once<T>::flag_;
    template<typename T>
    std::auto_ptr<T> singleton_once<T>::instance_;

}

////////////////////////////////////////////////////////////////////////////////
/*@ singleton instance to access singleton object. */
#define SINGLETON(class_type)\
public:\
    inline ~class_type(){}\
    static inline class_type& get_instance()\
{\
    return utility::singleton_meyers<class_type>::get_instance(); \
}\
private:\
    friend class utility::singleton_meyers<class_type>; \
    inline class_type(){}

/*@ singleton instance to access singleton object. */
#define SINGLETON_UNINIT(class_type)\
public:\
    static inline class_type& get_instance()\
{\
    return utility::singleton_meyers<class_type>::get_instance(); \
}\
private:\
    friend class utility::singleton_meyers<class_type>;

////////////////////////////////////////////////////////////////////////////////
/*@ singleton instance to access singleton object. */
#define SINGLETON_ONCE(class_type)\
public:\
    inline ~class_type(){}\
    static inline class_type& get_instance()\
{\
    return utility::singleton_once<class_type>::get_instance(); \
}\
private:\
    friend class utility::singleton_once<class_type>; \
    inline class_type(){}

/*@ singleton instance to access singleton object. */
#define SINGLETON_ONCE_UNINIT(class_type)\
public:\
    static inline class_type& get_instance()\
{\
    return utility::singleton_once<class_type>::get_instance(); \
}\
private:\
    friend class utility::singleton_once<class_type>;


//##############################################################################
/*@ singleton get function to access singleton object.*/
#define SINGLETON_GET(class_type)\
    inline class_type& get_##class_type()\
{\
    return class_type::get_instance(); \
}


#endif

