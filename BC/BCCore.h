#ifndef BC_BCCORE_H
#define BC_BCCORE_H

#define $$ (BCObjectRef)

#define val __auto_type

#ifdef WIN32

#define $(__val__) _Generic((__val__), \
	int8_t: BCNumberCreateInt8,        \
	int16_t: BCNumberCreateInt16,      \
	int32_t: BCNumberCreateInt32,      \
	int64_t: BCNumberCreateInt64,      \
	uint8_t: BCNumberCreateUInt8,      \
	uint16_t: BCNumberCreateUInt16,    \
	uint32_t: BCNumberCreateUInt32,    \
	uint64_t: BCNumberCreateUInt64,    \
	float: BCNumberCreateFloat,        \
	double: BCNumberCreateDouble,      \
	bool: BCNumberGetBool,         \
	default: BCNumberCreateInt32       \
)(__val__)

#else

#define $(__val__) _Generic((__val__), \
	int8_t: BCNumberCreateInt8,        \
	int16_t: BCNumberCreateInt16,      \
	int32_t: BCNumberCreateInt32,      \
	long long: BCNumberCreateInt64,    \
	int64_t: BCNumberCreateInt64,      \
	uint8_t: BCNumberCreateUInt8,      \
	uint16_t: BCNumberCreateUInt16,    \
	uint32_t: BCNumberCreateUInt32,    \
	uint64_t: BCNumberCreateUInt64,    \
	float: BCNumberCreateFloat,        \
	double: BCNumberCreateDouble,      \
	bool: BCNumberGetBool,         \
	default: BCNumberCreateInt32       \
)(__val__)

#endif

#endif //BC_BCCORE_H
