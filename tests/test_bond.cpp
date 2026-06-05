#include <gtest/gtest.h>
#include "fie/bond.hpp"
#include <cmath>

using namespace fie;

TEST(Bond, ParBondPricedAtPar) {
    // A bond with coupon = yield should price at par
    YieldCurve curve({1.0, 2.0, 5.0, 10.0}, {0.05, 0.05, 0.05, 0.05});
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 1, .maturity = 5.0};
    Real price = price_from_yield(bond, 0.05);
    EXPECT_NEAR(price, 100.0, 0.5);  // approximately par
}

TEST(Bond, HigherYieldLowerPrice) {
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 10.0};
    Real price_low = price_from_yield(bond, 0.03);
    Real price_high = price_from_yield(bond, 0.07);
    EXPECT_GT(price_low, price_high);
}

TEST(Bond, YTMRoundTrip) {
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.06, .frequency = 2, .maturity = 10.0};
    Real target_ytm = 0.055;
    Real price = price_from_yield(bond, target_ytm);
    Real recovered_ytm = yield_to_maturity(bond, price, 0.05);
    EXPECT_NEAR(recovered_ytm, target_ytm, 1e-6);
}

TEST(Bond, CashflowCount) {
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 5.0};
    auto flows = bond_cashflows(bond);
    EXPECT_EQ(flows.size(), 10u);  // 5 years * 2 payments/year
}

TEST(Bond, LastCashflowIncludesPrincipal) {
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.04, .frequency = 1, .maturity = 3.0};
    auto flows = bond_cashflows(bond);
    // Last flow should be coupon + principal = 4 + 100 = 104
    EXPECT_NEAR(flows.back().amount, 104.0, 1e-10);
}
