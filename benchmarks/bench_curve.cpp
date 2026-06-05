#include <benchmark/benchmark.h>
#include "fie/curve.hpp"
#include "fie/bond.hpp"
#include "fie/risk.hpp"

using namespace fie;

static void BM_CurveBootstrap(benchmark::State& state) {
    std::vector<Tenor> mats = {1, 2, 3, 5, 7, 10, 15, 20, 30};
    std::vector<Real> pars = {0.03, 0.035, 0.037, 0.04, 0.042, 0.045, 0.047, 0.048, 0.05};
    for (auto _ : state) {
        auto curve = bootstrap_from_swaps(mats, pars);
        benchmark::DoNotOptimize(curve);
    }
}
BENCHMARK(BM_CurveBootstrap);

static void BM_BondPricing(benchmark::State& state) {
    auto curve = YieldCurve({1, 2, 5, 10, 30}, {0.03, 0.035, 0.04, 0.045, 0.05});
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 10.0};
    for (auto _ : state) {
        Real p = price_bond(bond, curve);
        benchmark::DoNotOptimize(p);
    }
}
BENCHMARK(BM_BondPricing);

static void BM_KeyRateDurations(benchmark::State& state) {
    auto curve = YieldCurve({1, 2, 5, 10, 30}, {0.03, 0.035, 0.04, 0.045, 0.05});
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 10.0};
    for (auto _ : state) {
        auto krd = key_rate_durations(bond, curve);
        benchmark::DoNotOptimize(krd);
    }
}
BENCHMARK(BM_KeyRateDurations);

BENCHMARK_MAIN();
