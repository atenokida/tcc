/**----------------------------------------------
 * ?                    ABOUT
 * @author      : atenokida
 * @createdOn   : 29/june/2025
 * @brief       : Implementation file of `tug_of_war.h`.
 *---------------------------------------------**/

#include "cardinality_estimation/tug_of_war.h"

#include <algorithm>  // sort
#include <cstring>  // memset
#include <iterator>  // ostream_iterator
#include <memory>  // shared_ptr, unique_ptr, make_unique
#include <stdexcept>  // invalid_argument, runtime_error
#include <type_traits>  // is_same

#include "MurmurHash.h"

namespace cardinality_estimation {

TugOfWar::TugOfWar(const unsigned int depth, 
                  const unsigned int width, 
                  const uint32_t hash_seed)
  : depth_(depth),
    width_(width / 4 / depth), // !WARNING - No validation for division by 0
    hash_seed_(hash_seed)
{
  if (depth > kToWMaxHashNum)
    throw std::invalid_argument("`depth` must not exceed kToWMaxHashNum\n");

  for (size_t i = 0; i < depth_; ++i) {
    counter_[i] = std::shared_ptr<int[]>(new int[width_]);
    memset(counter_[i].get(), 0, sizeof(width_) * width_);
  }
}

const double TugOfWar::EstimateEquality(
  const Table& table, 
  const Predicate& predicate)
{ 
  // Since Tug-of-War only estimate col. homogeneous predicates
  // it doesn't matter which attribute we get.
  std::string column_name = predicate.lhs();

  if (!table.has_column(column_name))
    throw std::runtime_error("Column not found: " + column_name);

  const auto& column = table.get_column(column_name);

  // Build the sketch.
  std::visit(
    [this](const auto& arg) {
      using T = std::decay_t<decltype(arg)>;

      if constexpr (std::is_same_v<T, xt::xarray<double>> ||
                    std::is_same_v<T, xt::xarray<int>> ||
                    std::is_same_v<T, xt::xarray<std::string>>) {
        for (const auto& value : arg) {
          // Insert value into summary
          Update(static_cast<const void*>(&value));
        }
      }

    }, 
    column);
  
  // Estimate
  return F2Est(*this);
}

const double TugOfWar::EstimateEquality(
  const Table& table, 
  const std::vector<Predicate>& predicates)
{
  // implement me
  return 0.0;
}

// This is an adaptation of the `Insert()`
// function taken from the JoinSketch paper implementation.
void TugOfWar::Update(const void* str)
{
  unsigned g = 0;

  // Each row (depth) is a separate estimator.
  // We run multiple estimators to reduce the variance.
  for (size_t i = 0; i < depth_; ++i) {
    for (size_t j = 0; j < width_; ++j) {

      if (!g)
        g = ((unsigned)MurmurHash32(str, kToWKeyLen, hash_seed_ + i * j));

      if (g & 1)
        counter_[i][j]++;
      else
        counter_[i][j]--;
      
      g >>= 1;
    }
  }
}

// This implementation is an adaptation of both G. Cormode implementation
// and JoinSketch paper implementation.
const double TugOfWar::F2Est(const TugOfWar& sketch)
{
  auto depth = sketch.depth_;
  std::unique_ptr<long long[]> estimates = std::make_unique<long long[]>(depth);

  // Estimate F_2 (second frequency moment) for all estimators
  // and compute the mean of each group.
  for (size_t i = 0; i < depth; ++i) {
    long double z = 0;
    for (size_t j = 0; j < sketch.width_; ++j) {
      z += 1ll * sketch.counter_[i][j] * sketch.counter_[i][j];
    }
    estimates[i] = 1.0 * z / sketch.width_;
  }

  // Return the median of all groups.
  std::sort(estimates.get(), estimates.get() + depth);
  if (depth % 2 != 0)
    return (double)estimates[depth/2];

  return (double)(estimates[(depth-1)/2] + estimates[depth/2]) / 2.0;
}

}  // namespace cardinality_estimation