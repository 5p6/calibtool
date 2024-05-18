#include "utility.h"


namespace utility
{
    namespace internal
    {
        // 排序
        class StringSort
        {
        public:
            bool operator()(const std::string &a, const std::string &b)
            {
                return std::stoi(a.substr(0, a.size() - 4)) < std::stoi(b.substr(0, b.size() - 4));
            }
        };
    }

    //
    std::vector<std::string> getFilesFromPath(const std::string &path)
    {
        std::vector<std::string> paths;
        for (const auto &entry : std::filesystem::directory_iterator(path))
        {
            if (std::filesystem::is_regular_file(entry))
            {
                paths.push_back(entry.path().string());
            }
        }

        std::sort(paths.begin(), paths.end(), internal::StringSort());
        return paths;
    }

      //
    std::vector<std::string> getFilesFromPathWithoutSort(const std::string &path)
    {
        std::vector<std::string> paths;
        for (const auto &entry : std::filesystem::directory_iterator(path))
        {
            if (std::filesystem::is_regular_file(entry))
            {
                paths.push_back(entry.path().string());
            }
        }

        return paths;
    }
}