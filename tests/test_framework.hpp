#ifndef TEST_FRAMEWORK_HPP
#define TEST_FRAMEWORK_HPP

// A tiny, dependency-free unit-test framework.
//
// Usage:
//   TEST_CASE(name_of_test) { CHECK(cond); CHECK_EQ(a, b); }
// Then link against test_framework.cpp (which provides main()).

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace testfw
{
	struct TestCase
	{
		std::string name;
		void (*fn)();
	};

	// Global state, defined in test_framework.cpp.
	std::vector<TestCase>& registry();
	int& currentFailures();
	int& totalChecks();

	struct Registrar
	{
		Registrar(const std::string& name, void (*fn)())
		{
			TestCase tc;
			tc.name = name;
			tc.fn = fn;
			registry().push_back(tc);
		}
	};

	inline void reportFailure(const std::string& file, int line, const std::string& msg)
	{
		++currentFailures();
		std::cout << "    [FAIL] " << file << ":" << line << ": " << msg << std::endl;
	}
}

#define TEST_CASE(test_name)                                                      \
	static void test_name();                                                      \
	static testfw::Registrar registrar_##test_name(#test_name, &test_name);       \
	static void test_name()

#define CHECK(cond)                                                               \
	do {                                                                          \
		++testfw::totalChecks();                                                  \
		if (!(cond)) {                                                            \
			testfw::reportFailure(__FILE__, __LINE__, "CHECK(" #cond ") failed"); \
		}                                                                         \
	} while (0)

#define CHECK_EQ(a, b)                                                            \
	do {                                                                          \
		++testfw::totalChecks();                                                  \
		if (!((a) == (b))) {                                                      \
			std::ostringstream _oss;                                              \
			_oss << "CHECK_EQ(" #a ", " #b ") failed: "                           \
			     << "lhs=" << (a) << " rhs=" << (b);                              \
			testfw::reportFailure(__FILE__, __LINE__, _oss.str());                \
		}                                                                         \
	} while (0)

#endif // TEST_FRAMEWORK_HPP
