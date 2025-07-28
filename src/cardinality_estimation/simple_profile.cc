/**----------------------------------------------
 * ?                    ABOUT
 * @author      : atenokida
 * @createdOn   : 21/june/2025
 * @brief       : Implementation file for `simple_profile.h`.
 *---------------------------------------------**/

#include "cardinality_estimation/simple_profile.h"

#include <algorithm>  // max, max_element, min_element
#include <cmath>      // pow
#include <cstdint>    // uint32_t
#include <stdexcept>  // runtime_error
#include <string>
#include <unordered_map>
#include <utility>  // make_pair
#include <vector>

#include "cardinality_estimation/table.h"
// #include "cardinality_estimation/utils.h" // temporarily removed
#include "xtensor.hpp"

namespace cardinality_estimation {

const double SimpleProfile::EstimateEquality(const Table& table,
                                             const Predicate& predicate) const
{
  // Estimation of R⋈S: (nR * nS) / max(V(A,R), V(B,S)), where:
  //   - nR: number of rows in R;
  //   - nS: number of rows in S;
  //   - V(A,R): number of distinct values of attribute A in R;
  //   - V(B,S): number of distinct values of attribute B in S;
  // For more information, see text Sec. 2.3.2.
  // Since our experiments are for self-joins, the size of relations R and S are
  // always the same.
  const size_t relation_size = table.num_rows();
  const auto attrs_stats = RetrieveAttributeStatistics(
      table, {predicate.get_lhs(), predicate.get_rhs()});

  // There are several ways of getting the number of distinct values of a
  // attribute:
  //   - column index (System R);
  //   - histograms;
  //   - sketches (e.g. HLL);
  //   - estimations (cf. text Sec. 2.3.2).
  // We will use for the following experiment the approach of System R.
  // TODO: implement other approaches.
  const double attr_card_lhs = static_cast<double>(
      attrs_stats.at(predicate.get_lhs()).num_distinct_values);
  const double attr_card_rhs = static_cast<double>(
      attrs_stats.at(predicate.get_rhs()).num_distinct_values);

  const double relation_size_d = static_cast<double>(relation_size);
  const double result = (relation_size_d * relation_size_d) /
                        std::max(attr_card_lhs, attr_card_rhs);

  return result;
}

std::unordered_map<std::string,
                   AttributeStatistics<SimpleProfile::StatisticsVariant>>
SimpleProfile::RetrieveAttributeStatistics(
    const Table& table, const std::vector<std::string>& column_names) const
{
  std::unordered_map<std::string,
                     AttributeStatistics<SimpleProfile::StatisticsVariant>>
      result;

  for (const auto& column_name : column_names) {
    try {
      // Check if the column exists in the table
      if (!table.has_column(column_name)) {
        throw std::runtime_error("Column not found: " + column_name);
      }

      // Check if the column is already in the result map
      if (result.find(column_name) != result.end()) {
        continue;
      }

      // Get column data from the table
      const auto& column = table.get_column(column_name);

      // std::visit takes a variant and a set of
      // functions, and calls the correct function based on the type the variant
      // is holding at the time.
      // The square brackets denotes to the compiler which variables from the
      // outer scope should be captured.
      std::visit(
          [&result, &column_name](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            AttributeStatistics<SimpleProfile::StatisticsVariant> stats;

            if constexpr (std::is_same_v<T, xt::xarray<double>>) {
              stats.min_value = static_cast<double>(xt::amin(arg)());
              stats.max_value = static_cast<double>(xt::amax(arg)());
            } else if constexpr (std::is_same_v<T, xt::xarray<int>>) {
              stats.min_value = static_cast<int>(xt::amin(arg)());
              stats.max_value = static_cast<int>(xt::amax(arg)());
            } else if constexpr (std::is_same_v<T, xt::xarray<std::string>>) {
              // Lexicographical sort
              stats.min_value = static_cast<std::string>(
                  *std::min_element(arg.begin(), arg.end()));
              stats.max_value = static_cast<std::string>(
                  *std::max_element(arg.begin(), arg.end()));
            } else {
              throw std::runtime_error("Unsupported column type");
            }

            // We currently support only fundamental (primitive) types.
            // The usage of size() in this scenario will not
            // result in errors since all vectors are unidimensional (i.e.,
            // there isn't a column with a multidimensional data type).
            stats.num_distinct_values =
                static_cast<uint32_t>(xt::unique(arg).size());
            result.insert(std::make_pair(column_name, stats));
          },
          column);

      // result.insert(std::make_pair(column_name, stats));
    } catch (const std::runtime_error& e) {
      throw std::invalid_argument("Column not found: " + column_name);
    }
  }

  return result;
}

}  // namespace cardinality_estimation