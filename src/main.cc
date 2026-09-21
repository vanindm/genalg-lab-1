#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#define N_DIM 8

const std::string done_sym = "#";
const std::string not_done_sym = "-";

const int r = 120;

int n_called = 0;

double f (const std::array<double, N_DIM> &x) {
    ++n_called;
    double res = (x[0] - 1) * (x[0] - 1);
    for (int i = 1; i < N_DIM; ++i) {
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

std::vector<std::array<double, N_DIM>> tournamentSelection(const std::vector<std::array<double, N_DIM>>& in, std::size_t k, std::mt19937 &random) {
    std::vector<std::array<double, N_DIM>> res;
    res.reserve(in.size());
    std::uniform_int_distribution<std::size_t> pick(0, in.size() - 1);

    for (std::size_t t = 0; t < in.size(); ++t) {
        std::size_t best = pick(random);
        for (std::size_t i = 1; i < k; ++i) {
            std::size_t c = pick(random);
            if (f(in[c]) < f(in[best]))
                best = c;
        }
        res.push_back(in[best]);
    }
    
    return res;
}

std::vector<std::array<double, N_DIM>> recombine(const std::vector<std::array<double, N_DIM>>& in, double p, std::mt19937 &random) {
    std::vector<std::array<double, N_DIM>> res = in;
    std::uniform_real_distribution<double> u(0.0, 1.0);

    for (auto it = res.begin(); it != res.end(); ++(++it)) {
        std::array<double, N_DIM> &p1 = *it;
        std::array<double, N_DIM> &p2 = *(it + 1);
        if (u(random) >= p)
            continue;
        for (std::size_t i = 0; i < N_DIM; ++i) {
            if (u(random) < 0.5)
                std::swap(p1[i], p2[i]);
        }
    }

    return res;
}

std::vector<std::array<double, N_DIM>> mutate(const std::vector<std::array<double, N_DIM>>& in, double p, double sigma, double l, double h, std::mt19937 &random) {
    std::vector<std::array<double, N_DIM>> res = in;
    std::normal_distribution<double> d(0.0, 1.0);
    std::uniform_real_distribution<double> u(0.0, 1.0);
    for (auto &x : res) {
        for (auto &y : x) {
            if (u(random) < p) {
                y += sigma * d(random);
                y = std::clamp(y, l, h);
            }
        }
    }
    return res;
}

void GeneticAlg(int seed, int mu, int lambda, int T, double l, double u, double pCross, double pMut, double sigma, std::ostream &run_output, std::ostream &result_output) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<> d_initial(l, u);
    std::vector<std::array<double, N_DIM>> P(mu);

    std::generate(P.begin(), P.end(), [&d_initial, &rng](){
            std::array<double, N_DIM> a;
            std::generate(a.begin(), a.end(), [&d_initial, &rng](){
                    return d_initial(rng);
                    });
            return a;
        });

    auto itBest = std::max_element(P.begin(), P.end(), [&](const auto &a, const auto &b){return -f(a)<-f(b);});

    std::pair<std::array<double, N_DIM>, double> best = {*itBest, f(*itBest)};

    for (int nGeneration = 1; nGeneration < T; ++nGeneration) {
        auto M = tournamentSelection(P, lambda, rng); 
        auto O = mutate(recombine(M, pCross, rng), pMut, sigma, l, u, rng);
        P = O;
        itBest = std::max_element(P.begin(), P.end(), [&](const auto &a, const auto &b){return -f(a)<-f(b);});
        run_output << nGeneration + 1 << "," << f(*itBest) << "\n";
        if (f(*itBest) < best.second) {
            best = {*itBest, f(*itBest)};
        }
    }
    result_output << best.second << "\n";// << "\n";
    //std::for_each(best.first.begin(), best.first.end(), [&table_output](const auto& a){
    //        table_output << a << ",";
    //        });
}

void RandomSearch(int seed, size_t n_run, double l, double u, std::ostream &result_output) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<> d_initial(l, u);
    std::array<double, N_DIM> a;
    std::for_each(a.begin(), a.end(), [&d_initial, &rng](auto &a){
            a = d_initial(rng);
            });
    std::pair<std::array<double, N_DIM>, double> best = {a, f(a)};
    for (size_t i = 0; i < n_run; ++i) {
        std::for_each(a.begin(), a.end(), [&d_initial, &rng](auto &a){
                a = d_initial(rng);
                });
        if (f(a) < best.second) {
            best = {a, f(a)};
        }
    }
    result_output << best.second << "\n";
    return ;
}

void draw_progress_bar(int i, int n, int w) {
    int p = ((double) (i+1) / (double)n) * (double)w;
    std::stringstream percentage;
    percentage << ((i+1) * 100 / n) << "%";
    std::stringstream done;
    std::stringstream not_done;
    for (int i = 0; i < p; ++i)
        done << done_sym;
    for (int i = 0; i < w-p; ++i)
        not_done << not_done_sym;
    std::cout << "\t[" << done.str() << not_done.str() << "] " << percentage.str();
    std::cout << "\033[" << 1 + w + 2 + percentage.str().size() + 1 << "D";
    fflush(stdout);
}

int main() {
    std::mt19937 seedGen(1000);
    std::uniform_int_distribution<uint32_t> d(1, 100);

    std::ofstream f_runs("1_runs.csv");
    std::ofstream f_results("1_results.csv");

    std::cout << "Calculating first run..." << std::endl;

    for (int i = 0; i < r; ++i) {
        int seed = d(seedGen);
        draw_progress_bar(i, r, 50);
        GeneticAlg(seed, 30, 10, 30, -10.0, 10.0, 0.9, 1.0, 0.75, f_runs, f_results);
    }
    std::cout << std::endl;
    std::cout << "Done!" << std::endl;

    std::ofstream s_runs("2_runs.csv");
    std::ofstream s_results("2_results.csv");
    
    n_called = 0;

    std::cout << "Calculating second run..." << std::endl;

    seedGen = std::mt19937(1000);
    int n_called_delta = n_called;

    for (int i = 0; i < r; ++i) {
        int seed = d(seedGen);
        draw_progress_bar(i, r, 50);
        GeneticAlg(seed, 30, 10, 30, -10.0, 10.0, 0.9, 0.5, 0.75, s_runs, s_results);
        n_called_delta = n_called - n_called_delta;
    }

    std::cout << n_called_delta << "\n";

    std::cout << std::endl;
    std::cout << "Done!" << std::endl;

    std::ofstream b_results("basic_results.csv");
    std::cout << "Calculating basic run..." << std::endl;

    seedGen = std::mt19937(1000);

    for (int i = 0; i < r; ++i) {
        int seed = d(seedGen);
        draw_progress_bar(i, r, 50);
        RandomSearch(seed, n_called_delta, -10.0, 10.0, b_results);
    }

    std::cout << std::endl;
    std::cout << "Done!" << std::endl;

    return 0;
}
