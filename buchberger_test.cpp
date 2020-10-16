#include "buchberger.h"
#include <gtest/gtest.h>

TEST(Buchberger, IdealSetOrdering)
{
    decreasing_leading_term cmp;
    EXPECT_FALSE(cmp({}, {}));
    EXPECT_TRUE(cmp({ 1 }, {}));
    EXPECT_FALSE(cmp({}, { 1 }));
    EXPECT_TRUE(cmp({ 5 }, { 3 }));
    EXPECT_FALSE(cmp({ 3 }, { 5 }));
    EXPECT_TRUE(cmp({ 1, -3 }, { 4 }));
    EXPECT_FALSE(cmp({ 4 }, { 1, -3 }));

    // Make sure two polynomials with same leading monomial can still
    // be in an ideal_basis independently
    EXPECT_TRUE(cmp({ 1, 16 }, { 1, 10 }) || cmp({ 1, 10 }, { 1, 16 }));
}

ideal_basis buchberger_of(const ideal_basis& b)
{
    ideal_basis result = b;
    buchberger(result);
    return result;
}

extern void reduce_mod(polynomial& p, const polynomial& q);
polynomial polymod(const polynomial& p, const polynomial& q)
{
    polynomial result = p;
    reduce_mod(result, q);
    return result;
}

TEST(Buchberger, ReduceMod)
{
    EXPECT_EQ(polymod({}, {}), (polynomial {}));
    EXPECT_EQ(polymod({ 1, 2, 3 }, {}), (polynomial { 1, 2, 3 }));
    EXPECT_EQ(polymod({}, { 1, 2, 3 }), (polynomial {}));
    EXPECT_EQ(polymod({ 1, 2, 3 }, { 1, 2, 3 }), (polynomial {}));
    EXPECT_EQ(polymod({ 1, 2, 3, 0 }, { 1, 2, 3 }), (polynomial {}));
    EXPECT_EQ(polymod({ 1, 3, 5, 3 }, { 1, 2, 3 }), (polynomial {}));
    EXPECT_EQ(polymod({ 1, 2, 3 }, { 1, -2 }), (polynomial { 11 }));
    EXPECT_EQ(polymod({ 1, 2, 4, 6 }, { 2, 1, 3 }), (polynomial { 1, 0, 3, 3 }));
    EXPECT_EQ(polymod({ 4, 2, 6, 9 }, { 2, 1, 3 }), (polynomial { 9 }));
    EXPECT_EQ(polymod({ 1, -3 }, { 2 }), (polynomial { 1, 1 }));
}

TEST(Buchberger, Buchberger)
{
    EXPECT_EQ(buchberger_of({}), ideal_basis {});
    EXPECT_EQ(buchberger_of({ {} }), ideal_basis {});
    EXPECT_EQ(buchberger_of({ { 1, 3, 2 }, { 4, 4 } }),
        (ideal_basis { { 1, 3, 2 }, { 4, 4 } }));
    EXPECT_EQ(buchberger_of({ { 1, 3, 2 }, { 4, 4 }, {} }),
        (ideal_basis { { 1, 3, 2 }, { 4, 4 } }));
    EXPECT_EQ(buchberger_of({ { 1, -3, 2 }, { 4, -4 } }),
        (ideal_basis { { 1, 1, -2 }, { 4, -4 } }));
    EXPECT_EQ(buchberger_of({ { -1, -3, -2 }, { 4, 4 } }),
        (ideal_basis { { 1, 3, 2 }, { 4, 4 } }));
    EXPECT_EQ(buchberger_of({ { 16 }, { 10 } }),
        (ideal_basis { { 2 } }));
    EXPECT_EQ(buchberger_of({ { 10, 0 }, { 16 } }),
        (ideal_basis { { 2, 0 }, { 16 } }));
    EXPECT_EQ(buchberger_of({ { 16, 0 }, { 10 } }),
        (ideal_basis { { 2, 0 }, { 10 } }));
    EXPECT_EQ(buchberger_of({ { 10, -20 }, { 16 } }),
        (ideal_basis { { 2, 12 }, { 16 } }));
    EXPECT_EQ(buchberger_of({ { 1, 16 }, { 1, 10 } }),
        (ideal_basis { { 1, 4 }, { 6 } }));
    EXPECT_EQ(buchberger_of({ { 1, 1, 0 }, { 4, -3 } }),
        (ideal_basis { { 1, 15 }, { 21 } }));
    EXPECT_EQ(buchberger_of({ { 1, 1, 0 }, { 4, -4 } }),
        (ideal_basis { { 1, 1, 0 }, { 4, 4 }, { 8 } }));
    EXPECT_EQ(buchberger_of({ { 1, 1, 0 }, { 4, 0 } }),
        (ideal_basis { { 1, 1, 0 }, { 4, 0 } }));
    EXPECT_EQ(buchberger_of({ { 1, 1, 0 }, { 4, 4 } }),
        (ideal_basis { { 1, 1, 0 }, { 4, 4 } }));

    // representation of <2+3i> in Z[i]
    EXPECT_EQ(buchberger_of({ { 1, 0, 1 }, { 3, 2 } }),
        (ideal_basis { { 1, 5 }, { 13 } }));

    // representation of <2, -1+sqrt(-5)> in Z[sqrt(-5)]
    EXPECT_EQ(buchberger_of({ { 1, 0, 5 }, { 1, -1 }, { 2 } }),
        (ideal_basis { { 1, 1 }, { 2 } }));
    // representation of <3, -1+sqrt(-5)> in Z[sqrt(-5)]
    EXPECT_EQ(buchberger_of({ { 1, 0, 5 }, { 1, -1 }, { 3 } }),
        (ideal_basis { { 1, 2 }, { 3 } }));

#ifdef RUN_EXPENSIVE_TESTS
    // This might be a good test case to run if making changes to the
    // algorithm, to check that this still runs in a reasonable time.
    EXPECT_EQ(buchberger_of({ { 1, 0, 0, 5, 3, 0, 0, 0, 0, 0, 9, 0, 0, -3, 0, -1, 0, 0, 0, 0, 0, 0 },
                  { 3, 0, 1, 4, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 1 } }),
        (ideal_basis { { 1, 7747110435841547256507133_Z },
            { 11529190147322608601758016_Z } }));
#endif // RUN_EXPENSIVE_TESTS
}
