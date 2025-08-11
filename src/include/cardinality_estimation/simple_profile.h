/**----------------------------------------------
 * ?                    ABOUT
 * @author      : atenokida
 * @createdOn   : 21/june/2025
 * @brief       : Header file for the "assumption estimator" using the Simple
 *                Profile (System R) statistics.
 *---------------------------------------------**/

#pragma once

#include <cstdint>  // uint32_t
#include <unordered_map>
#include <variant>
#include <vector>

#include "base_estimator.h"
#include "predicate.h"
#include "table.h"

namespace cardinality_estimation {

template <typename T>
struct AttributeStatistics {
  uint32_t num_distinct_values;
  T min_value;
  T max_value;
};

class SimpleProfile {
 public:
  // `column_names` is the list of attributes that will be used to store statistics.
  SimpleProfile(const Table& table,
                const std::vector<std::string>& column_names)
      : attributes_statistics_(
            RetrieveAttributeStatistics(table, column_names)) {}

  // This function returns the cardinality estimation, i.e., the expected number
  // of tuples that will be emitted by running the predicate on the table.
  const double EstimateEquality(const Table& table,
                                const Predicate& predicate) const;

  // For complex predicates, the approach is to estimate each
  // predicate independently and then compute the final estimation by using
  // the independence assumption, i.e., assume there is no correlation between
  // attributes. e.g., "t.A = t'.A ∧ t.B = t'.B" is estimated by the formula:
  // sel(t.A = t'.A) x sel(t.B = t'.B), where sel(pred) is the selectivity of
  // the predicate pred.
  // Note that:
  //  - selectivity: the fraction of tuples that qualifies the predicate;
  //  - cardinality: the number of rows will be emitted (i.e., are in the
  //                 resultant set) by the querying predicate.
  // This function returns the cardinality estimation, i.e., the expected number
  // of tuples that will be emitted by running the predicate on the table.
  const double EstimateEquality(const Table& table,
                                const std::vector<Predicate>& predicates) const;

 private:
  using StatisticsVariant = std::variant<int, double, std::string>;

  // Retrieves a map of AttributeStatistics for each specified
  // column that is available in the table.
  std::unordered_map<std::string, AttributeStatistics<StatisticsVariant>>
  RetrieveAttributeStatistics(
      const Table& table, const std::vector<std::string>& column_names) const;

  const std::unordered_map<std::string, AttributeStatistics<StatisticsVariant>>
      attributes_statistics_;
};

}  // namespace cardinality_estimation