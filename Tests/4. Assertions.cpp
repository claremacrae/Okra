#include "Okra.h"

class StringListener : public okra::IListener
{
public:
	std::vector<okra::TestInfo> starts;
	void OnStart(const okra::TestInfo &testInfo) override { starts.push_back(testInfo); }

	std::vector<okra::TestInfo> ends;
	void OnEnd(const okra::TestInfo &testInfo, std::chrono::high_resolution_clock::duration duration) override
	{
		ends.push_back(testInfo);
	}

	std::vector<std::string> fails;
	void OnFail(const std::string &message) override { fails.push_back(message); }
};

TEST("FAIL")
{
	const okra::TestInfo subject{"", []() { FAIL("blah"); }};

	const auto string_listener = std::make_shared<StringListener>();
	ASSERT(!subject.Run({string_listener}));
	ASSERT_EQUAL(1, string_listener->fails.size());
	ASSERT_EQUAL(":  - assert FAILED - \nblah\n", string_listener->fails[0]);
}

TEST("ASSERT fail")
{
	const okra::TestInfo subject{"", []() { ASSERT(true); }};

	const auto string_listener = std::make_shared<StringListener>();
	ASSERT(subject.Run({string_listener}));
	ASSERT_EQUAL(0, string_listener->fails.size());
}

TEST("ASSERT fail")
{
	const okra::TestInfo subject{"", []() { ASSERT(false); }};
	const auto string_listener = std::make_shared<StringListener>();
	ASSERT(!subject.Run({string_listener}));
	ASSERT_EQUAL(1, string_listener->fails.size());
	ASSERT_EQUAL(":  - assert FAILED - \nfalse\n", string_listener->fails[0]);
}
