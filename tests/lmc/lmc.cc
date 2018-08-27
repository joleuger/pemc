#include<gtest/gtest.h>

#include "pemc/lmc/lmc.h"

using namespace pemc;


TEST(lmc_test, lmc_is_constructed) {
    Lmc lmc;

    ASSERT_EQ(lmc.getStates().size(), 0) << "FAIL";
    ASSERT_EQ(lmc.getTransitions().size(), 0) << "FAIL";
}
