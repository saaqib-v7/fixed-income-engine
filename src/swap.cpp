#include "fie/swap.hpp"
#include <cmath>

namespace fie {

Real swap_annuity(Tenor maturity, const YieldCurve& curve, int frequency) {
    Real dt = 1.0 / frequency;
    int n = static_cast<int>(maturity * frequency);
    Real annuity = 0.0;
    for (int i = 1; i <= n; ++i) {
        annuity += dt * curve.discount(i * dt);
    }
    return annuity;
}

Real par_swap_rate(Tenor maturity, const YieldCurve& curve, int fixed_frequency) {
    // Par rate = (1 - df(T)) / annuity
    Real df_T = curve.discount(maturity);
    Real ann = swap_annuity(maturity, curve, fixed_frequency);
    if (ann <= 0.0) return 0.0;
    return (1.0 - df_T) / ann;
}

Real price_swap(const SwapSpec& swap, const YieldCurve& curve) {
    // Fixed leg PV
    Real fixed_dt = 1.0 / swap.fixed_frequency;
    int n_fixed = static_cast<int>(swap.maturity * swap.fixed_frequency);
    Real fixed_pv = 0.0;
    for (int i = 1; i <= n_fixed; ++i) {
        Real t = i * fixed_dt;
        fixed_pv += swap.fixed_rate * fixed_dt * curve.discount(t);
    }
    fixed_pv *= swap.notional;

    // Float leg PV = notional * (1 - df(T)) for a par float leg
    Real float_pv = swap.notional * (1.0 - curve.discount(swap.maturity));

    // Receiver swap: receive fixed, pay float
    return fixed_pv - float_pv;
}

Real swap_dv01(const SwapSpec& swap, const YieldCurve& curve) {
    // Bump all rates by 1bp and reprice
    auto tenors = curve.tenors();
    auto rates = curve.rates();

    std::vector<Real> rates_up(rates.size());
    std::vector<Real> rates_down(rates.size());
    for (size_t i = 0; i < rates.size(); ++i) {
        rates_up[i] = rates[i] + 0.0001;
        rates_down[i] = rates[i] - 0.0001;
    }

    YieldCurve curve_up(tenors, rates_up);
    YieldCurve curve_down(tenors, rates_down);

    Real pv_up = price_swap(swap, curve_up);
    Real pv_down = price_swap(swap, curve_down);

    return (pv_down - pv_up) / 2.0;
}

}  // namespace fie
