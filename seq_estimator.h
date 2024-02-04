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

#ifndef LZJ_SEQ_ESTIMATOR_H_
#define LZJ_SEQ_ESTIMATOR_H_

#include <iostream>     // cout
#include <cmath>        // nan
#include <limits>       // numeric_limits

namespace Hasaki {

// 可以迭代估计一个序列的 mean/var/min/max
// 开销很小, O(1)
class SeqEstimator
{
public:
    SeqEstimator();
    SeqEstimator(const double init_value);
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

SeqEstimator::SeqEstimator()
        : m_n(0)
        , m_mean(0)
        , m_var(0)
        , m_min(std::numeric_limits<double>::max())
        , m_max(std::numeric_limits<double>::lowest())
{
}

SeqEstimator::SeqEstimator(const double init_value)
        : m_n(0)
        , m_mean(0)
        , m_var(0)
        , m_min(std::numeric_limits<double>::max())
        , m_max(std::numeric_limits<double>::lowest())
{
    Append(init_value);
}

double SeqEstimator::GetN() const {
    return m_n;
}

double SeqEstimator::GetMean() const {
    return m_n >= 1 ? m_mean : NAN;
}

double SeqEstimator::GetVar() const {
    return m_n >= 2 ? m_var: NAN;
}

double SeqEstimator::GetMin() const {
    return m_n >= 1 ? m_min: NAN;
}

double SeqEstimator::GetMax() const {
    return m_n >= 1 ? m_max: NAN;
}

void SeqEstimator::Append(const double new_value) {
    // 为了减少分支判断带来的开销, 用了一些技巧, 从而牺牲了可读性
    // 我们注意到, 使用了技巧后, 对于所有有效的结果都不会改变, 但提高了运行效率
    // 对于 n = 0, mu = 0 (INVALID), var = 0 (INVALID)
    // 对于 n = 1, mu = x1, var = x1^2 (INVALID)
    // 对于 n = 2, mu = (x1 + x2) / 2, var = (x2 - x1)^2 / 2 = (x1 - mu)^2 + (x2 - mu)^2
    double rev_beta_n = m_n == 0 ? 0 : (1 - 1 / m_n);   // 1 - beta(n)
    ++ m_n;
    double beta_n1 = 1 / m_n;                           // beta(n + 1)
    double xn1_mun = new_value - m_mean;                // x(n + 1) - mu(n)
    m_mean += beta_n1 * xn1_mun;                        // mu(n + 1) = mu(n) + beta(n + 1) * (x(n + 1) - mu(n))
    m_var = rev_beta_n * m_var + beta_n1 * xn1_mun * xn1_mun;   // var(n + 1) = (1 - beta(n)) * var(n) + beta(n + 1) * (x(n + 1) - mu(n))^2

    if (new_value < m_min)
        m_min = new_value;
    if (new_value > m_max)
        m_max = new_value;
}

std::ostream &operator<<(std::ostream &os, SeqEstimator &seq) {
    os << "(n = " << seq.GetN()
       << ", avg = " << seq.GetMean()
       << ", var = " << seq.GetVar()
       << ", min = " << seq.GetMin()
       << ", max = " << seq.GetMax()
       << ")";
    return os;
}

} // namespace Hasaki

#endif //LZJ_SEQ_ESTIMATOR_H_
