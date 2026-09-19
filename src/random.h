#pragma once

#include <cstdint>
#include <random>
#include <cmath>

/* **
 * @class IRandomGen
 * @brief Interface of random generator
 */
template<typename T>
class IRandomGen {
public:
    virtual T Get() = 0;
    virtual IRandomGen* Clone() const = 0;
    virtual ~IRandomGen() = default;
    virtual T operator()() = 0;
};

/* **
 * @class RandomGenDouble
 * @brief Wraps T IRandomGen implementation, which returns type U - sizeof(U) bit integer, so it would return a random double in [-1.0, 1.0] or [0.0, 1.0] (U signed or unsigned).
 */
template<typename T, typename U>
class RandomGenDouble : public IRandomGen<double>{
    T *gen;
public:
    /* **
     * @brief Constructs a wrapper over clone of T, which returns random U type.
     */
    RandomGenDouble(const T& gen) {
        this->gen = gen.Clone();
    }
    RandomGenDouble(T* gen) {
        this->gen = gen;
    }
    ~RandomGenDouble() {
        delete gen;
    }
    virtual IRandomGen<double>* Clone() const {
        return new RandomGenDouble(*gen);
    }
    virtual double Get() {
        return (double)gen->Get() / pown(2, sizeof(U)*8);
    }
    virtual double operator()() {
        return Get();
    }
    static double min() {
        if constexpr(std::is_signed_v<U>)
            return -1.0;
        else
            return 0.0;
    }
    static double max() {
        return 1.0;
    }
};

/* **
 * @class MTRandom32
 * @brief Implementation of random generator using mt1993
 */
class MTRandom32 : public IRandomGen<std::int32_t> {
    std::mt19937 gen;
public:
    MTRandom32(std::int32_t seed);
    MTRandom32(const std::mt19937 &gen);
    virtual std::int32_t Get();
    virtual std::int32_t operator()();
    virtual IRandomGen<std::int32_t>* Clone() const;
    ~MTRandom32();
    virtual std::int32_t min();
    virtual std::int32_t max();
};
