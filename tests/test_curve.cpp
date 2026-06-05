#include <gtest/gtest.h>
#include "fie/curve.hpp"
#include <cmath>

using namespace fie;

class CurveTest : public ::testing::Test {
protected:
    YieldCurve flat_curve;
    YieldCurve upward_curve;

    void SetUp() override {
        // Flat 5% curve
        flat_curve = YieldCurve(
            {1.0, 2.0, 5.0, 10.0, 30.0},
            {0.05, 0.05, 0.05, 0.05, 0.05}
        );
        // Upward sloping
        upward_curve = YieldCurve(
            {1.0, 2.0, 5.0, 10.0, 30.0},
            {0.02, 0.025, 0.03, 0.035, 0.04}
        );
    }
};

TEST_F(CurveTest, FlatCurveDiscountFactors) {
    EXPECT_NEAR(flat_curve.discount(1.0), std::exp(-0.05), 1e-10);
    EXPECT_NEAR(flat_curve.discount(5.0), std::exp(-0.25), 1e-10);
}

TEST_F(CurveTest, DiscountDecreasingInTime) {
    EXPECT_GT(upward_curve.discount(1.0), upward_curve.discount(5.0));
    EXPECT_GT(upward_curve.discount(5.0), upward_curve.discount(10.0));
}

TEST_F(CurveTest, ZeroRateInterpolation) {
    Real rate_3y = upward_curve.zero_rate(3.0);
    // Should be between 2Y and 5Y rates
    EXPECT_GT(rate_3y, 0.025);
    EXPECT_LT(rate_3y, 0.03);
}

TEST_F(CurveTest, ForwardRatePositive) {
    Real fwd = upward_curve.forward_rate(2.0, 5.0);
    EXPECT_GT(fwd, 0.0);
}

TEST(Bootstrap, ParSwapRates) {
    // Bootstrap from par swap rates
    std::vector<Tenor> mats = {1.0, 2.0, 3.0, 5.0, 10.0};
    std::vector<Real> pars = {0.03, 0.035, 0.037, 0.04, 0.045};

    YieldCurve curve = bootstrap_from_swaps(mats, pars);
    // Discount factor at t=0 should be 1
    EXPECT_NEAR(curve.discount(0.0), 1.0, 0.01);
    // All discount factors should be positive
    for (auto t : mats) {
        EXPECT_GT(curve.discount(t), 0.0);
        EXPECT_LT(curve.discount(t), 1.0);
    }
}
