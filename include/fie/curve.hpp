#pragma once

#include "common.hpp"
#include "interpolation.hpp"

namespace fie {

// A yield curve bootstrapped from market instruments
// Stores zero rates and provides discount factors via interpolation
class YieldCurve {
public:
    YieldCurve() = default;

    // Build from known zero rates at given tenors
    YieldCurve(std::vector<Tenor> tenors, std::vector<Real> zero_rates);

    // Discount factor for a given tenor
    Real discount(Tenor t) const;

    // Zero rate (continuously compounded) at tenor t
    Real zero_rate(Tenor t) const;

    // Instantaneous forward rate at tenor t
    Real forward_rate(Tenor t) const;

    // Forward rate between t1 and t2
    Real forward_rate(Tenor t1, Tenor t2) const;

    const std::vector<Tenor>& tenors() const { return tenors_; }
    const std::vector<Real>& rates() const { return rates_; }

private:
    std::vector<Tenor> tenors_;
    std::vector<Real> rates_;
    CubicSplineInterpolator interp_;
};

// Bootstrap a yield curve from par swap rates
// Assumes annual fixed payments, continuous compounding for zero rates
YieldCurve bootstrap_from_swaps(
    const std::vector<Tenor>& maturities,
    const std::vector<Real>& par_rates
);

// Bootstrap from deposit rates (short end) and swap rates (long end)
YieldCurve bootstrap_from_market(
    const std::vector<Tenor>& deposit_tenors,
    const std::vector<Real>& deposit_rates,
    const std::vector<Tenor>& swap_tenors,
    const std::vector<Real>& swap_rates
);

}  // namespace fie
