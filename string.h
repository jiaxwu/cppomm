#include <string>
#include <vector>
#include <ranges>

namespace cppomm {
namespace string {

// 切分字符串
std::vector<std::string> Split(const std::string& s, char delim) {
    std::vector<std::string> tokens;
    size_t lastPos = s.find_first_not_of(delim, 0);
    size_t pos = s.find(delim, lastPos);
    while (lastPos != std::string::npos) {
        tokens.emplace_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delim, pos);
        pos = s.find(delim, lastPos);
    }
    return tokens;
}

} // namespace string
} // namespace cppomm
