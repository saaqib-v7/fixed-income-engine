#include <gtest/gtest.h>
#include "fie/interpolation.hpp"
#include <cmath>

using namespace fie;

TEST(LinearInterpolator, ExactPoints) {
    LinearInterpolator interp({1.0, 2.0, 3.0}, {10.0, 20.0, 30.0});
    EXPECT_DOUBLE_EQ(interp(1.0), 10.0);
    EXPECT_DOUBLE_EQ(interp(2.0), 20.0);
    EXPECT_DOUBLE_EQ(interp(3.0), 30.0);
}

TEST(LinearInterpolator, Midpoints) {
    LinearInterpolator interp({0.0, 1.0, 2.0}, {0.0, 10.0, 20.0});
    EXPECT_NEAR(interp(0.5), 5.0, 1e-10);
    EXPECT_NEAR(interp(1.5), 15.0, 1e-10);
}

TEST(LinearInterpolator, Extrapolation) {
    LinearInterpolator interp({1.0, 2.0, 3.0}, {10.0, 20.0, 30.0});
    EXPECT_DOUBLE_EQ(interp(0.0), 10.0);  // flat extrapolation
    EXPECT_DOUBLE_EQ(interp(5.0), 30.0);
}

TEST(CubicSpline, ExactPoints) {
    CubicSplineInterpolator interp({1.0, 2.0, 3.0, 4.0}, {1.0, 4.0, 9.0, 16.0});
    EXPECT_NEAR(interp(1.0), 1.0, 1e-10);
    EXPECT_NEAR(interp(4.0), 16.0, 1e-10);
}

TEST(CubicSpline, SmoothInterpolation) {
    // Quadratic data: spline should interpolate exactly
    std::vector<Real> xs = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<Real> ys;
    for (auto x : xs) ys.push_back(x * x);

    CubicSplineInterpolator interp(xs, ys);
    EXPECT_NEAR(interp(0.5), 0.25, 0.05);
    EXPECT_NEAR(interp(1.5), 2.25, 0.05);
    EXPECT_NEAR(interp(2.5), 6.25, 0.1);
}

TEST(NelsonSiegelSvensson, LongTermLevel) {
    NelsonSiegelSvensson::Parameters params{
        .beta0 = 0.04, .beta1 = -0.02, .beta2 = 0.01,
        .beta3 = 0.005, .tau1 = 1.5, .tau2 = 5.0
    };
    NelsonSiegelSvensson nss(params);
    // At very long maturities, yield should converge to beta0
    EXPECT_NEAR(nss(100.0), 0.04, 0.001);
}

TEST(NelsonSiegelSvensson, ShortRate) {
    NelsonSiegelSvensson::Parameters params{
        .beta0 = 0.04, .beta1 = -0.02, .beta2 = 0.0,
        .beta3 = 0.0, .tau1 = 1.5, .tau2 = 5.0
    };
    NelsonSiegelSvensson nss(params);
    // At t->0, yield = beta0 + beta1
    EXPECT_NEAR(nss(0.001), 0.02, 0.005);
}
