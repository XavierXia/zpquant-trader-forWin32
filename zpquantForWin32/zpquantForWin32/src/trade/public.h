#ifndef _COMMONFILES_COMPRETRADESYSTEMHEADERS_PUBLIC_H_
#define _COMMONFILES_COMPRETRADESYSTEMHEADERS_PUBLIC_H_

#define ARCHIVE_FILE_SUFFIX ".sarchive"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#define _WIN32_WINNT 0x0501
#define _WIN32_WINNT 0x0A00
#define FILE_PATH_SEPARATOR "\\"
#define STRATEGY_SUFFIX ".dll"
#define snprintf _snprintf
typedef HINSTANCE StrategyHandleType;
#else
#include <dlfcn.h>
#include <pthread.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <iconv.h>
#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#define FILE_PATH_SEPARATOR "/"
#define STRATEGY_SUFFIX ".so"
typedef void * StrategyHandleType;
#endif

template<int N>
struct PowerOfTwo {
	static const long long val = 2 * PowerOfTwo<N - 1>::val;
};
template<>
struct PowerOfTwo<0> {
	static const long long val = 1;
};

#define _MaskFor1Bit 0x1
#define _MaskFor2Bit 0x3
#define _MaskFor3Bit 0x7
#define _MaskFor4Bit 0xF
#define _MaskFor5Bit 0x1F
#define _MaskFor6Bit 0x3F
#define _MaskFor7Bit 0x7F
#define _MaskFor8Bit 0xFF

#define _StrategyCustomBitCount 3
#define _MaskForStrategyCustom _MaskFor3Bit

#define _StrategyIDBitCount 8
#define _MaskForStrategyID _MaskFor8Bit

#define _OrderDirectionBitCount 1
#define _MaskForOrderDirection _MaskFor1Bit

#define _OrderOffsetBitCount 2
#define _MaskForOrderOffset _MaskFor2Bit

#define _SystemNumberBitCount 3
#define _MaskForSystemNumber _MaskFor3Bit

#define _AccountNumberBitCount 4
#define _MaskForAccountNumber _MaskFor4Bit

#define _MaxStrategyCustom (PowerOfTwo<_StrategyCustomBitCount>::val-1)
#define _MaxStrategyID (PowerOfTwo<_StrategyIDBitCount>::val-1)
#define _MaxOrderDirection (PowerOfTwo<_OrderDirectionBitCount>::val-1)
#define _MaxOrderOffset (PowerOfTwo<_OrderOffsetBitCount>::val-1)
#define _MaxSystemNumber (PowerOfTwo<_SystemNumberBitCount>::val-1)
#define _MaxAccountNumber (PowerOfTwo<_AccountNumberBitCount>::val-1)

#endif