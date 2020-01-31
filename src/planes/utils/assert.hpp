#ifndef PLANES_UTILS_ASSERT_HPP
#define PLANES_UTILS_ASSERT_HPP

#ifdef DEBUG
#include <stdexcept>
#include <sstream>
#include <string>

// Custom assert function based from the assertion function in
// https://github.com/catchorg/Catch2/issues/553#issuecomment-163741540
//
// We need a custom assert function so that we can still assert but yet still
// be able to perform testing with Catch 2. Using the standard assert will
// mean more work for us just so that assert and Catch 2 play nicely with
// each other. Catch 2 is, by default, unable to catch assert failures. The
// test app will just immmediately terminate, thus Catch's behaviour. Using
// a custom assert function will provide us a simpler way to work with Catch 2
// without having to resort to a more complicated approach discussed in the
// following link:
// https://github.com/catchorg/Catch2/issues/553
namespace planes::utils {
  class AssertionError : public std::runtime_error {}

  inline void assert_impl(bool hasAssertPassed, std::string expr,
                          std::string file, std::string func, int line)
  {
    if (!hasAssertPassed) {
      std::stringstream msg;
      msg << "Assertion " << expr << " failed in " << file << ":" << line
          << " (" << func << ") failed.";

      throw AssertionError(msg.str());
    }
  }

  #define ASSERT(assertion) assert_impl((assertion), #assertion,
                                        __FILE__, __func__, __LINE__)
}
#else
namespace planes::utils {
  #define ASSERT(assertion) ((void)0)
}
#endif
#endif