#pragma once

#define RequiemBuild
#define LevelersBuild
//#define DuelBuild
//#define AlchemySorter

#define ClassFactory(smartPT, className, ...) \
	smartPT.reset(new className(__VA_ARGS__)); \
	smartPT->afterConstruction()

#define CALL_MEMBER_FN_OFFSET(obj, fn) ((*(obj)).*(*(_##fn##_GetPtr(obj))))

#define DEFINE_MEMBER_FN_OFFSET(className, functionName, retnType, offset, ...)		\
	typedef retnType (className::* _##functionName##_type)(__VA_ARGS__);			\
	\
	inline _##functionName##_type * _##functionName##_GetPtr(void* obj)			\
	{																			\
	static const UInt32 _offset = offset;										\
	UInt32 _address = *(UInt32*)obj;											\
	_address += _offset;														\
	return (_##functionName##_type *)_address;									\
	}

#define CALL_MEMBER_FN_EX(obj, fn) ((*(obj)).*(*(_##fn##_GetPtr())))

#define RandomFloat(LO, HI) LO+static_cast<float>(rand())/(static_cast <float>(RAND_MAX)/(HI-LO))

#ifdef _DEBUG
#ifdef _MSC_VER
#include <iostream>
#include <iomanip>
void OpenConsole();
#endif        // or std::cerr << (x) << std::flush
#endif

#include <string>
bool SubstringEqual(const char* string, const char* sub_string);
SInt32 Substring(const char* string, const char* sub_string);
UInt32 log2(UInt32 in);
std::string format(const char *fmt, ...);
