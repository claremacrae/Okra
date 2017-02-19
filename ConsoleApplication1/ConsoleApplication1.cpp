#include "stdafx.h"

#include "Okra.h"

Example("common root of two related paths")
{
    auto result = GetCommonRootTwo(R"(C:\foo\bar.cpp)", R"(C:\foo\qux\baz.cpp)");
    AssertEqual(result.string(), string(R"(C:\foo)"));
}

Example("common root of vector of paths")
{
    auto result = GetCommonRootMany({ R"(C:\foo\bar.cpp)", R"(C:\foo\qux\baz.cpp)" });
    AssertEqual(result.string(), string(R"(C:\foo)"));
}

Example("returns the given path relative to the base")
{
    auto result = make_path_relative(R"(C:\foo)", R"(C:\foo\qux\baz.cpp)");
    AssertEqual(result.string(), string(R"(qux\baz.cpp)"));
}

int main(int argc, char** argv)
{
    allExamples.RunAll();
    return 0;
}