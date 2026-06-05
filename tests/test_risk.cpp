#include <gtest/gtest.h>
#include "fie/risk.hpp"
#include <cmath>

using namespace fie;

TEST(BondRisk, DurationPositive) {
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 10.0};
    auto risk = compute_bond_risk(bond, 0.05);
    EXPECT_GT(risk.macaulay_duration, 0.0);
    EXPECT_GT(risk.modified_duration, 0.0);
    EXPECT_LT(risk.macaulay_duration, bond.maturity);
}

TEST(BondRisk, ZeroCouponDurationEqualsMaturity) {
    BondSpec zero{.face_value = 100.0, .coupon_rate = 0.0, .frequency = 1, .maturity = 5.0};
    auto risk = compute_bond_risk(zero, 0.04);
    EXPECT_NEAR(risk.macaulay_duration, 5.0, 0.01);
}

TEST(BondRisk, ConvexityPositive) {
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 10.0};
    auto risk = compute_bond_risk(bond, 0.05);
    EXPECT_GT(risk.convexity, 0.0);
}

TEST(BondRisk, DV01Positive) {
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 10.0};
    auto risk = compute_bond_risk(bond, 0.05);
    EXPECT_GT(risk.dv01, 0.0);
}

TEST(KeyRateDuration, SumsToEffectiveDuration) {
    YieldCurve curve({1.0, 2.0, 5.0, 10.0, 30.0}, {0.03, 0.035, 0.04, 0.045, 0.05});
    BondSpec bond{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 10.0};

    auto krd = key_rate_durations(bond, curve);
    Real sum_krd = 0.0;
    for (auto d : krd) sum_krd += d;

    Real eff_dur = effective_duration(bond, curve);

    // Sum of KRDs should approximately equal effective duration
    EXPECT_NEAR(sum_krd, eff_dur, 0.5);
}
