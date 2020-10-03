#pragma once

#include <gmpxx.h>
#include <initializer_list>
#include <vector>
#include <string>

using Z = mpz_class;
inline Z operator""_Z(const char* s) {
  return operator""_mpz(s);
}

class polynomial {
public:
  polynomial();
  polynomial(std::initializer_list<Z> coeffs);
  explicit polynomial(std::vector<Z> coeffs);

  int degree() const;
  Z coefficient(int d) const;
  const std::vector<Z>& coefficients() const;

  void negate();
  polynomial times_x_to(int n) const;
  polynomial& operator-=(const polynomial& other);
  polynomial& operator*=(Z n);

  std::string to_string() const;

private:
  std::vector<Z> m_coeffs;

  void normalize();
  static std::string monomial_to_string(const Z& coeff, int d);
};

bool operator==(const polynomial& p, const polynomial& q);
bool operator!=(const polynomial& p, const polynomial& q);

polynomial operator-(const polynomial& p);
polynomial operator-(const polynomial& p, const polynomial& q);
polynomial operator*(Z n, const polynomial& p);
