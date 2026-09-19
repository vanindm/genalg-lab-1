#include "random.h"
#include <climits>

MTRandom32::MTRandom32(std::int32_t seed) : gen(seed) {}
MTRandom32::MTRandom32(const std::mt19937 &gen) : gen(gen) {}
MTRandom32::~MTRandom32() {}

std::int32_t MTRandom32::Get() {
    return gen();
}

std::int32_t MTRandom32::operator()() {
    return Get();
}

IRandomGen<std::int32_t> *MTRandom32::Clone() const {
    return new MTRandom32(this->gen);
}

std::int32_t MTRandom32::min() {
    return std::numeric_limits<std::int32_t>::min();
}

std::int32_t MTRandom32::max() {
    return std::numeric_limits<std::int32_t>::max();
}
