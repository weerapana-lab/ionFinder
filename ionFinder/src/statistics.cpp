//
// statistics.cpp
// ionFinder
// -----------------------------------------------------------------------------
// MIT License
// Copyright 2021 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
//

#include "statistics.hpp"

double statistics::TDist::calcCoeff() const {
    double pi = 4.0 * atan(1.0);
    double ret = tgamma(0.5 * (_nu + 1.0 )) / tgamma(0.5 * _nu) / sqrt(_nu * pi);
    return ret;
}

 //! Set number of degrees of freedom.
void statistics::TDist::setNu(double nu){
    _nu = nu;
    _coeff = calcCoeff();
}

//! Probability density function of the student's t distribution
double statistics::TDist::pdf(double x) const{
    double ret = _coeff * pow(1.0 + x * x / _nu, -0.5 * (_nu + 1.0 ));
    return ret;
}

double statistics::TDist::pValue(double x) const{
    auto fp = std::bind(&statistics::TDist::pdf, *this, std::placeholders::_1);
    return  trapezium(0, x, fp) + 0.5;
}

double statistics::NormDist::calcCoeff() const{
    double pi = 4.0 * atan(1.0);
    double ret = 1 / (sqrt(2 * pi));
    return ret;
}

double statistics::NormDist::pdf(double x) const{
    double ret = _coeff * exp(-0.5 * pow(x, 2));
    return ret;
}

double statistics::NormDist::pValue(double x) const{
    auto fp = std::bind(&statistics::NormDist::pdf, *this, std::placeholders::_1);
    return trapezium(0, x, fp) + 0.5;
}

/**
 * numerical integration of f(x) from a to b
 * @param a Start of interval.
 * @param b End of interval.
 * @param f Function
 * @param n Sampling interval for integration.
 * @return area
 */
double statistics::trapezium(double a, double b, std::function<double(double)> f, int n)
{
    double dx = (b - a) / n;
    double integral = f(a) + f(b);
    for(int i = 1; i <= n - 1; i++) integral += 2.0 * f(a + i * dx);
    integral *= dx / 2.0;
    return integral;
}
