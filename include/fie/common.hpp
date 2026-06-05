#pragma once

#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <concepts>
#include <span>

namespace fie {

// Basic type aliases
using Real = double;
using Tenor = double;  // in years

struct CashFlow {
    Real amount;
    Tenor time;  // time in years from valuation date
};

// Day count conventions
enum class DayCount {
    ACT_360,
    ACT_365,
    THIRTY_360
};

// Compounding frequency
enum class Compounding {
    Continuous,
    Annual,
    SemiAnnual,
    Quarterly
};

// Concept for anything that can give us a discount factor
template<typename T>
concept DiscountProvider = requires(T t, Tenor tenor) {
    { t.discount(tenor) } -> std::convertible_to<Real>;
};

// Convert between rate and discount factor
inline Real rate_to_discount(Real rate, Tenor t, Compounding comp = Compounding::Continuous) {
    switch (comp) {
        case Compounding::Continuous:
            return std::exp(-rate * t);
        case Compounding::Annual:
            return std::pow(1.0 + rate, -t);
        case Compounding::SemiAnnual:
            return std::pow(1.0 + rate / 2.0, -2.0 * t);
        case Compounding::Quarterly:
            return std::pow(1.0 + rate / 4.0, -4.0 * t);
    }
    return 0.0;
}

inline Real discount_to_rate(Real df, Tenor t, Compounding comp = Compounding::Continuous) {
    if (t <= 0.0 || df <= 0.0) return 0.0;
    switch (comp) {
        case Compounding::Continuous:
            return -std::log(df) / t;
        case Compounding::Annual:
            return std::pow(df, -1.0 / t) - 1.0;
        case Compounding::SemiAnnual:
            return 2.0 * (std::pow(df, -1.0 / (2.0 * t)) - 1.0);
        case Compounding::Quarterly:
            return 4.0 * (std::pow(df, -1.0 / (4.0 * t)) - 1.0);
    }
    return 0.0;
}

}  // namespace fie
