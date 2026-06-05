#include <gtest/gtest.h>
#include "fie/scenario.hpp"
#include <cmath>

using namespace fie;

class ScenarioTest : public ::testing::Test {
protected:
    YieldCurve curve;
    BondSpec bond;
    void SetUp() override {
        curve = YieldCurve({1.0, 2.0, 5.0, 10.0, 30.0}, {0.03, 0.035, 0.04, 0.045, 0.05});
        bond = BondSpec{.face_value = 100.0, .coupon_rate = 0.05, .frequency = 2, .maturity = 10.0};
    }
};

TEST_F(ScenarioTest, ParallelShiftUp) {
    auto result = run_scenario(bond, curve, ScenarioType::ParallelShift, 100.0);
    // Rates up => price down
    EXPECT_LT(result.pnl, 0.0);
}

TEST_F(ScenarioTest, ParallelShiftDown) {
    auto result = run_scenario(bond, curve, ScenarioType::ParallelShift, -100.0);
    // Rates down => price up
    EXPECT_GT(result.pnl, 0.0);
}

TEST_F(ScenarioTest, ZeroShockNoPnL) {
    auto result = run_scenario(bond, curve, ScenarioType::ParallelShift, 0.0);
    EXPECT_NEAR(result.pnl, 0.0, 1e-10);
}

TEST_F(ScenarioTest, ScenarioTableCorrectSize) {
    std::vector<Real> shocks = {-100, -50, -25, 0, 25, 50, 100};
    auto table = scenario_table(bond, curve, ScenarioType::ParallelShift, shocks);
    EXPECT_EQ(table.size(), 7u);
}

TEST_F(ScenarioTest, TwistProducesResult) {
    auto result = run_scenario(bond, curve, ScenarioType::Twist, 50.0);
    EXPECT_NE(result.pnl, 0.0);
}

TEST_F(ScenarioTest, ButterflyProducesResult) {
    auto result = run_scenario(bond, curve, ScenarioType::Butterfly, 25.0);
    EXPECT_NE(result.pnl, 0.0);
}
