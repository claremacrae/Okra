#pragma once

#define AssertEqual(t1, t2) AssertEqual_((t1), (t2), #t1 " == " #t2)
template<class T>
void AssertEqual_(const T& t1, const T& t2, string message)
{
    if (t1 != t2)
    {
        cout << message << " - assert FAILED - " << t1 << " != " << t2 << endl;
    }
}

filesystem::path format_path_for_display(const filesystem::path& base, filesystem::path file)
{
    return filesystem::path(file.replace_extension().string().substr(base.string().length() + 1));
}

struct Example
{
    filesystem::path file;
    string name;
    function<void(void)> body;

    void Run(filesystem::path base) const
    {
        cout << format_path_for_display(base, file) << endl;
        cout << name << endl;
        body();
        cout << endl;
    }
};

filesystem::path GetCommonRootTwo(const filesystem::path& path1, const filesystem::path& path2)
{
    filesystem::path common;
    auto iter1 = path1.begin();
    auto iter2 = path2.begin();
    while (iter1 != path1.end() && iter2 != path2.end())
    {
        if (*iter1 != *iter2) { break; }
        common /= *iter1;
        iter1++;
        iter2++;
    }
    
    return common;
}

filesystem::path GetCommonRootMany(vector<filesystem::path> paths)
{
    return accumulate(
        next(paths.begin()),
        paths.end(),
        paths.front().remove_filename(),
        GetCommonRootTwo);
}

class Examples
{
    vector<Example> examples;

public:
    void Add(Example example) { examples.push_back(example); }

    static filesystem::path GetCommonFileRoot(const vector<Example>& examples)
    {
        vector<filesystem::path> paths;
        transform(examples.cbegin(), examples.cend(), back_inserter(paths), [](const Example& _) {return _.file; });
        return GetCommonRootMany(paths);
    }

    void RunAll() const
    {
        auto maximumSharedFilePrefix = GetCommonFileRoot(examples);
        for (const auto& example : examples)
        {
            example.Run(maximumSharedFilePrefix);
        }
    }
};

Examples allExamples;

#define Example(name) Example_(name, __COUNTER__)
#define Example_(name, counter) Example__(name, counter)
#define Example__(name, counter) Example___(name, Example##counter, ExampleInitializer##counter)
#define Example___(name, initializerName, bodyName)                                     \
    void bodyName();                                                                    \
    struct initializerName                                                              \
    {                                                                                   \
        initializerName()                                                               \
        {                                                                               \
            allExamples.Add({__FILE__, name, bodyName});                                \
        }                                                                               \
    } initializerName##Instance;                                                        \
    void bodyName()
