#pragma once

#include "common.hpp"
#include "curve.hpp"

namespace fie {

// Fixed-rate bond specification
struct BondSpec {
    Real face_value = 100.0;
    Real coupon_rate;           // annual coupon as decimal (e.g., 0.05 for 5%)
    int frequency = 2;          // payments per year (1=annual, 2=semi, 4=quarterly)
    Tenor maturity;             // years to maturity
};

// Bond pricing results
struct BondPrice {
    Real clean_price;
    Real dirty_price;
    Real accrued_interest;
    Real yield_to_maturity;
};

// Price a bond given a discount curve
Real price_bond(const BondSpec& bond, const YieldCurve& curve);

// Generate cash flows for a bond
std::vector<CashFlow> bond_cashflows(const BondSpec& bond);

// Compute yield to maturity given a market price
// Uses Newton-Raphson iteration
Real yield_to_maturity(const BondSpec& bond, Real market_price, Real guess = 0.05);

// Price from yield (flat curve pricing)
Real price_from_yield(const BondSpec& bond, Real ytm);

}  // namespace fie
