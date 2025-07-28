/**----------------------------------------------
 * ?                    ABOUT
 * @author      : atenokida
 * @createdOn   : 21/june/2025
 * @brief       : Header file for the "assumption estimator" using the Simple
 *                Profile (System R) statistics.
 *---------------------------------------------**/

#pragma once

#include <cstdint>   // uint32_t
#include <unordered_map>
#include <variant>
#include <vector>

#include "base_estimator.h"

namespace cardinality_estimation {

template <typename T>
struct AttributeStatistics {
  uint32_t num_distinct_values;
  T min_value;
  T max_value;
};

class SimpleProfile : public BaseEstimator {
 public:
  // Constructor.
  // column_names is the list of attributes that will be used to store statistics.
  SimpleProfile(const Table& table, const std::vector<std::string>& column_names)
      : attributes_statistics_(RetrieveAttributeStatistics(table, column_names)) {}

  // Single predicate cardinality estimation member functions.
  const double EstimateEquality(const Table& table,
                                const Predicate& predicate) const;

  // Complex predicate (multiple predicates) cardinality estimation member
  // functions.
  const double EstimateEquality(
      const Table& table, std::vector<const Predicate>& predicates) const {
    return 0.0;
  };

 private:
  using StatisticsVariant = std::variant<int, double, std::string>;

  // Retrieves a map of AttributeStatistics for each specified
  // column that is available in the table.
  std::unordered_map<std::string, AttributeStatistics<StatisticsVariant>>
  RetrieveAttributeStatistics(const Table& table, const std::vector<std::string>& column_names) const;

  const std::unordered_map<std::string, AttributeStatistics<StatisticsVariant>> attributes_statistics_;
};

}  // namespace cardinality_estimation