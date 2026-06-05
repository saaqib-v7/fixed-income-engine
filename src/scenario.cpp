#include "fie/scenario.hpp"
#include <cmath>

namespace fie {

YieldCurve apply_parallel_shift(const YieldCurve& curve, Real shift_bps) {
    Real shift = shift_bps / 10000.0;
    auto tenors = curve.tenors();
    auto rates = curve.rates();

    std::vector<Real> shifted(rates.size());
    for (size_t i = 0; i < rates.size(); ++i) {
        shifted[i] = rates[i] + shift;
    }
    return YieldCurve(tenors, shifted);
}

YieldCurve apply_twist(const YieldCurve& curve, Real short_shift_bps,
                       Real long_shift_bps, Tenor pivot_tenor) {
    auto tenors = curve.tenors();
    auto rates = curve.rates();

    std::vector<Real> shifted(rates.size());
    for (size_t i = 0; i < rates.size(); ++i) {
        // Linear interpolation between short and long shift
        Real weight = tenors[i] / pivot_tenor;
        if (weight > 2.0) weight = 2.0;  // cap for very long end
        Real shift_bps = short_shift_bps * (1.0 - weight / 2.0)
                       + long_shift_bps * (weight / 2.0);
        shifted[i] = rates[i] + shift_bps / 10000.0;
    }
    return YieldCurve(tenors, shifted);
}

YieldCurve apply_butterfly(const YieldCurve& curve, Real wing_shift_bps,
                           Real belly_shift_bps) {
    auto tenors = curve.tenors();
    auto rates = curve.rates();
    Real max_tenor = tenors.back();

    std::vector<Real> shifted(rates.size());
    for (size_t i = 0; i < rates.size(); ++i) {
        // Belly is around 5-7 year point, wings are short and long end
        Real normalized = tenors[i] / max_tenor;
        // Parabolic weight: max at wings (0, 1), min at belly (0.5)
        Real wing_weight = 4.0 * (normalized - 0.5) * (normalized - 0.5);
        Real belly_weight = 1.0 - wing_weight;

        Real shift = (wing_shift_bps * wing_weight + belly_shift_bps * belly_weight) / 10000.0;
        shifted[i] = rates[i] + shift;
    }
    return YieldCurve(tenors, shifted);
}

ScenarioResult run_scenario(const BondSpec& bond, const YieldCurve& curve,
                            ScenarioType type, Real shock_bps) {
    Real price_before = price_bond(bond, curve);
    YieldCurve shocked_curve = curve;  // will be replaced

    switch (type) {
        case ScenarioType::ParallelShift:
            shocked_curve = apply_parallel_shift(curve, shock_bps);
            break;
        case ScenarioType::Twist:
            shocked_curve = apply_twist(curve, -shock_bps, shock_bps);
            break;
        case ScenarioType::Butterfly:
            shocked_curve = apply_butterfly(curve, shock_bps, -shock_bps);
            break;
    }

    Real price_after = price_bond(bond, shocked_curve);
    Real pnl = price_after - price_before;
    Real pnl_pct = (price_before != 0.0) ? pnl / price_before * 100.0 : 0.0;

    return ScenarioResult{
        .type = type,
        .shock_size = shock_bps,
        .price_before = price_before,
        .price_after = price_after,
        .pnl = pnl,
        .pnl_percent = pnl_pct
    };
}

std::vector<ScenarioResult> scenario_table(
    const BondSpec& bond, const YieldCurve& curve,
    ScenarioType type,
    const std::vector<Real>& shocks_bps) {

    std::vector<ScenarioResult> results;
    results.reserve(shocks_bps.size());
    for (Real shock : shocks_bps) {
        results.push_back(run_scenario(bond, curve, type, shock));
    }
    return results;
}

}  // namespace fie
