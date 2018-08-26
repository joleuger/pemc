#include<gtest/gtest.h>


TEST(string_test, strings_are_equal) {
    ASSERT_EQ("a", "a") << "FAIL";
    ASSERT_EQ("a", "a") << "FAIL";
}

#include "pemc/basic/configuration.h"

using namespace pemc;

TEST(configuration_test, output_works) {
    Configuration config;
    *config.cout << ("configuration prints");
}
