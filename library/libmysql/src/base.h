/*****************************************************************************
 libmysql Copyright (c) 2020. All Rights Reserved.

 FileName: base.h
 Version: 1.0
 Date: 2018.7.2

 History:
 eric     2018.7.2   1.0     Create
 ******************************************************************************/

#ifndef BASE_H_
#define BASE_H_

#include <string.h>
#include <assert.h>
#include <string>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef COMMON_EXPORT
#define COMMON_API  __declspec(dllexport)
#else 
#define COMMON_API  __declspec(dllimport)
#endif
#else//WIN32 END
#define COMMON_API
#endif

#if !defined __GNUC_MINOR__ || defined __INTEL_COMPILER || defined __SUNPRO_C || defined __SUNPRO_CC || defined __llvm__ || defined __clang__ || defined _MSC_VER
  #define base_gcc_version(major,minor) 0
#else
  #define base_gcc_version(major,minor) (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#endif

/* memory fence defines */
#ifdef _MSC_VER
  #include <intrin.h>
  #if _MSC_VER >= 1800 /* VC++ 2013 */
	#define base_memory_fence() std::atomic_thread_fence(std::memory_order_acq_rel)
  #elif _MSC_VER >= 1500 /* VC++ 2008 */
  	/* apparently, microsoft broke all the memory barrier stuff in Visual Studio 2008... */
	#define base_memory_fence()  _ReadWriteBarrier(); MemoryBarrier()
  #elif _MSC_VER >= 1400 /* VC++ 2005 */
	#define base_memory_fence()  _ReadWriteBarrier()
  #elif defined _WIN32
	#include <WinNT.h>
	#define base_memory_fence()  MemoryBarrier() /* actually just xchg on x86... scary */
  #endif
#else
  #ifndef base_memory_fence
    #if base_gcc_version(2,5) || defined __INTEL_COMPILER || (__llvm__ && __GNUC__) || __SUNPRO_C >= 0x5110 || __SUNPRO_CC >= 0x5110
      #if __i386 || __i386__
        #define base_memory_fence()    		__asm__ __volatile__("lock; orb $0, -1(%%esp)" : : : "memory")
        #define base_memory_fence_acq() 	__asm__ __volatile__(""                        : : : "memory")
        #define base_memory_fence_rls() 	__asm__ __volatile__("")
      #elif __amd64 || __amd64__ || __x86_64 || __x86_64__
        #define base_memory_fence()       	__asm__ __volatile__("mfence"   : : : "memory")
        #define base_memory_fence_acq() 	__asm__ __volatile__(""         : : : "memory")
        #define base_memory_fence_rls() 	__asm__ __volatile__("")
      #elif __powerpc__ || __ppc__ || __powerpc64__ || __ppc64__
        #define base_memory_fence()      	__asm__ __volatile__("sync"     : : : "memory")
      #elif defined __ARM_ARCH_6__  || defined __ARM_ARCH_6J__  \
         || defined __ARM_ARCH_6K__ || defined __ARM_ARCH_6ZK__
        #define base_memory_fence()      	__asm__ __volatile__("mcr p15,0,%0,c7,c10,5" : : "r" (0) : "memory")
      #elif defined __ARM_ARCH_7__  || defined __ARM_ARCH_7A__  \
         || defined __ARM_ARCH_7M__ || defined __ARM_ARCH_7R__
        #define base_memory_fence()      	__asm__ __volatile__("dmb"      : : : "memory")
      #elif __aarch64__
        #define base_memory_fence()     	__asm__ __volatile__("dmb ish"  : : : "memory")
      #elif (__sparc || __sparc__) && !__sparcv8
        #define base_memory_fence()       	__asm__ __volatile__("membar #LoadStore | #LoadLoad | #StoreStore | #StoreLoad" : : : "memory")
        #define base_memory_fence_acq() 	__asm__ __volatile__("membar #LoadStore | #LoadLoad"                            : : : "memory")
        #define base_memory_fence_rls() 	__asm__ __volatile__("membar #LoadStore             | #StoreStore")
      #elif defined __s390__ || defined __s390x__
        #define base_memory_fence()      	__asm__ __volatile__("bcr 15,0" : : : "memory")
      #elif defined __mips__
        /* GNU/Linux emulates sync on mips1 architectures, so we force its use */
        /* anybody else who still uses mips1 is supposed to send in their version, with detection code. */
        #define base_memory_fence()    		__asm__ __volatile__(".set mips2; sync; .set mips0" : : : "memory")
      #elif defined __alpha__
        #define base_memory_fence()     	__asm__ __volatile__("mb"       : : : "memory")
      #elif defined __hppa__
        #define base_memory_fence()      	__asm__ __volatile__(""         : : : "memory")
        #define base_memory_fence_rls() 	__asm__ __volatile__("")
      #elif defined __ia64__
        #define base_memory_fence()       	__asm__ __volatile__("mf"       : : : "memory")
      #elif defined __m68k__
        #define base_memory_fence()      	__asm__ __volatile__(""         : : : "memory")
      #elif defined __m88k__
        #define base_memory_fence()     	__asm__ __volatile__("tb1 0,%%r0,128" : : : "memory")
      #elif defined __sh__
        #define base_memory_fence()     	__asm__ __volatile__(""         : : : "memory")
      #endif
    #endif
  #endif

  #ifndef base_memory_fence
	#if base_gcc_version(4,7)
      /* see comment below (stdatomic.h) about the C11 memory model. */
      #define base_memory_fence()       __atomic_thread_fence(__ATOMIC_SEQ_CST)
      #define base_memory_fence_acq() 	__atomic_thread_fence(__ATOMIC_ACQUIRE)
      #define base_memory_fence_rls() 	__atomic_thread_fence(__ATOMIC_RELEASE)
  	#elif ECB_GCC_VERSION(4,4) || defined __INTEL_COMPILER || defined __clang__
      #define base_memory_fence()      	__sync_synchronize()
	#elif __SUNPRO_C >= 0x5110 || __SUNPRO_CC >= 0x5110
      #include <mbarrier.h>
      #define base_memory_fence()       __machine_rw_barrier()
      #define base_memory_fence_acq() 	__machine_r_barrier()
      #define base_memory_fence_rls() 	__machine_w_barrier()
  	#elif __xlC__
      #define base_memory_fence()       __sync ()
    #endif
  #endif
#endif

/* atomic operations defines */
#if defined(_MSC_VER)
  #include <Windows.h>
  #define base_fetch_and_inc(x) InterlockedExchangeAdd((x), 1)
  #define base_fetch_and_dec(x) InterlockedExchangeAdd((x), -1)
  #define base_fetch_and_add(x, c) InterlockedExchangeAdd((x), c)
  #define base_fetch_and_sub(x, c) InterlockedExchangeAdd((x), -(c))
  #define base_compare_and_exchange(des, cmp, exch) InterlockedCompareExchange((des), (exch), (cmp))
  #define base_exchange(des, exch) InterlockedExchange((des), (exch))
#else
  #define base_fetch_and_inc(x) __sync_fetch_and_add((x), 1)
  #define base_fetch_and_dec(x) __sync_fetch_and_sub((x), 1)
  #define base_fetch_and_add(x, c) __sync_fetch_and_add((x), c)
  #define base_fetch_and_sub(x, c) __sync_fetch_and_sub((x), c)
  #define base_compare_and_exchange(des, cmp, exch) __sync_val_compare_and_swap((des), (cmp), (exch))
  #define base_exchange(des, exch) __sync_lock_test_and_set((des), (exch))
#endif

/****************************************************************************/

#define CHECK_LABEL(ret, label) { if (ret != 0) goto label; }
#define GOTO_LABEL(label) { goto label; }
#define BSUCCEEDED(ret) (ret == 0)
#define BFAILED(ret) (ret != 0)
#define ASSIGN_AND_CHECK_LABEL(ret, val, label) { ret = val; if (ret != 0) goto label; }
#define LABEL_SCOPE_START {
#define LABEL_SCOPE_END }
#define SAFE_RELEASE(x) { if (x != NULL) { delete x; x = NULL; } }
#define SAFE_RELEASE_EX(x, op) { if (x != NULL) { x->op(); delete x; x = NULL; } }
#define BMAX(a,b) ((a) > (b) ? (a) : (b))
#define BMIN(a,b) ((a) < (b) ? (a) : (b))
#define BABS(a) ((a) > 0 ? (a) : -(a))
#define FEPSILON 0.000001	/* 1e-6*/
#define DEPSILON 0.000000000000001 /* 1e-15 */
#define IS_FVALUE_EQUAL(a, b) ((BABS(a) >= 1 && BABS((a) - (b)) <= FEPSILON * BABS(a)) || (BABS((a) - (b)) <= FEPSILON))
#define IS_DVALUE_EQUAL(a, b) ((BABS(a) >= 1 && BABS((a) - (b)) <= DEPSILON * BABS(a)) || (BABS((a) - (b)) <= DEPSILON))
#define COMPARE_VALUE(a, b)  ((a) > (b) ? 1 : (a) == (b) ? 0 : -1)
#define COMPARE_FVALUE(a, b) (IS_FVALUE_EQUAL((a), (b)) ? 0 : (a) > (b) ? 1 : -1)
#define COMPARE_DVALUE(a, b) (IS_DVALUE_EQUAL((a), (b)) ? 0 : (a) > (b) ? 1 : -1)

/****************************************************************************/

#endif
