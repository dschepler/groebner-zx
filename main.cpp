#include "buchberger.h"
#include "polynomial.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include <unistd.h>

int main()
{
    if (isatty(STDIN_FILENO)) {
        std::cout << "groebner-zx  Copyright (C) 2020  Daniel Schepler\n";
        std::cout << "This program comes with ABSOLUTELY NO WARRANTY.\n";
        std::cout << "This is free software, and you are welcome to redistribute it\n";
        std::cout << "under the conditions of the GNU General Public License v3\n\n";
        std::cout << "Enter the list of polynomials, one on each line as a list of coefficients\n";
        std::cout << "e.g. enter x^5 - 3x^2 + x as: 1 0 0 -3 1 0\n";
        std::cout << "Then end the list with a blank line or EOF\n";
    }

    std::string line;
    std::vector<polynomial> v;
    while (std::getline(std::cin, line) && !line.empty()) {
        std::istringstream iss { line };
        polynomial p { std::vector<Z> { std::istream_iterator<Z> { iss },
            std::istream_iterator<Z> {} } };
        v.push_back(std::move(p));
    }

    std::cout << "< ";
    bool first = true;
    for (const auto& p : v) {
        if (!first)
            std::cout << ", ";
        std::cout << p.to_string();
        first = false;
    }
    std::cout << " > = " << std::flush;

    ideal_basis b { v.begin(), v.end() };
    buchberger(b);
    std::cout << "< ";
    first = true;
    for (const auto& p : b) {
        if (!first)
            std::cout << ", ";
        std::cout << p.to_string();
        first = false;
    }
    std::cout << " >\n";
    return 0;
}
