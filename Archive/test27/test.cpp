#include <alya.h>

#include <cstring>

constexpr uint32_t ALIGN_BITS = 2;
constexpr uint32_t RND_LOSS = (1 << ALIGN_BITS) - 1;
constexpr uint32_t EFF_BITS = 4;
constexpr uint32_t EFF_SIZE = 1 << EFF_BITS;

uint32_t getClassIf(uint32_t size)
{
	assert(size > 1);
	size--;
	if (size < (EFF_SIZE << ALIGN_BITS) * 2) // 2!
		return size >> ALIGN_BITS;

	uint32_t hi = 31 ^ __builtin_clz(size);
	assert(hi >= EFF_BITS + ALIGN_BITS);
	hi -= EFF_BITS;
	size >>= hi;
	assert(size >= 1 * EFF_SIZE);
	assert(size < 2 * EFF_SIZE);
	return size + EFF_SIZE * (hi - ALIGN_BITS);
}

uint32_t getClass(uint32_t size)
{
	assert(size > 1);
	size--;
	uint32_t hi = 31 ^ __builtin_clz(size | (1u << (EFF_BITS + ALIGN_BITS)));
	hi -= EFF_BITS;
	size >>= hi;
	return size + EFF_SIZE * (hi - ALIGN_BITS);
}

uint32_t getClass0(uint32_t size)
{
	assert(size > 1);
	size--;
	size >>= ALIGN_BITS;
	if (size < EFF_SIZE)
		return size;
	uint32_t hi = 31 ^ __builtin_clz(size);
	assert(hi >= EFF_BITS);
	hi -= EFF_BITS;
	size >>= hi;
	size %= EFF_SIZE;
	return size + EFF_SIZE * (hi + 1);
}

std::string fmt(uint32_t i) {
	std::string res = std::to_string(i);
	while (res.size() < 6) {
		res = " " + res;
	}
	return res;
}

uint32_t getSize(uint32_t sizeClass)
{
	sizeClass += 1;
	if (sizeClass < EFF_SIZE) {
		return sizeClass << ALIGN_BITS;
	}
	sizeClass -= EFF_SIZE;
	return (EFF_SIZE + sizeClass % EFF_SIZE) << (sizeClass / EFF_SIZE + ALIGN_BITS);
}

int main()
{
	for (uint32_t i = 0; i < 6; i++) {
		for (uint32_t j = 0; j < 16; j++) {
			std::cout << fmt(getSize(i * 16 + j));
		}
		std::cout << std::endl;
	}

	double ave_perc_loss = 0;
	size_t ave_perc_loss_n = 0;
	for (uint32_t i = 2; i < 530; i++) {
		uint32_t cl0 = getClass0(i);
		uint32_t cl = getClass(i);
		uint32_t top = getSize(cl);
		uint32_t loss = top - i;
		double perc_loss = loss  * 100;
		perc_loss /= i;
		COUTF(i, cl, cl0, top, loss, perc_loss);
		assert(cl == cl0);
		assert(i <= top);
		if (i >= 16) {
			ave_perc_loss += perc_loss;
			ave_perc_loss_n++;
		}
	}
	ave_perc_loss /= ave_perc_loss_n;
	COUTF(ave_perc_loss);

	return 0;
}
