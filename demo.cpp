#include <iostream>
#include <iomanip>
#include "fie/common.hpp"
#include "fie/interpolation.hpp"
#include "fie/curve.hpp"
#include "fie/bond.hpp"
#include "fie/swap.hpp"
#include "fie/risk.hpp"
#include "fie/scenario.hpp"

int main() {
    using namespace fie;
    std::cout << std::fixed << std::setprecision(6);

    // 1. Bootstrap yield curve from par swap rates
    std::vector<Tenor> mats = {1.0, 2.0, 3.0, 5.0, 7.0, 10.0};
    std::vector<Real> pars = {0.035, 0.038, 0.040, 0.043, 0.045, 0.048};
    auto curve = bootstrap_from_swaps(mats, pars);

    std::cout << "=== Yield Curve ===" << std::endl;
    for (auto t : mats) {
        std::cout << "  " << t << "Y: zero=" << curve.zero_rate(t)*100 << "%, df=" << curve.discount(t) << std::endl;
    }

    // 2. Price a 10Y 5% semi-annual bond
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 10.0};
    Real price = price_bond(bond, curve);
    Real ytm = yield_to_maturity(bond, price);
    std::cout << "\n=== Bond Pricing ===" << std::endl;
    std::cout << "  10Y 5% semi-annual bond" << std::endl;
    std::cout << "  Price: " << price << std::endl;
    std::cout << "  YTM:   " << ytm * 100 << "%" << std::endl;

    // 3. Risk metrics
    auto risk = compute_bond_risk(bond, ytm);
    std::cout << "\n=== Risk Metrics ===" << std::endl;
    std::cout << "  Macaulay Duration: " << risk.macaulay_duration << std::endl;
    std::cout << "  Modified Duration: " << risk.modified_duration << std::endl;
    std::cout << "  Convexity:         " << risk.convexity << std::endl;
    std::cout << "  DV01:              " << risk.dv01 << std::endl;

    // 4. Par swap rate
    Real par10 = par_swap_rate(10.0, curve);
    std::cout << "\n=== Swap ===" << std::endl;
    std::cout << "  10Y par swap rate: " << par10 * 100 << "%" << std::endl;

    // 5. Scenario analysis
    std::cout << "\n=== Scenario Analysis (Parallel Shift) ===" << std::endl;
    std::vector<Real> shocks = {-100, -50, 0, 50, 100};
    for (auto s : shocks) {
        auto result = run_scenario(bond, curve, ScenarioType::ParallelShift, s);
        std::cout << "  " << std::setw(5) << s << "bp: price=" << result.price_after
                  << " P&L=" << result.pnl << " (" << result.pnl_percent << "%)" << std::endl;
    }

    std::cout << "\nAll OK." << std::endl;
    return 0;
}
