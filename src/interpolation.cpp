#include "fie/interpolation.hpp"
#include <algorithm>
#include <cmath>

namespace fie {

// --- LinearInterpolator ---

LinearInterpolator::LinearInterpolator(std::vector<Real> xs, std::vector<Real> ys)
    : xs_(std::move(xs)), ys_(std::move(ys)) {
    if (xs_.size() != ys_.size() || xs_.size() < 2) {
        throw std::invalid_argument("Need at least 2 points for interpolation");
    }
}

Real LinearInterpolator::operator()(Real x) const {
    if (x <= xs_.front()) return ys_.front();
    if (x >= xs_.back()) return ys_.back();

    auto it = std::lower_bound(xs_.begin(), xs_.end(), x);
    size_t i = std::distance(xs_.begin(), it) - 1;

    Real t = (x - xs_[i]) / (xs_[i + 1] - xs_[i]);
    return ys_[i] + t * (ys_[i + 1] - ys_[i]);
}

Real LinearInterpolator::derivative(Real x) const {
    if (x <= xs_.front() || x >= xs_.back()) return 0.0;

    auto it = std::lower_bound(xs_.begin(), xs_.end(), x);
    size_t i = std::distance(xs_.begin(), it) - 1;

    return (ys_[i + 1] - ys_[i]) / (xs_[i + 1] - xs_[i]);
}

// --- CubicSplineInterpolator ---

CubicSplineInterpolator::CubicSplineInterpolator(std::vector<Real> xs, std::vector<Real> ys)
    : xs_(std::move(xs)), ys_(std::move(ys)) {
    if (xs_.size() != ys_.size() || xs_.size() < 3) {
        throw std::invalid_argument("Need at least 3 points for cubic spline");
    }
    compute_coefficients();
}

void CubicSplineInterpolator::compute_coefficients() {
    size_t n = xs_.size() - 1;
    std::vector<Real> h(n), alpha(n);

    for (size_t i = 0; i < n; ++i) {
        h[i] = xs_[i + 1] - xs_[i];
    }
    for (size_t i = 1; i < n; ++i) {
        alpha[i] = 3.0 / h[i] * (ys_[i + 1] - ys_[i])
                 - 3.0 / h[i - 1] * (ys_[i] - ys_[i - 1]);
    }

    // Tridiagonal system for natural spline (c[0] = c[n] = 0)
    c_.resize(n + 1, 0.0);
    std::vector<Real> l(n + 1), mu(n + 1), z(n + 1);
    l[0] = 1.0; mu[0] = 0.0; z[0] = 0.0;

    for (size_t i = 1; i < n; ++i) {
        l[i] = 2.0 * (xs_[i + 1] - xs_[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    l[n] = 1.0; z[n] = 0.0; c_[n] = 0.0;

    // Back substitution
    b_.resize(n); d_.resize(n); a_.resize(n);
    for (int j = static_cast<int>(n) - 1; j >= 0; --j) {
        size_t k = static_cast<size_t>(j);
        c_[k] = z[k] - mu[k] * c_[k + 1];
        b_[k] = (ys_[k + 1] - ys_[k]) / h[k] - h[k] * (c_[k + 1] + 2.0 * c_[k]) / 3.0;
        d_[k] = (c_[k + 1] - c_[k]) / (3.0 * h[k]);
        a_[k] = ys_[k];
    }
}

Real CubicSplineInterpolator::operator()(Real x) const {
    if (x <= xs_.front()) return ys_.front();
    if (x >= xs_.back()) return ys_.back();

    auto it = std::upper_bound(xs_.begin(), xs_.end(), x);
    size_t i = std::distance(xs_.begin(), it) - 1;
    if (i >= a_.size()) i = a_.size() - 1;

    Real dx = x - xs_[i];
    return a_[i] + b_[i] * dx + c_[i] * dx * dx + d_[i] * dx * dx * dx;
}

Real CubicSplineInterpolator::derivative(Real x) const {
    if (x <= xs_.front() || x >= xs_.back()) return 0.0;

    auto it = std::upper_bound(xs_.begin(), xs_.end(), x);
    size_t i = std::distance(xs_.begin(), it) - 1;
    if (i >= b_.size()) i = b_.size() - 1;

    Real dx = x - xs_[i];
    return b_[i] + 2.0 * c_[i] * dx + 3.0 * d_[i] * dx * dx;
}

// --- NelsonSiegelSvensson ---

NelsonSiegelSvensson::NelsonSiegelSvensson(Parameters params)
    : params_(params) {}

Real NelsonSiegelSvensson::operator()(Tenor t) const {
    if (t <= 0.0) return params_.beta0 + params_.beta1;

    Real x1 = t / params_.tau1;
    Real x2 = t / params_.tau2;
    Real exp1 = std::exp(-x1);
    Real exp2 = std::exp(-x2);

    Real f1 = (1.0 - exp1) / x1;
    Real f2 = f1 - exp1;
    Real f3 = (1.0 - exp2) / x2 - exp2;

    return params_.beta0 + params_.beta1 * f1 + params_.beta2 * f2 + params_.beta3 * f3;
}

Real NelsonSiegelSvensson::forward_rate(Tenor t) const {
    if (t <= 0.0) return params_.beta0 + params_.beta1;

    Real x1 = t / params_.tau1;
    Real x2 = t / params_.tau2;
    Real exp1 = std::exp(-x1);
    Real exp2 = std::exp(-x2);

    return params_.beta0 + params_.beta1 * exp1
         + params_.beta2 * x1 * exp1 + params_.beta3 * x2 * exp2;
}

}  // namespace fie
