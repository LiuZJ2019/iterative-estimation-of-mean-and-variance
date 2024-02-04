/**
 * @brief   本程序可以 O(1) 复杂度统计序列的均值和方差
 *
 * @author  LZJ (ZhouJie Liu)
 *
 * @date    2024.1
 *
 * @see     原理写在这个博客里的
 *          https://blog.csdn.net/MamiyaHasaki/article/details/124789567
 */

#ifndef LZJ_SEQ_ESTIMATOR_OPTIMIZED_H_
#define LZJ_SEQ_ESTIMATOR_OPTIMIZED_H_

#include <iostream>     // cout
#include <cmath>        // nan
#include <limits>       // numeric_limits

namespace Hasaki {

enum SeqType {
    NONE = 0ULL,
    MEAN = (1ULL << 0),
    VAR = (1ULL << 1),
    MIN = (1ULL << 2),
    MAX = (1ULL << 3),
};

template <size_t target, size_t mask>
constexpr bool checkType() {
    return (target & mask) == mask;
}

// 可以迭代估计一个序列的 mean/var/min/max
// 开销很小, O(1)
template<size_t type>
class SeqEstimatorOptimized
{
public:
    SeqEstimatorOptimized();
    SeqEstimatorOptimized(const double init_value);
    double GetN() const;
    double GetMean() const;
    double GetVar() const;
    double GetMin() const;
    double GetMax() const;

    void Append(const double new_value);

private:
    double m_n;
    double m_mean;
    double m_var;
    double m_min;
    double m_max;
};

template<size_t type>
SeqEstimatorOptimized<type>::SeqEstimatorOptimized()
        : m_n(0)
        , m_mean(0)
        , m_var(0)
        , m_min(std::numeric_limits<double>::max())
        , m_max(std::numeric_limits<double>::lowest())
{
}

template<size_t type>
SeqEstimatorOptimized<type>::SeqEstimatorOptimized(const double init_value)
        : m_n(0)
        , m_mean(0)
        , m_var(0)
        , m_min(std::numeric_limits<double>::max())
        , m_max(std::numeric_limits<double>::lowest())
{
    Append(init_value);
}

template<size_t type>
double SeqEstimatorOptimized<type>::GetN() const {
    return m_n;
}

template<size_t type>
double SeqEstimatorOptimized<type>::GetMean() const {
    static_assert (checkType<type, SeqType::MEAN> (), "a template estimator without Mean choice calls GetMean method");
    return m_n >= 1 ? m_mean : NAN;
}

template<size_t type>
double SeqEstimatorOptimized<type>::GetVar() const {
    static_assert (checkType<type, SeqType::VAR> (), "a template estimator without Var choice calls GetVar method");
    return m_n >= 2 ? m_var: NAN;
}

template<size_t type>
double SeqEstimatorOptimized<type>::GetMin() const {
    static_assert (checkType<type, SeqType::MIN> (), "a template estimator without Min choice calls GetMin method");
    return m_n >= 1 ? m_min: NAN;
}

template<size_t type>
double SeqEstimatorOptimized<type>::GetMax() const {
    static_assert (checkType<type, SeqType::MAX> (), "a template estimator without Max choice calls GetMin method");
    return m_n >= 1 ? m_max: NAN;
}

template<size_t type>
void SeqEstimatorOptimized<type>::Append(const double new_value) {
    // 1. min, max
    if constexpr (checkType<type, SeqType::MIN> ()) {
        if (new_value < m_min)
            m_min = new_value;
    }
    if constexpr (checkType<type, SeqType::MAX> ()) {
        if (new_value > m_max)
            m_max = new_value;
    }

    // 2. var or (mean and var)
    if constexpr (checkType<type, SeqType::VAR> ()) {
        double rev_beta_n = m_n == 0 ? 0 : (1 - 1 / m_n);   // 1 - beta(n)
        ++ m_n;
        double beta_n1 = 1 / m_n;                           // beta(n + 1)
        double xn1_mun = new_value - m_mean;                // x(n + 1) - mu(n)
        m_mean += beta_n1 * xn1_mun;                        // mu(n + 1) = mu(n) + beta(n + 1) * (x(n + 1) - mu(n))
        m_var = rev_beta_n * m_var + beta_n1 * xn1_mun * xn1_mun;   // var(n + 1) = (1 - beta(n)) * var(n) + beta(n + 1) * (x(n + 1) - mu(n))^2
    }
    // 3. only mean, no var
    else if constexpr (checkType<type, SeqType::MEAN> ()) {
        ++ m_n;
        double beta_n1 = 1 / m_n;
        double xn1_mun = new_value - m_mean;                // x(n + 1) - mu(n)
        m_mean += beta_n1 * xn1_mun;
    }
    else {
        ++ m_n;
    }
}

template<size_t type>
std::ostream &operator<<(std::ostream &os, SeqEstimatorOptimized<type> &seq) {
    os << "(n = " << seq.GetN();
    if constexpr (checkType<type, SeqType::MEAN> ()) {
        os << ", avg = " << seq.GetMean();
    }
    if constexpr (checkType<type, SeqType::VAR> ()) {
        os << ", var = " << seq.GetVar();
    }
    if constexpr (checkType<type, SeqType::MIN> ()) {
        os << ", min = " << seq.GetMin();
    }
    if constexpr (checkType<type, SeqType::MAX> ()) {
        os << ", max = " << seq.GetMax();
    }
    os << ")";
    return os;
}

} // namespace Hasaki

#endif //LZJ_SEQ_ESTIMATOR_OPTIMIZED_H_
