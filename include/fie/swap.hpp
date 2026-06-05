#pragma once

#include "common.hpp"
#include "curve.hpp"

namespace fie {

// Interest rate swap specification
struct SwapSpec {
    Real notional = 1000000.0;
    Real fixed_rate;          // annual fixed rate
    int fixed_frequency = 1;  // fixed leg payments per year (1=annual)
    int float_frequency = 4;  // float leg payments per year (4=quarterly)
    Tenor maturity;           // years
};

// Price a vanilla interest rate swap (receive fixed, pay float)
// Returns NPV from the perspective of the fixed-rate receiver
Real price_swap(const SwapSpec& swap, const YieldCurve& curve);

// Compute par swap rate for a given maturity
// The rate at which the swap has zero NPV
Real par_swap_rate(Tenor maturity, const YieldCurve& curve,
                   int fixed_frequency = 1);

// Swap annuity (PV of fixed leg per unit notional per unit rate)
Real swap_annuity(Tenor maturity, const YieldCurve& curve,
                  int frequency = 1);

// DV01 of a swap (change in NPV per 1bp parallel shift)
Real swap_dv01(const SwapSpec& swap, const YieldCurve& curve);

}  // namespace fie
