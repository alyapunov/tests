#include <iostream>
#include <alya.h>
#include <set>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>

using namespace std;

const size_t min_len = 1;
const size_t max_len = 20;
const size_t enc_size = (max_len + 6) / 7;
const size_t count = 1024*4;
typedef unsigned long long enc_t;

unsigned char strings[count][max_len];
size_t string_lens[count];

enc_t pre_enc[count][enc_size];

void
fill_strings()
{
	for (size_t i = 0; i < count; i++) {
		size_t len = rand() % (max_len - min_len + 1) + min_len;
		string_lens[i] = len;
		for (size_t j = 0; j < len; j++)
			strings[i][j] = (unsigned char)(rand() % 2 + '0');
	}
}


template<enc_t shift>
enc_t shift_mask(enc_t x)
{
	return (x << (shift * 9 + 1)) | (1ull << (shift * 9));
}

void
encode(unsigned char *str, size_t len, enc_t *res)
{
	size_t i = 0;
	for (; len; i++) {
		size_t n = len < 7 ? len : 7;
		len -= n;
		enc_t tmp = 0, tmp1, tmp2; (void)tmp1; (void)tmp2;
#if 0
		for (size_t j = 0; j < n; j++) {
			size_t x = str[j];
			tmp |= (x << ((6 - j) * 9 + 1)) | (1ull << ((6 - j) * 9));
		}
		str += n;
#elif 1
		switch(n) {
		case 1:
			tmp = shift_mask<6>(*str++);
			break;
		case 2:
			tmp = shift_mask<6>(*str++);
			tmp |= shift_mask<5>(*str++);
			break;
		case 3:
			tmp = shift_mask<6>(*str++);
			tmp |= shift_mask<5>(*str++);
			tmp |= shift_mask<4>(*str++);
			break;
		case 4:
			tmp = shift_mask<6>(*str++);
			tmp |= shift_mask<5>(*str++);
			tmp |= shift_mask<4>(*str++);
			tmp |= shift_mask<3>(*str++);
			break;
		case 5:
			tmp = shift_mask<6>(*str++);
			tmp |= shift_mask<5>(*str++);
			tmp |= shift_mask<4>(*str++);
			tmp |= shift_mask<3>(*str++);
			tmp |= shift_mask<2>(*str++);
			break;
		case 6:
			tmp = shift_mask<6>(*str++);
			tmp |= shift_mask<5>(*str++);
			tmp |= shift_mask<4>(*str++);
			tmp |= shift_mask<3>(*str++);
			tmp |= shift_mask<2>(*str++);
			tmp |= shift_mask<1>(*str++);
			break;
		case 7:
			tmp = shift_mask<6>(*str++);
			tmp |= shift_mask<5>(*str++);
			tmp |= shift_mask<4>(*str++);
			tmp |= shift_mask<3>(*str++);
			tmp |= shift_mask<2>(*str++);
			tmp |= shift_mask<1>(*str++);
			tmp |= shift_mask<0>(*str++);
			break;
		}
#elif 1
		switch(n) {
		case 1:
			tmp = shift_mask<6>(str[0]);
			break;
		case 2:
			tmp = shift_mask<6>(str[0]);
			tmp |= shift_mask<5>(str[1]);
			break;
		case 3:
			tmp = shift_mask<6>(str[0]);
			tmp |= shift_mask<5>(str[1]);
			tmp |= shift_mask<4>(str[2]);
			break;
		case 4:
			tmp = shift_mask<6>(str[0]);
			tmp1 = shift_mask<5>(str[1]);
			tmp |= shift_mask<4>(str[2]);
			tmp1 |= shift_mask<3>(str[3]);
			tmp |= tmp1;
			break;
		case 5:
			tmp = shift_mask<6>(str[0]);
			tmp1 = shift_mask<5>(str[1]);
			tmp |= shift_mask<4>(str[2]);
			tmp1 |= shift_mask<3>(str[3]);
			tmp |= shift_mask<2>(str[4]);
			tmp |= tmp1;
			break;
		case 6:
			tmp = shift_mask<6>(str[0]);
			tmp1 = shift_mask<5>(str[1]);
			tmp2 = shift_mask<4>(str[2]);
			tmp |= shift_mask<3>(str[3]);
			tmp1 |= shift_mask<2>(str[4]);
			tmp2 |= shift_mask<1>(str[5]);
			tmp |= tmp1 | tmp2;
			break;
		case 7:
			tmp = shift_mask<6>(str[0]);
			tmp1 = shift_mask<5>(str[1]);
			tmp2 = shift_mask<4>(str[2]);
			tmp |= shift_mask<3>(str[3]);
			tmp1 |= shift_mask<2>(str[4]);
			tmp2 |= shift_mask<1>(str[5]);
			tmp |= shift_mask<0>(str[6]);
			tmp |= tmp1 | tmp2;
			break;
		}
		str += n;
#else
		switch(n) {
		case 7:
			tmp |= shift_mask<0>(str[6]);
		case 6:
			tmp |= shift_mask<1>(str[5]);
		case 5:
			tmp |= shift_mask<2>(str[4]);
		case 4:
			tmp |= shift_mask<3>(str[3]);
		case 3:
			tmp |= shift_mask<4>(str[2]);
		case 2:
			tmp |= shift_mask<5>(str[1]);
		case 1:
			tmp |= shift_mask<6>(str[0]);
		}
		str += n;
#endif
		*res++ = tmp;
	}
	for (; i < enc_size; i++) {
		*res++ = 0ull;
	}
}

int
enc_comp(enc_t *enc1, enc_t *enc2, size_t len)
{
	for (size_t i = 0; i < len; i++)
		if (enc1[i] < enc2[i])
			return -1;
		else if (enc1[i] > enc2[i])
			return 1;
	return 0;
}

int
comm_comp(unsigned char *s1, size_t len1, unsigned char *s2, size_t len2)
{
	size_t len = len1 < len2 ? len1 : len2;
	int m = memcmp(s1, s2, len);
	if (m < 0)
		return -1;
	else if(m > 0)
		return 1;
	if (len1 < len2)
		return -1;
	else if (len1 > len2)
		return 1;
	else
		return 0;

}

int main(int, const char**)
{
	fill_strings();
	for (size_t i = 0; i < count; i++) {
		enc_t enc1[enc_size], enc2[enc_size];
		encode(strings[i], string_lens[i], enc1);
		encode(strings[i], string_lens[i], pre_enc[i]);
		for (size_t j = 0; j < count; j++) {
			encode(strings[j], string_lens[j], enc2);
			int res1 = enc_comp(enc1, enc2, enc_size);
			int res2 = comm_comp(strings[i], string_lens[i], strings[j], string_lens[j]);
			assert(res1 == res2);
		}
	}

	enc_t probe = 0;
	double t = 0;

	probe = 0;
	t = omp_get_wtime();
	for (size_t k = 0; k < count; k++) {
		for (size_t i = 0; i < count; i++) {
			enc_t enc[enc_size];
			encode(strings[i], string_lens[i], enc);
			probe ^= enc[0];
		}
	}
	t = omp_get_wtime() - t;
	COUT("Just encoding:");
	COUTF(t, count * count, count * count / t, probe);

	probe = 0;
	t = omp_get_wtime();
	for (size_t i = 0; i < count; i++) {
		for (size_t j = i; j < count; j++) {
			probe += comm_comp(strings[i], string_lens[i], strings[j], string_lens[j]) + 1;
		}
	}
	t = omp_get_wtime() - t;
	COUT("Simple comp:");
	COUTF(t, count * count / 2, count * count / 2 / t, probe);

	probe = 0;
	t = omp_get_wtime();
	for (size_t i = 0; i < count; i++) {
		enc_t enc1[enc_size], enc2[enc_size];
		encode(strings[i], string_lens[i], enc1);
		for (size_t j = i; j < count; j++) {
			encode(strings[j], string_lens[j], enc2);
			probe += enc_comp(enc1, enc2, enc_size) + 1;
		}
	}
	t = omp_get_wtime() - t;
	COUT("Encoded comp:");
	COUTF(t, count * count / 2, count * count / 2 / t, probe);

	probe = 0;
	t = omp_get_wtime();
	for (size_t i = 0; i < count; i++) {
		for (size_t j = i; j < count; j++) {
			probe += enc_comp(pre_enc[i], pre_enc[j], enc_size) + 1;
		}
	}
	t = omp_get_wtime() - t;
	COUT("Cheat encoded comp:");
	COUTF(t, count * count / 2, count * count / 2 / t, probe);
}
