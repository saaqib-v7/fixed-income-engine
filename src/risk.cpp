#include "fie/risk.hpp"
#include <cmath>

namespace fie {

BondRisk compute_bond_risk(const BondSpec& bond, Real ytm) {
    auto flows = bond_cashflows(bond);
    Real price = price_from_yield(bond, ytm);

    Real period_rate = ytm / bond.frequency;
    Real mac_dur = 0.0;
    Real conv = 0.0;

    int n = static_cast<int>(bond.maturity * bond.frequency);
    for (int i = 0; i < n; ++i) {
        Real t = flows[i].time;
        Real pv_cf = flows[i].amount / std::pow(1.0 + period_rate, i + 1);
        mac_dur += t * pv_cf;
        conv += t * (t + 1.0 / bond.frequency) * pv_cf;
    }

    mac_dur /= price;
    conv /= (price * std::pow(1.0 + period_rate, 2));

    Real mod_dur = mac_dur / (1.0 + ytm / bond.frequency);
    Real dv01 = mod_dur * price * 0.0001;  // per 100 face

    return BondRisk{
        .macaulay_duration = mac_dur,
        .modified_duration = mod_dur,
        .convexity = conv,
        .dv01 = dv01
    };
}

std::vector<Real> key_rate_durations(const BondSpec& bond,
                                      const YieldCurve& curve,
                                      Real bump_size) {
    auto tenors = curve.tenors();
    auto rates = curve.rates();
    Real base_price = price_bond(bond, curve);

    std::vector<Real> krd(tenors.size());

    for (size_t k = 0; k < tenors.size(); ++k) {
        // Bump rate at tenor k
        std::vector<Real> bumped_rates = rates;
        bumped_rates[k] += bump_size;

        YieldCurve bumped_curve(tenors, bumped_rates);
        Real bumped_price = price_bond(bond, bumped_curve);

        krd[k] = -(bumped_price - base_price) / (bump_size * base_price);
    }

    return krd;
}

Real effective_duration(const BondSpec& bond, const YieldCurve& curve,
                        Real bump_size) {
    auto tenors = curve.tenors();
    auto rates = curve.rates();

    std::vector<Real> rates_up(rates.size()), rates_down(rates.size());
    for (size_t i = 0; i < rates.size(); ++i) {
        rates_up[i] = rates[i] + bump_size;
        rates_down[i] = rates[i] - bump_size;
    }

    YieldCurve curve_up(tenors, rates_up);
    YieldCurve curve_down(tenors, rates_down);

    Real price_up = price_bond(bond, curve_up);
    Real price_down = price_bond(bond, curve_down);
    Real base_price = price_bond(bond, curve);

    return (price_down - price_up) / (2.0 * bump_size * base_price);
}

Real effective_convexity(const BondSpec& bond, const YieldCurve& curve,
                         Real bump_size) {
    auto tenors = curve.tenors();
    auto rates = curve.rates();

    std::vector<Real> rates_up(rates.size()), rates_down(rates.size());
    for (size_t i = 0; i < rates.size(); ++i) {
        rates_up[i] = rates[i] + bump_size;
        rates_down[i] = rates[i] - bump_size;
    }

    YieldCurve curve_up(tenors, rates_up);
    YieldCurve curve_down(tenors, rates_down);

    Real price_up = price_bond(bond, curve_up);
    Real price_down = price_bond(bond, curve_down);
    Real base_price = price_bond(bond, curve);

    return (price_up + price_down - 2.0 * base_price) / (bump_size * bump_size * base_price);
}

}  // namespace fie
