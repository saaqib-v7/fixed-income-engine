#include "fie/bond.hpp"
#include <cmath>

namespace fie {

std::vector<CashFlow> bond_cashflows(const BondSpec& bond) {
    std::vector<CashFlow> flows;
    Real coupon_payment = bond.face_value * bond.coupon_rate / bond.frequency;
    Real dt = 1.0 / bond.frequency;
    int n_payments = static_cast<int>(bond.maturity * bond.frequency);

    for (int i = 1; i <= n_payments; ++i) {
        Real t = i * dt;
        Real amount = coupon_payment;
        if (i == n_payments) {
            amount += bond.face_value;  // principal at maturity
        }
        flows.push_back({amount, t});
    }
    return flows;
}

Real price_bond(const BondSpec& bond, const YieldCurve& curve) {
    auto flows = bond_cashflows(bond);
    Real pv = 0.0;
    for (const auto& cf : flows) {
        pv += cf.amount * curve.discount(cf.time);
    }
    return pv;
}

Real price_from_yield(const BondSpec& bond, Real ytm) {
    auto flows = bond_cashflows(bond);
    Real pv = 0.0;
    Real period_rate = ytm / bond.frequency;

    int n = static_cast<int>(bond.maturity * bond.frequency);
    for (int i = 0; i < n; ++i) {
        pv += flows[i].amount / std::pow(1.0 + period_rate, i + 1);
    }
    return pv;
}

Real yield_to_maturity(const BondSpec& bond, Real market_price, Real guess) {
    // Newton-Raphson iteration
    Real ytm = guess;
    constexpr int max_iter = 100;
    constexpr Real tol = 1e-10;

    for (int iter = 0; iter < max_iter; ++iter) {
        Real price = price_from_yield(bond, ytm);
        Real diff = price - market_price;

        if (std::abs(diff) < tol) break;

        // Numerical derivative
        Real h = 0.0001;
        Real price_up = price_from_yield(bond, ytm + h);
        Real deriv = (price_up - price) / h;

        if (std::abs(deriv) < 1e-15) break;

        ytm -= diff / deriv;
    }
    return ytm;
}

}  // namespace fie
