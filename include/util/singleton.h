#ifndef singleton_h__
#define singleton_h__
#include "util/util.hpp"  
template<typename T>
class Singleton
{
    public:
        Singleton()
        {
            assert(!ms_pSingleton);
            ms_pSingleton = static_cast<T*>(this);
        }
        ~Singleton()
        {
            assert(ms_pSingleton);
            ms_pSingleton = NULL;
        }

        static T* Instance()
        {
            return ms_pSingleton;
        }

    protected:
        static T* ms_pSingleton;
};

template<typename T>
T* Singleton<T>::ms_pSingleton = NULL;


#endif // !singleton_h__
