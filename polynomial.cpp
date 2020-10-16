#include "polynomial.h"
#include <algorithm>

polynomial::polynomial(std::initializer_list<Z> coeffs) : m_coeffs(coeffs)
{
  normalize();
}

polynomial::polynomial(std::vector<Z> coeffs) : m_coeffs(std::move(coeffs))
{
  normalize();
}

void polynomial::normalize() {
  auto first_nonzero = find_if(m_coeffs.begin(), m_coeffs.end(),
			       [](const Z& coeff) { return coeff != 0; });
  m_coeffs.erase(m_coeffs.begin(), first_nonzero);
}

void polynomial::negate() {
  for (auto& coeff : m_coeffs)
    coeff = -coeff;
}

polynomial& polynomial::operator*=(Z n) {
  if (n == 0)
    m_coeffs.clear();
  else {
    for (Z& coeff : m_coeffs)
      coeff *= n;
  }
  return *this;
}

polynomial operator*(const polynomial& p, const polynomial& q) {
    // Following the classic recursive algorithm with O(d^lg(3)) multiplications of Z values

    if (p == polynomial{} || q == polynomial{})
        return polynomial{};
    if (p.degree() == 0)
        return p.coefficient(0) * q;
    if (q.degree() == 0)
        return p * q.coefficient(0);

    // In the following, we'll be using each coefficient of p and q
    // multiple times, which is why we have designed the interface to let
    // the caller materialize p and q for us.

    polynomial_expr pe { p.degree() / 2, [&p](int d) -> const Z& { return p.coefficient(d * 2); } };
    polynomial_expr po { (p.degree() - 1) / 2, [&p](int d) -> const Z& { return p.coefficient(d * 2 + 1); } };
    polynomial_expr qe { q.degree() / 2, [&q](int d) -> const Z& { return q.coefficient(d * 2); } };
    polynomial_expr qo { (q.degree() - 1) / 2, [&q](int d) -> const Z& { return q.coefficient(d * 2 + 1); } };

    polynomial pe_qe = pe * qe;
    polynomial po_qo = po * qo;
    polynomial pepo_qeqo = (pe + po) * (qe + qo);

    return polynomial_expr {
        p.degree() + q.degree(),
        [&](int d) -> Z {
            if (d % 2 == 0)
                return (pe_qe + po_qo.times_x_to(1)).coefficient(d / 2);
            else
                return (pepo_qeqo - pe_qe - po_qo).coefficient(d / 2);
        }
    };
}

std::string polynomial::monomial_to_string(const Z& coeff, int d) {
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
  
std::string polynomial::to_string() const {
  if (m_coeffs.empty())
    return "0";
  std::string result = monomial_to_string(m_coeffs.front(), degree());
  for (int d = degree() - 1; d >= 0; --d) {
    auto coeff = coefficient(d);
    if (coeff > 0) {
      result += " + ";
      result += monomial_to_string(coeff, d);
    }
    else if (coeff < 0) {
      result += " - ";
      result += monomial_to_string(-coeff, d);
    }
  }

  return result;
}
