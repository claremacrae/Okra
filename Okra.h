#pragma once

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

#include <experimental/filesystem>

namespace okra
{
	template <class T, class U>
	void AssertEqual(const T &t, const U &u, const std::string &message, bool &pass)
	{
		pass = true;
		if (t != u) {
			std::cout << ": " << message << " - assert FAILED - " << t << " != " << u << std::endl;
			pass = false;
		}
	}

	struct ExampleInfo
	{
		std::experimental::filesystem::path file;
		std::string name;
		std::function<void(bool &)> body;

		bool Run() const;
	};

	namespace internals
	{
		template <typename TClock>
		std::pair<long long, bool>
		time_to_execute_microseconds(const std::function<void(bool &OKRA_pass)> &operation)
		{
			auto begin = TClock::now();
			bool pass;
			operation(pass);
			auto end = TClock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			return {duration.count(), pass};
		}
	}

	bool ExampleInfo::Run() const
	{
		std::cout << name;
		bool pass;
		long long execution_time_us;
		std::tie(execution_time_us, pass) =
		    internals::time_to_execute_microseconds<std::chrono::high_resolution_clock>(body);
		std::cout << " (" << (execution_time_us / 1000.0) << " ms)" << std::endl;
		return pass;
	}

	namespace internals
	{
		class Examples
		{
			std::vector<ExampleInfo> examples;

		public:
			void Add(ExampleInfo exampleInfo) { examples.push_back(exampleInfo); }

			bool RunAll() const
			{
				if (examples.empty()) {
					return false;
				}
				bool pass = true;
				for (const auto &exampleInfo : examples) {
					pass &= exampleInfo.Run();
				}
				return pass;
			}
		};

		Examples allExamples;
	} // namespace internals
} // namespace okra

#define OKRA_EXAMPLE(name) OKRA_EXAMPLE_(name, __COUNTER__)
#define OKRA_EXAMPLE_(name, counter) OKRA_EXAMPLE__(name, counter)
#define OKRA_EXAMPLE__(name, counter) OKRA_EXAMPLE___(name, EXAMPLE_##counter, ExampleInitializer##counter)
#define OKRA_EXAMPLE___(name, bodyName, initializerName)                                                               \
	void bodyName(bool &OKRA_pass);                                                                                \
	struct initializerName                                                                                         \
	{                                                                                                              \
		initializerName() { okra::internals::allExamples.Add({__FILE__, name, bodyName}); }                    \
	} initializerName##Instance;                                                                                   \
	void bodyName(bool &OKRA_pass)

#ifndef OKRA_DO_NOT_DEFINE_EXAMPLE
#define EXAMPLE OKRA_EXAMPLE
#endif

#define OKRA_ASSERT_EQUAL(t1, t2) okra::AssertEqual((t1), (t2), #t1 " == " #t2, OKRA_pass)

#ifndef OKRA_DO_NOT_DEFINE_ASSERT_EQUAL
#define ASSERT_EQUAL OKRA_ASSERT_EQUAL
#endif

int main(int argc, char **argv) { return okra::internals::allExamples.RunAll() ? 0 : 1; }
