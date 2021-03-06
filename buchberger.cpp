#include "buchberger.h"
#include <algorithm>

bool decreasing_leading_term::operator()(const polynomial& p, const polynomial& q) const
{
    if (p.degree() > q.degree())
        return true;
    else if (p.degree() < q.degree())
        return false;
    else {
        for (int d = p.degree(); d >= 0; --d) {
            if (p.coefficient(d) > q.coefficient(d))
                return true;
            else if (p.coefficient(d) < q.coefficient(d))
                return false;
        }
        return false;
    }
}

void reduce_mod(polynomial& p, const polynomial& q)
{
    int q_deg = q.degree();
    if (q_deg < 0)
        return;
    Z q_leading_coeff = q.coefficient(q_deg);

    int initial_p_deg = p.degree();
    for (int i = initial_p_deg - q_deg; i >= 0; --i) {
        Z p_coeff = p.coefficient(i + q_deg);
        Z quotient = p_coeff / q_leading_coeff;
        if (p_coeff - quotient * q_leading_coeff < 0)
            --quotient;
        p -= quotient * times_x_to(q, i);
    }
}

bool buchberger_search(ideal_basis& b)
{
    // First try modding out any entry by following entries
    for (auto i = b.begin(); i != b.end(); ++i) {
        auto p = *i;
        for (auto j = std::next(i); j != b.end(); ++j) {
            reduce_mod(p, *j);
        }
        if (p != *i) {
            if (p.leading_coefficient() < 0)
                p.negate();
            b.erase(i);
            if (p != polynomial {})
                b.insert(std::move(p));
            return true;
        }
    }

    // Then try raising a lower entry to match the degree of a higher entry,
    // modding out, and see if we get something new; this is the generalization
    // of forming the twist of pairs in the usual Buchberger algorithm.
    for (auto i = b.begin(); i != b.end(); ++i) {
        for (auto j = std::next(i); j != b.end(); ++j) {
            polynomial p = times_x_to(*j, i->degree() - j->degree());
            for (auto k = i; k != b.end(); ++k)
                reduce_mod(p, *k);
            if (p != polynomial {}) {
                if (p.leading_coefficient() < 0)
                    p.negate();
                b.insert(std::move(p));
                return true;
            }
        }
    }

    return false;
}

void buchberger(ideal_basis& b)
{
    // Remove 0 generator if present, for sanity of following code
    b.erase(polynomial {});

    // Make leading terms of all generators positive
    while (true) {
        auto i = std::find_if(b.begin(), b.end(), [](const polynomial& p) { return p.leading_coefficient() < 0; });
        if (i == b.end())
            break;
        polynomial minus_p = -(*i);
        b.erase(i);
        b.insert(std::move(minus_p));
    }

    while (buchberger_search(b))
        ;
}
