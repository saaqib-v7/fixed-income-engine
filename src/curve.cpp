#include "fie/curve.hpp"
#include <cmath>
#include <algorithm>

namespace fie {

YieldCurve::YieldCurve(std::vector<Tenor> tenors, std::vector<Real> zero_rates)
    : tenors_(std::move(tenors)), rates_(std::move(zero_rates)),
      interp_(tenors_, rates_) {}

Real YieldCurve::discount(Tenor t) const {
    return std::exp(-zero_rate(t) * t);
}

Real YieldCurve::zero_rate(Tenor t) const {
    if (t <= 0.0) return rates_.front();
    return interp_(t);
}

Real YieldCurve::forward_rate(Tenor t) const {
    Real h = 0.001;
    Real r_t = zero_rate(t);
    Real r_th = zero_rate(t + h);
    // f(t) = d/dt [r(t)*t] = r(t) + t * r'(t)
    return r_t + t * (r_th - r_t) / h;
}

Real YieldCurve::forward_rate(Tenor t1, Tenor t2) const {
    if (t2 <= t1) return zero_rate(t1);
    Real df1 = discount(t1);
    Real df2 = discount(t2);
    return std::log(df1 / df2) / (t2 - t1);
}

YieldCurve bootstrap_from_swaps(
    const std::vector<Tenor>& maturities,
    const std::vector<Real>& par_rates) {

    size_t n = maturities.size();
    std::vector<Real> zero_rates(n);
    std::vector<Real> dfs(n);

    for (size_t i = 0; i < n; ++i) {
        Real coupon = par_rates[i];
        Real sum_df = 0.0;
        for (size_t j = 0; j < i; ++j) {
            sum_df += dfs[j];
        }
        // par bond: 1 = c * sum(df_j) + (1 + c) * df_i
        // => df_i = (1 - c * sum_df) / (1 + c)
        dfs[i] = (1.0 - coupon * sum_df) / (1.0 + coupon);
        zero_rates[i] = -std::log(dfs[i]) / maturities[i];
    }

    return YieldCurve(maturities, zero_rates);
}

YieldCurve bootstrap_from_market(
    const std::vector<Tenor>& deposit_tenors,
    const std::vector<Real>& deposit_rates,
    const std::vector<Tenor>& swap_tenors,
    const std::vector<Real>& swap_rates) {

    // Combine deposits (simple zero rates) with bootstrapped swap curve
    std::vector<Tenor> all_tenors;
    std::vector<Real> all_rates;

    // Deposits: rate is already a zero rate (approximately)
    for (size_t i = 0; i < deposit_tenors.size(); ++i) {
        all_tenors.push_back(deposit_tenors[i]);
        // Convert money market rate to continuous zero rate
        Real df = 1.0 / (1.0 + deposit_rates[i] * deposit_tenors[i]);
        all_rates.push_back(-std::log(df) / deposit_tenors[i]);
    }

    // Bootstrap swaps
    std::vector<Real> dfs;
    for (auto& t : all_tenors) {
        dfs.push_back(std::exp(-all_rates.back() * t));
    }

    for (size_t i = 0; i < swap_tenors.size(); ++i) {
        Real c = swap_rates[i];
        Real sum_df = 0.0;
        // Use all previously computed discount factors for annual payments
        for (size_t j = 0; j < all_tenors.size(); ++j) {
            sum_df += std::exp(-all_rates[j] * all_tenors[j]);
        }
        Real df_new = (1.0 - c * sum_df) / (1.0 + c);
        Real zr = -std::log(df_new) / swap_tenors[i];

        all_tenors.push_back(swap_tenors[i]);
        all_rates.push_back(zr);
    }

    return YieldCurve(all_tenors, all_rates);
}

}  // namespace fie
