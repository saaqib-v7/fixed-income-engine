#pragma once

#include "common.hpp"
#include <cassert>

namespace fie {

// Linear interpolation on a sorted set of (x, y) pairs
class LinearInterpolator {
public:
    LinearInterpolator() = default;
    LinearInterpolator(std::vector<Real> xs, std::vector<Real> ys);

    Real operator()(Real x) const;
    Real derivative(Real x) const;

    const std::vector<Real>& x_values() const { return xs_; }
    const std::vector<Real>& y_values() const { return ys_; }

private:
    std::vector<Real> xs_;
    std::vector<Real> ys_;
};

// Cubic spline interpolation with natural boundary conditions
class CubicSplineInterpolator {
public:
    CubicSplineInterpolator() = default;
    CubicSplineInterpolator(std::vector<Real> xs, std::vector<Real> ys);

    Real operator()(Real x) const;
    Real derivative(Real x) const;

private:
    void compute_coefficients();

    std::vector<Real> xs_;
    std::vector<Real> ys_;
    std::vector<Real> a_, b_, c_, d_;  // spline coefficients
};

// Nelson-Siegel-Svensson parametric model for yield curves
// y(t) = b0 + b1*((1-exp(-t/tau1))/(t/tau1))
//       + b2*((1-exp(-t/tau1))/(t/tau1) - exp(-t/tau1))
//       + b3*((1-exp(-t/tau2))/(t/tau2) - exp(-t/tau2))
class NelsonSiegelSvensson {
public:
    struct Parameters {
        Real beta0;   // long-term level
        Real beta1;   // short-term component
        Real beta2;   // medium-term hump
        Real beta3;   // second hump (Svensson extension)
        Real tau1;    // decay factor 1
        Real tau2;    // decay factor 2
    };

    NelsonSiegelSvensson() = default;
    explicit NelsonSiegelSvensson(Parameters params);

    Real operator()(Tenor t) const;
    Real forward_rate(Tenor t) const;

    const Parameters& params() const { return params_; }

private:
    Parameters params_{};
};

}  // namespace fie
