//
// statistics.hpp
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

#ifndef statistics_hpp
#define statistics_hpp

#include <cmath>
#include <functional>
#include <iostream>

namespace statistics{

    class ProbabilityDist;
    class TDist;

    double trapezium(double a, double b, std::function<double(double)> f, int n = 1000);

    //! Abstract base class for probability distributions
    class ProbabilityDist{
    protected:
        double _coeff;
        double _nu;
        virtual double calcCoeff() const = 0;
    public:
        ProbabilityDist(){
            _coeff = 0;
            _nu = 0;
        }
        //! Probability density function of distribution
        virtual double pdf(double x) const = 0;
        //! p-value of value x
        virtual double pValue(double x) const = 0;
    };

    //! Student's T distribution
    class TDist: public ProbabilityDist{
        double calcCoeff() const override;
    public:
        TDist() : ProbabilityDist(){};
        explicit TDist(double nu) : ProbabilityDist(){
            setNu(nu);
        }
        void setNu(double nu);
        double pdf(double x) const override;
        double pValue(double x) const override;
    };

    //! Normal distribution
    class NormDist: public ProbabilityDist{
        double calcCoeff() const override;
    public:
        NormDist() : ProbabilityDist(){
            _coeff = calcCoeff();
        };
        double pdf(double x) const override;
        double pValue(double x) const override;
    };

    /**
     * Calculate mean of a member in a vector \p v of type \T
     * @tparam T
     * @param v A vector of objects
     * @param f A function returning a double from type \p T
     * @return mean
     */
    template<class T> double mean(const std::vector<T>& v, std::function<double(T)> f) {
        double sum = 0;
        for(auto n : v) sum += f(n);
        return sum / double(v.size());
    }

    template<class T> double mean(const std::vector<T>& v) {
        double sum = 0;
        for(auto n : v) sum += double(n);
        return sum / double(v.size());
    }

    /**
     * Calculate standard deviation of a member in a vector \p v of type \T
     * @tparam T
     * @param v A vector of objects
     * @param f A function returning a double from type \p T
     * @return standard deviation
     */
    template<class T> double sd(const std::vector<T>& v, std::function<double(T)> f){
        double mean = ::statistics::mean<T>(v, f);
        double ss = 0; // sum of squared differences
        for(auto n : v) ss += pow(f(n) - mean, 2);
        double sd = sqrt(ss / double(v.size() - 1));
        return sd;
    }

    template<class T> double sd(const std::vector<T>& v){
        double mean = ::statistics::mean<T>(v);
        double ss = 0; // sum of squared differences
        for(auto n : v) ss += pow(double(n) - mean, 2);
        double sd = sqrt(ss / double(v.size() - 1));
        return sd;
    }
}

#endif
