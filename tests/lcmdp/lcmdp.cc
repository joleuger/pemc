#include<gtest/gtest.h>

#include "pemc/lcmdp/lcmdp.h"

using namespace pemc;


TEST(lcmdp_test, lcmdp_is_constructed) {
    Lcmdp lcmdp;

    ASSERT_EQ(lcmdp.initialTransitionFrom, 0) << "FAIL";
    ASSERT_EQ(lcmdp.initialTransitionCount, 0) << "FAIL";
    ASSERT_EQ(lcmdp.states.size(), 0) << "FAIL";
    ASSERT_EQ(lcmdp.transitions.size(), 0) << "FAIL";
}
