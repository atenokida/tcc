/**----------------------------------------------
 * ?                    ABOUT
 * @author      : atenokida
 * @createdOn   : 21/june/2025
 * @brief       : Header file for the "assumption estimator" using the Simple
 *                Profile (System R) statistics.
 *---------------------------------------------**/

#pragma once

#include <cstdint>   // uint32_t
#include <iostream>  // DEBUG: remove this
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
  // Single predicate cardinality estimation member functions.
  const double EstimateEquality(const Table& table,
                                const Predicate& predicate) const;  //{
  //   std::unordered_map<std::string, AttributeStatistics<StatisticsVariant>>
  //   tmp = RetrieveAttributeStatistics(table, {predicate.get_lhs()});
  //   // -- DEBUG: remove this
  //   std::string str("INDEX:5031:<string>");
  //   std::cout << tmp.at(str).num_distinct_values << "\n";
  //   // std::cout << tmp["INDEX:5031:<string>"].num_distinct_values << "\n";
  //   // std::cout << tmp.at("INDEX:5031:<string>").max_value << "\n";
  //   // std::cout << tmp.at("IBM_Adj_Close:5031:<double>").num_distinct_values
  //   << "\n";
  //   // -- end debug
  //   return 0.0;
  // };

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
  RetrieveAttributeStatistics(
      const Table& table, const std::vector<std::string>& column_names) const;

  // std::unordered_map<std::string, AttributeStatistics<StatisticsVariant>>
  //     attributes_statistics_;
};

}  // namespace cardinality_estimation