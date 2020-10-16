#include "polynomial.h"
#include <gtest/gtest.h>

TEST(Polynomial, Equality) {
  EXPECT_EQ((polynomial{}), (polynomial{}));
  EXPECT_EQ((polynomial{0, 0}), (polynomial{}));
  EXPECT_EQ((polynomial{}), (polynomial{0, 0}));
  EXPECT_EQ((polynomial{1, 2}), (polynomial{1, 2}));
  EXPECT_EQ((polynomial{0, 1, 2}), (polynomial{1, 2}));
  EXPECT_EQ((polynomial{1, 2}), (polynomial{0, 1, 2}));
  EXPECT_NE((polynomial{1, 2}), (polynomial{1, 2, 0}));
}

TEST(Polynomial, Degree) {
  EXPECT_EQ((polynomial{}.degree()), -1);
  EXPECT_EQ((polynomial{5}.degree()), 0);
  EXPECT_EQ((polynomial{1, 2, 3}.degree()), 2);
}

TEST(Polynomial, Coefficient) {
  EXPECT_EQ((polynomial{}.coefficient(0)), 0);
  EXPECT_EQ((polynomial{}.coefficient(1)), 0);
  EXPECT_EQ((polynomial{5}.coefficient(0)), 5);
  EXPECT_EQ((polynomial{5}.coefficient(1)), 0);
  EXPECT_EQ((polynomial{5}.coefficient(2)), 0);
  EXPECT_EQ((polynomial{1, 2, 3}.coefficient(0)), 3);
  EXPECT_EQ((polynomial{1, 2, 3}.coefficient(1)), 2);
  EXPECT_EQ((polynomial{1, 2, 3}.coefficient(2)), 1);
  EXPECT_EQ((polynomial{1, 2, 3}.coefficient(3)), 0);
  EXPECT_EQ((polynomial{1, 2, 3}.coefficient(4)), 0);
}

TEST(Polynomial, Negate) {
  EXPECT_EQ((-polynomial{}), (polynomial{}));
  EXPECT_EQ((-polynomial{1, 2}), (polynomial{-1, -2}));
  EXPECT_EQ((-polynomial{1, -3, -2}), (polynomial{-1, 3, 2}));
}

TEST(Polynomial, TimesXTo) {
  EXPECT_EQ((polynomial{}.times_x_to(5)), (polynomial{}));
  EXPECT_EQ((polynomial{1, 2, 3}.times_x_to(0)), (polynomial{1, 2, 3}));
  EXPECT_EQ((polynomial{1, 2, 3}.times_x_to(3)),
	    (polynomial{1, 2, 3, 0, 0, 0}));
}

TEST(Polynomial, ScalarMult) {
  EXPECT_EQ((2 * polynomial{}), (polynomial{}));
  EXPECT_EQ((0 * polynomial{1, 2, 3}), (polynomial{}));
  EXPECT_EQ((3 * polynomial{1, -2, 3}), (polynomial{3, -6, 9}));
  EXPECT_EQ((-3 * polynomial{1, -2, 3}), (polynomial{-3, 6, -9}));
}

TEST(Polynomial, Add) {
    EXPECT_EQ((polynomial{} + polynomial{}), (polynomial{}));
    EXPECT_EQ((polynomial{1, 2, 3} + polynomial{}), (polynomial{1, 2, 3}));
    EXPECT_EQ((polynomial{} + polynomial{1, 2, 3}), (polynomial{1, 2, 3}));
    EXPECT_EQ((polynomial{1, 2, 3} + polynomial{1, 2, 3}), (polynomial{2, 4, 6}));
    EXPECT_EQ((polynomial{1, 2, 3} + polynomial{-1, -2, -3}), (polynomial{}));
    EXPECT_EQ((polynomial{1, 2, 3, 4} + polynomial{-1, -2, 5, 6}),
              (polynomial{8, 10}));
    EXPECT_EQ((polynomial{1, 2, 3, 4} + polynomial{5, 6}),
              (polynomial{1, 2, 8, 10}));
    EXPECT_EQ((polynomial{5, 6} + polynomial{1, 2, 3, 4}),
              (polynomial{1, 2, 8, 10}));
}

TEST(Polynomial, Subtract) {
  EXPECT_EQ((polynomial{} - polynomial{}), (polynomial{}));
  EXPECT_EQ((polynomial{1, 2, 3} - polynomial{}), (polynomial{1, 2, 3}));
  EXPECT_EQ((polynomial{} - polynomial{1, 2, 3}), (polynomial{-1, -2, -3}));
  EXPECT_EQ((polynomial{1, 2, 3} - polynomial{1, 2, 3}), (polynomial{}));
  EXPECT_EQ((polynomial{1, 2, 3, 4} - polynomial{1, 2, 5, 6}),
	    (polynomial{-2, -2}));
  EXPECT_EQ((polynomial{1, 2, 3, 4} - polynomial{5, 6}),
	    (polynomial{1, 2, -2, -2}));
  EXPECT_EQ((polynomial{5, 6} - polynomial{1, 2, 3, 4}),
	    (polynomial{-1, -2, 2, 2}));
}

TEST(Polynomial, Multiply) {
    EXPECT_EQ((polynomial{} * polynomial{}), (polynomial{}));
    EXPECT_EQ((polynomial{} * polynomial{1, 2, 3}), (polynomial{}));
    EXPECT_EQ((polynomial{1, 2, 3} * polynomial{}), (polynomial{}));
    EXPECT_EQ((polynomial{2} * polynomial{1, 2, 3}), (polynomial{2, 4, 6}));
    EXPECT_EQ((polynomial{1, 2, 3} * polynomial{2}), (polynomial{2, 4, 6}));
    EXPECT_EQ((polynomial{1, 5} * polynomial{1, 6}), (polynomial{1, 11, 30}));
    EXPECT_EQ((polynomial{1, 2} * polynomial{1, -2}), (polynomial{1, 0, -4}));
    EXPECT_EQ((polynomial{1, 1, 1, 1, 1} * polynomial{1, 1, 1}), (polynomial{1, 2, 3, 3, 3, 2, 1}));
}

TEST(Polynomial, ExprTemplates) {
    polynomial p { 2, 3, 4 };
    polynomial q { 5, 6 };
    EXPECT_EQ(p * q - p, p * (q - polynomial{1}));
    EXPECT_EQ(p + q + p, 2 * p + q);
    EXPECT_EQ(3 * p + 4 * times_x_to(q, 2), (polynomial{20, 30, 9, 12}));
    EXPECT_EQ(5 * p - 2 * times_x_to(q, 1), (polynomial{3, 20}));
}

TEST(Polynomial, ToString) {
  EXPECT_EQ((polynomial{}.to_string()), "0");
  EXPECT_EQ((polynomial{13}.to_string()), "13");
  EXPECT_EQ((polynomial{-3}.to_string()), "-3");
  EXPECT_EQ((polynomial{1, 0}.to_string()), "x");
  EXPECT_EQ((polynomial{1, 4}.to_string()), "x + 4");
  EXPECT_EQ((polynomial{1, -4}.to_string()), "x - 4");
  EXPECT_EQ((polynomial{2, 3}.to_string()), "2 x + 3");
  EXPECT_EQ((polynomial{-2, -3}.to_string()), "-2 x - 3");
  EXPECT_EQ((polynomial{-1, 5}.to_string()), "-x + 5");
  EXPECT_EQ((polynomial{1, 3, 2}.to_string()), "x^2 + 3 x + 2");
  EXPECT_EQ((polynomial{1, 0, -3, 0, 0, 1, 5}.to_string()), "x^6 - 3 x^4 + x + 5");
  EXPECT_EQ((polynomial{-1, 0, 0, 0}.to_string()), "-x^3");
  EXPECT_EQ((polynomial{1, 0, 1}.to_string()), "x^2 + 1");
  EXPECT_EQ((polynomial{1, 0, -1}.to_string()), "x^2 - 1");
  EXPECT_EQ((polynomial{-1}.to_string()), "-1");
}
