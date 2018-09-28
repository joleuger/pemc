#include<gtest/gtest.h>

#include "pemcCpp/cppFormula.h"

TEST(string_test, strings_are_equal) {
    ASSERT_EQ("a", "a") << "FAIL";
    ASSERT_EQ("a", "a") << "FAIL";
}
