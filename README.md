# Fixed-Income Pricing & Risk Engine

A C++20 library for pricing fixed-income instruments and computing risk analytics. Covers yield curve construction (bootstrapping from deposits and swaps), bond pricing, interest rate swap valuation, duration/convexity metrics, key-rate durations, and scenario analysis.

I built this to understand the actual mechanics behind bond math rather than relying on black-box libraries. The focus was on getting the numerics right -- correct bootstrapping, proper Newton-Raphson convergence for YTM, natural cubic spline interpolation -- and writing the kind of clean, testable code you'd want in a production analytics engine.

The curve module supports bootstrapping from par swap rates or from a combination of money market deposits and swap rates. Interpolation is either cubic spline (for smooth curves) or Nelson-Siegel-Svensson (parametric, useful for fitting published parameters). The risk module computes both analytical duration/convexity from yields and effective (curve-bumped) duration for non-trivial instruments.

Scenario analysis lets you stress the portfolio under parallel shifts, curve twists, and butterfly moves, giving you a full P&L table without needing a separate risk system.

## Building

Requires a C++20 compiler (GCC 11+, Clang 14+, or MSVC 2022).

```
mkdir build && cd build
cmake .. -DFIE_BUILD_TESTS=ON
cmake --build .
ctest
```

## Structure

- `include/fie/` -- public headers
- `src/` -- implementation
- `tests/` -- Google Test unit tests
- `benchmarks/` -- Google Benchmark microbenchmarks (optional)

## Design decisions

Used concepts to constrain template parameters on the discount provider interface. Went with natural cubic splines over monotone-preserving because the smoothness gives better forward rate behaviour. The bootstrapper does sequential solving rather than global calibration because it's simpler and you can see exactly where things break if market data is inconsistent.
