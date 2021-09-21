#include <iostream>
#include <alya.h>

#define MP_SOURCE
#include <msgpuck.h>
#include <stdio.h>
#include <inttypes.h>
#include <msgprint.h>

using namespace std;
using namespace alya;

template <int ValueCategory, uint64_t Value>
struct CUintCore;

template <uint64_t Value>
struct CUintCore<0, Value> {
	uint8_t value;
	CUintCore() : value(Value) {}
} __attribute__((packed));

template <uint64_t Value>
struct CUintCore<1, Value> {
	uint8_t prefix;
	uint8_t value;
	CUintCore() : prefix(0xcc), value(Value) {}
} __attribute__((packed));

template <uint64_t Value>
struct CUintCore<2, Value> {
	uint8_t prefix;
	uint16_t value;
	CUintCore() : prefix(0xcd), value(mp_bswap_u16(Value)) {}
} __attribute__((packed));

template <uint64_t Value>
struct CUintCore<3, Value> {
	uint8_t prefix;
	uint32_t value;
	CUintCore() : prefix(0xce), value(mp_bswap_u32(Value)) {}
} __attribute__((packed));

template <uint64_t Value>
struct CUintCore<4, Value> {
	uint8_t prefix;
	uint64_t value;
	CUintCore() : prefix(0xcf), value(mp_bswap_u64(Value)) {}
} __attribute__((packed));

template <uint64_t Value>
struct CUint {
	enum { ValueCategory = Value <= 0x7Fu ? 0 : Value <= 0xFFu ? 1 : Value <= 0xFFFFu ? 2 : Value <= 0xFFFFFFFFu ? 3 : 4 };
	CUintCore<ValueCategory, Value> core;
	const char *Data() const { return (char *)this; }
} __attribute__((packed));

//struct

template <class T>
struct CMapEncoderEntry;

template <>
struct CMapEncoderEntry<uint32_t> {
	char MapKey;
	char MapValPrefix;
	uint32_t MapValue;
	CMapEncoderEntry(char Key, uint32_t Val) : MapKey(Key), MapValPrefix(0xce), MapValue(mp_bswap_u32(Val)) {}
} __attribute__((packed));

template <>
struct CMapEncoderEntry<uint64_t> {
	char MapKey;
	char MapValPrefix;
	uint64_t MapValue;
	CMapEncoderEntry(char Key, uint32_t Val) : MapKey(Key), MapValPrefix(0xcf), MapValue(mp_bswap_u64(Val)) {}
} __attribute__((packed));

template <>
struct CMapEncoderEntry<double> {
	char MapKey;
	char MapValPrefix;
	double MapValue;
	CMapEncoderEntry(char Key, double Val) : MapKey(Key), MapValPrefix(0xcb), MapValue(mp_bswap_double(Val)) {}
} __attribute__((packed));

template <class ... Other>
struct CMapEncoderEntries;

template <class T, class ... Other>
struct CMapEncoderEntries<T, Other...> {
	CMapEncoderEntry<T> Entry;
	CMapEncoderEntries<Other...> Next;
	template <class ... OtherPairs>
	CMapEncoderEntries(char Key, T Val, OtherPairs... other) : Entry(Key, Val), Next(other...) {}
} __attribute__((packed));

template <class T>
struct CMapEncoderEntries<T> {
	CMapEncoderEntry<T> Entry;
	CMapEncoderEntries(char Key, T Val) : Entry(Key, Val) {}
} __attribute__((packed));

template <class ... Other>
struct CEntriesCounter;

template <class T, class ... Other>
struct CEntriesCounter<T, Other...> {
	enum { Count = 1 + CEntriesCounter<Other...>::Count };
};

template <class T>
struct CEntriesCounter<T> {
	enum { Count = 1 };
};

template <class ... Other>
struct CMapEncoder {
	char Count;
	CMapEncoderEntries<Other...> Entries;
	template <class ... OtherPairs>
	CMapEncoder(OtherPairs... other) : Count(0x80 | (char) CEntriesCounter<Other...>::Count), Entries(other...) {}
	const char *GetBuffer() const { return (const char *)this; }
} __attribute__((packed));

int main(int, const char**)
{
	//const char *t = "\225\002\314\322\315\002X\316V%\030\060\002";
	//mp_print(t);
	//return 0;
	COUTF(sizeof(CUint<0>));
	COUTF(sizeof(CUint<127>));
	COUTF(sizeof(CUint<128>));
	COUTF(sizeof(CUint<65535>));
	COUTF(sizeof(CUint<65536>));
	COUTF(sizeof(CUint<6000000000ull>));

	mp_print(CUint<0>().Data());
	mp_print(CUint<127>().Data());
	mp_print(CUint<128>().Data());
	mp_print(CUint<65535>().Data());
	mp_print(CUint<65536>().Data());
	mp_print(CUint<6000000000ull>().Data());

}
