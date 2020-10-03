#include "polynomial.h"
#include <algorithm>

polynomial::polynomial() { }
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

int polynomial::degree() const {
  return m_coeffs.size() - 1;
}

Z polynomial::coefficient(int d) const {
  if (d > degree())
    return 0;
  return m_coeffs[degree() - d];
}

const std::vector<Z>& polynomial::coefficients() const {
  return m_coeffs;
}

void polynomial::negate() {
  for (auto& coeff : m_coeffs)
    coeff = -coeff;
}

polynomial polynomial::times_x_to(int n) const {
  if (m_coeffs.empty())
    return *this;
  auto result_coeffs = m_coeffs;
  for (int i = 0; i < n; i++)
    result_coeffs.push_back(0);
  return polynomial{result_coeffs};
}

polynomial& polynomial::operator-=(const polynomial& other) {
  while (other.m_coeffs.size() > this->m_coeffs.size())
    m_coeffs.insert(m_coeffs.begin(), 0);
  auto i = m_coeffs.begin() + (this->m_coeffs.size() - other.m_coeffs.size());
  for (const auto& other_coeff : other.m_coeffs) {
    *i -= other_coeff;
    ++i;
  }
  normalize();
  return *this;
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

bool operator==(const polynomial& p, const polynomial& q) {
  return p.coefficients() == q.coefficients();
}
bool operator!=(const polynomial& p, const polynomial& q) {
  return !(p == q);
}

polynomial operator-(const polynomial& p) {
  polynomial result = p;
  result.negate();
  return result;
}

polynomial operator-(const polynomial& p, const polynomial& q) {
  polynomial result = p;
  result -= q;
  return result;
}

polynomial operator*(Z n, const polynomial& p) {
  polynomial result = p;
  result *= n;
  return result;
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
