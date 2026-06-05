#pragma once

#include "common.hpp"
#include "curve.hpp"
#include "bond.hpp"

namespace fie {

// Scenario types
enum class ScenarioType {
    ParallelShift,   // uniform shift across all tenors
    Twist,           // short end moves opposite to long end
    Butterfly        // wings move opposite to belly
};

// Result of scenario analysis
struct ScenarioResult {
    ScenarioType type;
    Real shock_size;      // in basis points
    Real price_before;
    Real price_after;
    Real pnl;            // price_after - price_before
    Real pnl_percent;    // percentage change
};

// Apply a parallel shift to the curve
YieldCurve apply_parallel_shift(const YieldCurve& curve, Real shift_bps);

// Apply a twist (short end up, long end down, or vice versa)
// pivot_tenor: tenor at which shift is zero
YieldCurve apply_twist(const YieldCurve& curve, Real short_shift_bps,
                       Real long_shift_bps, Tenor pivot_tenor = 5.0);

// Apply a butterfly (wings shift one way, belly shifts the other)
YieldCurve apply_butterfly(const YieldCurve& curve, Real wing_shift_bps,
                           Real belly_shift_bps);

// Run scenario analysis on a bond
ScenarioResult run_scenario(const BondSpec& bond, const YieldCurve& curve,
                            ScenarioType type, Real shock_bps);

// Run a full scenario table across multiple shocks
std::vector<ScenarioResult> scenario_table(
    const BondSpec& bond, const YieldCurve& curve,
    ScenarioType type,
    const std::vector<Real>& shocks_bps);

}  // namespace fie
