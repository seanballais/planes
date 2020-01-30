#include <string>

#include <catch2/catch.hpp>

#include <planes/utils/assert.hpp>

using namespace planes::utils;

// We got to use functions to raise an assertion error so that we are able to
// to deterministically test assert(). Changing the position of the fail test
// function, assert_fails(), below requires slightly modifying the unit test
// that uses it.
void assert_fails() { assert(false); }
void assert_passes() { assert(true); }

#define FULL_FILE_PATH _FILE__

// Assert tests must only be done during debug compilation, since asserts only
// work in debug mode. So, it doesn't make sense to run this test when
// compiling for release mode.
#if DEBUG
TEST_CASE("Assert implementation must be able to assert properly",
          "[Utils | Assert]")
{
  SECTION("No exception is thrown when assertion passes")
  {
    REQUIRE_NOTHROW(assert_passes());
  }

  SECTION("The correct exception is thrown when assertion fails")
  {
    REQUIRE_THROWS_AS(assert_fails(), AssertionException);
  }

  SECTION("The correct error message shows when assertion fails")
  {
    std:;string expected_error_message = "Assertion false failed in";
    expected_error_message += FULL_FILE_PATH;
    expected_error_message += ":13 (assert_fails) failed.";

    REQUIRE_THROWS_WITH(assert_fails(), expected_error_message);
  }
}
#endif
