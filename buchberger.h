#pragma once

#include <set>
#include "polynomial.h"

struct decreasing_leading_term {
  bool operator()(const polynomial& p, const polynomial& q) const;
};

using ideal_basis = std::set<polynomial, decreasing_leading_term>;

void buchberger(ideal_basis& b);
