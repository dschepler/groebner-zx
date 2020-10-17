#include "polynomial.h"
#include <algorithm>

polynomial::polynomial(std::initializer_list<Z> coeffs)
    : m_coeffs(coeffs)
{
    normalize();
}

polynomial::polynomial(std::vector<Z> coeffs)
    : m_coeffs(std::move(coeffs))
{
    normalize();
}

void polynomial::normalize()
{
    auto first_nonzero = find_if(m_coeffs.begin(), m_coeffs.end(),
        [](const Z& coeff) { return coeff != 0; });
    m_coeffs.erase(m_coeffs.begin(), first_nonzero);
}

void polynomial::negate()
{
    for (auto& coeff : m_coeffs)
        coeff = -coeff;
}

polynomial& polynomial::operator*=(Z n)
{
    if (n == 0)
        m_coeffs.clear();
    else {
        for (Z& coeff : m_coeffs)
            coeff *= n;
    }
    return *this;
}

namespace polynomial_mult_details {

auto evenpart(const polynomial& p) {
    return polynomial_expr {
        p.degree() / 2,
        [&p](int d) -> const Z& { return p.coefficient(d * 2); }
    };
}

auto oddpart(const polynomial& p) {
    return polynomial_expr {
        (p.degree() - 1) / 2,
        [&p](int d) -> const Z& { return p.coefficient(d * 2 + 1); }
    };
}

template <typename PolyExpr1, typename PolyExpr2,
          typename = typename PolyExpr1::is_polynomial_expr,
          typename = typename PolyExpr2::is_polynomial_expr>
auto interleave(PolyExpr1&& p, PolyExpr2&& q) {
    return polynomial_expr {
        std::max(2 * p.degree_bound(), 2 * q.degree_bound() + 1),
        [&p, &q](int d) -> Z {
            if (d % 2 == 0)
                return p.coefficient(d / 2);
            else
                return q.coefficient(d / 2);
        }
    };
}

} // namespace polynomial_mult_details

polynomial operator*(const polynomial& p, const polynomial& q)
{
    // Following the classic recursive algorithm with O(d^lg(3)) multiplications of Z values

    if (p == polynomial {} || q == polynomial {})
        return polynomial {};
    if (p.degree() == 0)
        return p.coefficient(0) * q;
    if (q.degree() == 0)
        return p * q.coefficient(0);

    // In the following, we'll be using each coefficient of p and q
    // multiple times, which is why we have designed the interface to let
    // the caller materialize p and q for us.

    auto pe = polynomial_mult_details::evenpart(p);
    auto po = polynomial_mult_details::oddpart(p);
    auto qe = polynomial_mult_details::evenpart(q);
    auto qo = polynomial_mult_details::oddpart(q);

    polynomial pe_qe = pe * qe;
    polynomial po_qo = po * qo;
    polynomial pepo_qeqo = (pe + po) * (qe + qo);

    return polynomial_mult_details::interleave(
        pe_qe + times_x_to(po_qo, 1),
        pepo_qeqo - pe_qe - po_qo);
}

std::string polynomial::monomial_to_string(const Z& coeff, int d)
{
    std::string result;
    if (coeff == 1 && d > 0)
        result = "";
    else if (coeff == -1 && d > 0)
        result = "-";
    else {
        result = coeff.get_str();
        if (d > 0)
            result += " ";
    }
    if (d == 1)
        result += "x";
    else if (d > 1) {
        result += "x^";
        result += std::to_string(d);
    }
    return result;
}

std::string polynomial::to_string() const
{
    if (m_coeffs.empty())
        return "0";
    std::string result = monomial_to_string(m_coeffs.front(), degree());
    for (int d = degree() - 1; d >= 0; --d) {
        auto coeff = coefficient(d);
        if (coeff > 0) {
            result += " + ";
            result += monomial_to_string(coeff, d);
        } else if (coeff < 0) {
            result += " - ";
            result += monomial_to_string(-coeff, d);
        }
    }

    return result;
}
