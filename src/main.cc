#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <random>

#define N_DIM 8

double f (const std::array<double, N_DIM> &x) {
    double res = (x[0] - 1) * (x[0] - 1);
    for (int i = 2; i < N_DIM; ++i) {
        res += i * (2 * x[i] * x[i] - x[i - 1]) * (2 * x[i] * x[i] - x[i - 1]);
    }
    return res;
}

bool check(const std::array<double, N_DIM> &x, double l, double u) {
    for (int i = 0; i < N_DIM; i++) {
        if ((x[i] < l) || x[i] > u) {
            return false;
        }
    }
    return true;
}

std::vector<std::array<double, N_DIM>> tournamentSelection(const std::vector<std::array<double, N_DIM>>& in, int k, std::mt19937 &random) {
    std::uniform_int_distribution<> d(0, in.size());
    d(random);
}

void GeneticAlg(int seed, int mu, int T, double l, double u, double pCross, double pMut) {
    std::mt19937 rng(seed);
    
    for (int nGeneration = 0; nGeneration < T; ++nGeneration) {
    }
}

int main() {
    int seed = 123;
    GeneticAlg(seed, 30, 30, -10, 10, 0.9, 0.0);
    return 0;
}
