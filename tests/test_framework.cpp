#include "test_framework.hpp"

namespace testfw
{
	std::vector<TestCase>& registry()
	{
		static std::vector<TestCase> r;
		return r;
	}

	int& currentFailures()
	{
		static int f = 0;
		return f;
	}

	int& totalChecks()
	{
		static int c = 0;
		return c;
	}
}

int main()
{
	using namespace testfw;

	int failedTests = 0;
	int passedTests = 0;

	std::cout << "Running " << registry().size() << " test case(s)..." << std::endl;

	for (size_t i = 0; i < registry().size(); ++i)
	{
		const TestCase& tc = registry()[i];
		int before = currentFailures();
		std::cout << "  - " << tc.name << std::endl;
		tc.fn();
		if (currentFailures() == before)
		{
			++passedTests;
		}
		else
		{
			++failedTests;
		}
	}

	std::cout << "\n============================================" << std::endl;
	std::cout << "Test cases: " << passedTests << " passed, "
	          << failedTests << " failed (" << registry().size() << " total)" << std::endl;
	std::cout << "Checks:     " << totalChecks() << " total, "
	          << currentFailures() << " failed" << std::endl;
	std::cout << "============================================" << std::endl;

	return (failedTests == 0) ? 0 : 1;
}
