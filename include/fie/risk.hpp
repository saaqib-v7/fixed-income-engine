#pragma once

#include "common.hpp"
#include "curve.hpp"
#include "bond.hpp"

namespace fie {

// Risk metrics for a bond
struct BondRisk {
    Real macaulay_duration;
    Real modified_duration;
    Real convexity;
    Real dv01;  // dollar value of 1bp
};

// Compute full risk metrics for a bond given a yield
BondRisk compute_bond_risk(const BondSpec& bond, Real ytm);

// Key-rate duration: sensitivity to a shift at a specific tenor point
// Returns a vector of key-rate durations for each curve tenor
std::vector<Real> key_rate_durations(const BondSpec& bond,
                                      const YieldCurve& curve,
                                      Real bump_size = 0.0001);

// Effective duration (using curve bumping)
Real effective_duration(const BondSpec& bond, const YieldCurve& curve,
                        Real bump_size = 0.0001);

// Effective convexity
Real effective_convexity(const BondSpec& bond, const YieldCurve& curve,
                         Real bump_size = 0.0001);

}  // namespace fie
