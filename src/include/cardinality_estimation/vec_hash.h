#include <vector>

namespace std {

// Specialization for vector<size_t>.
template <>
struct hash<vector<size_t>> {
  // Solution from: https://stackoverflow.com/a/72073933
  size_t operator()(const vector<size_t>& vec) const {
    std::size_t seed = vec.size();
    for (auto x : vec) {
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = (x >> 16) ^ x;
      seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

}  // namespace std