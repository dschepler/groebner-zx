#pragma once

#include <gmpxx.h>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

using Z = mpz_class;
inline Z operator""_Z(const char* s)
{
    return operator""_mpz(s);
}

// polynomial expression templates: each type should declare a member type
// is_polynomial_expr, and implement methods compatible with:
//    int degree_bound() const;
//      upper bound on degree of the result (does not need to be exact in
//      cases such as sum or difference of two polynomials)
//    Z coefficient(int d) const;
//      return the coefficient of x^d - where d can still be greater than
//      degree_bound()

template <typename CoeffCallable>
class polynomial_expr {
public:
    using is_polynomial_expr = std::true_type;

    polynomial_expr(int degree_bound, CoeffCallable&& coeff_callable)
        : m_degree_bound(degree_bound)
        , m_coeff_callable(std::forward<CoeffCallable>(coeff_callable))
    {
    }
    int degree_bound() const { return m_degree_bound; }
    Z coefficient(int d) const
    {
        return m_coeff_callable(d);
    }

private:
    int m_degree_bound;
    typename std::decay_t<CoeffCallable> m_coeff_callable;
};

template <typename PolyExpr1, typename PolyExpr2,
    typename = typename std::decay_t<PolyExpr1>::is_polynomial_expr,
    typename = typename std::decay_t<PolyExpr2>::is_polynomial_expr>
auto operator+(PolyExpr1&& p, PolyExpr2&& q)
{
    return polynomial_expr {
        std::max(p.degree_bound(), q.degree_bound()),
        [&p, &q](int d) -> Z { return p.coefficient(d) + q.coefficient(d); }
    };
}

template <typename PolyExpr1, typename PolyExpr2,
    typename = typename std::decay_t<PolyExpr1>::is_polynomial_expr,
    typename = typename std::decay_t<PolyExpr2>::is_polynomial_expr>
auto operator-(PolyExpr1&& p, PolyExpr2&& q)
{
    return polynomial_expr {
        std::max(p.degree_bound(), q.degree_bound()),
        [&p, &q](int d) -> Z { return p.coefficient(d) - q.coefficient(d); }
    };
}

template <typename PolyExpr,
    typename = typename std::decay_t<PolyExpr>::is_polynomial_expr>
auto operator-(PolyExpr&& p)
{
    return polynomial_expr {
        p.degree_bound(),
        [&p](int d) -> Z { return -(p.coefficient(d)); }
    };
}

template <typename PolyExpr,
    typename = typename std::decay_t<PolyExpr>::is_polynomial_expr>
auto operator*(const Z& n, PolyExpr&& p)
{
    return polynomial_expr {
        n == 0 ? -1 : p.degree_bound(),
        [&n, &p](int d) -> Z { return n * p.coefficient(d); }
    };
}

template <typename PolyExpr,
    typename = typename std::decay_t<PolyExpr>::is_polynomial_expr>
auto operator*(PolyExpr&& p, const Z& n)
{
    return polynomial_expr {
        n == 0 ? -1 : p.degree_bound(),
        [&n, &p](int d) -> Z { return p.coefficient(d) * n; }
    };
}

class polynomial;
polynomial operator*(const polynomial& p, const polynomial& q);

template <typename PolyExpr,
    typename = typename std::decay_t<PolyExpr>::is_polynomial_expr>
auto times_x_to(PolyExpr&& p, int d)
{
    return polynomial_expr {
        p.degree_bound() < 0 ? -1 : p.degree_bound() + d,
        [&p, d](int e) -> Z { return e >= d ? p.coefficient(e - d) : 0; }
    };
}

template <typename PolyExpr1, typename PolyExpr2,
    typename = typename std::decay_t<PolyExpr1>::is_polynomial_expr,
    typename = typename std::decay_t<PolyExpr2>::is_polynomial_expr>
bool operator==(PolyExpr1&& p, PolyExpr2&& q)
{
    auto d = std::max(p.degree_bound(), q.degree_bound());
    for (int i = 0; i <= d; ++i)
        if (p.coefficient(i) != q.coefficient(i))
            return false;
    return true;
}

template <typename PolyExpr1, typename PolyExpr2,
    typename = typename std::decay_t<PolyExpr1>::is_polynomial_expr,
    typename = typename std::decay_t<PolyExpr2>::is_polynomial_expr>
bool operator!=(PolyExpr1&& p, PolyExpr2&& q)
{
    auto d = std::max(p.degree_bound(), q.degree_bound());
    for (int i = 0; i <= d; ++i)
        if (p.coefficient(i) != q.coefficient(i))
            return true;
    return false;
}

class polynomial {
public:
    using is_polynomial_expr = std::true_type;

    polynomial() = default;
    polynomial(std::initializer_list<Z> coeffs);
    explicit polynomial(std::vector<Z> coeffs);

    polynomial(const polynomial&) = default;
    polynomial(polynomial&&) = default;
    template <typename PolyExpr,
        typename = typename std::decay_t<PolyExpr>::is_polynomial_expr>
    polynomial(PolyExpr&& p)
    {
        int d = p.degree_bound();
        if (d >= 0) {
            m_coeffs.reserve(d + 1);
            for (; d >= 0; --d)
                m_coeffs.push_back(p.coefficient(d));
            normalize();
        }
    }

    polynomial& operator=(const polynomial&) = default;
    polynomial& operator=(polynomial&&) = default;
    template <typename PolyExpr,
        typename = typename std::decay_t<PolyExpr>::is_polynomial_expr>
    polynomial& operator=(PolyExpr&& p)
    {
        auto deg = p.degree_bound();
        m_coeffs.resize(deg + 1);
        for (int d = 0; d <= deg; ++d)
            m_coeffs[deg - d] = p.coefficient(d);
        normalize();
        return *this;
    }

    int degree() const { return m_coeffs.size() - 1; }
    int degree_bound() const { return degree(); }
    const Z& coefficient(int d) const
    {
        static Z static_zero = 0;
        return d > degree() ? static_zero : m_coeffs[degree() - d];
    }

    // leading_coefficient has as a precondition that the polynomial must not be 0
    const Z& leading_coefficient() const
    {
        return m_coeffs.front();
    }

    void negate();

    template <typename PolyExpr,
        typename = typename std::decay_t<PolyExpr>::is_polynomial_expr>
    polynomial& operator+=(PolyExpr&& p)
    {
        if (p.degree_bound() > degree())
            m_coeffs.insert(m_coeffs.begin(), p.degree_bound() - degree(), 0);
        for (int d = p.degree_bound(); d >= 0; --d)
            m_coeffs[m_coeffs.size() - d - 1] += p.coefficient(d);
        normalize();
        return *this;
    }
    template <typename PolyExpr,
        typename = typename std::decay_t<PolyExpr>::is_polynomial_expr>
    polynomial& operator-=(PolyExpr&& p)
    {
        if (p.degree_bound() > degree())
            m_coeffs.insert(m_coeffs.begin(), p.degree_bound() - degree(), 0);
        for (int d = p.degree_bound(); d >= 0; --d)
            m_coeffs[m_coeffs.size() - d - 1] -= p.coefficient(d);
        normalize();
        return *this;
    }
    polynomial& operator*=(Z n);
    polynomial& operator*=(const polynomial& p)
    {
        return *this = (*this * p);
    }

    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const polynomial& p)
    {
        return os << p.to_string();
    }

private:
    std::vector<Z> m_coeffs;

    void normalize();
    static std::string monomial_to_string(const Z& coeff, int d);
};
