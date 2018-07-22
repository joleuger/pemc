#include<gtest/gtest.h>

#include "pemc/lmc/lmcModelChecker.h"

using namespace pemc;


TEST(lmcModelChecker_test, lmc_is_constructed) {
    Lmc lmc;

    ASSERT_EQ(lmc.initialTransitionFrom, 0) << "FAIL";
    ASSERT_EQ(lmc.initialTransitionCount, 0) << "FAIL";
    ASSERT_EQ(lmc.states.size(), 0) << "FAIL";
    ASSERT_EQ(lmc.transitions.size(), 0) << "FAIL";
}
