#include <gtest/gtest.h>
#include "fie/swap.hpp"
#include <cmath>

using namespace fie;

class SwapTest : public ::testing::Test {
protected:
    YieldCurve curve;
    void SetUp() override {
        curve = YieldCurve(
            {1.0, 2.0, 3.0, 5.0, 10.0},
            {0.03, 0.035, 0.037, 0.04, 0.045}
        );
    }
};

TEST_F(SwapTest, ParSwapHasZeroNPV) {
    Real par_rate = par_swap_rate(5.0, curve);
    SwapSpec swap{.notional = 1000000.0, .fixed_rate = par_rate, .maturity = 5.0};
    Real npv = price_swap(swap, curve);
    EXPECT_NEAR(npv, 0.0, 1.0);  // within $1 of zero
}

TEST_F(SwapTest, AboveParRatePositiveNPV) {
    Real par_rate = par_swap_rate(5.0, curve);
    SwapSpec swap{.notional = 1000000.0, .fixed_rate = par_rate + 0.01, .maturity = 5.0};
    Real npv = price_swap(swap, curve);
    EXPECT_GT(npv, 0.0);  // receiving above-market fixed rate
}

TEST_F(SwapTest, DV01Positive) {
    Real par_rate = par_swap_rate(10.0, curve);
    SwapSpec swap{.notional = 1000000.0, .fixed_rate = par_rate, .maturity = 10.0};
    Real dv01 = swap_dv01(swap, curve);
    EXPECT_GT(dv01, 0.0);
}

TEST_F(SwapTest, AnnuityPositive) {
    Real ann = swap_annuity(5.0, curve);
    EXPECT_GT(ann, 0.0);
    EXPECT_LT(ann, 5.0);  // less than sum of undiscounted year fractions
}
