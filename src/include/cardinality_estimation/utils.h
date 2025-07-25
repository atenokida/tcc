#pragma once

#include <stdexcept> // runtime_error
#include <string>
#include <unordered_map>

#include "cardinality_estimation/predicate.h"
#include "cardinality_estimation/table.h"

namespace cardinality_estimation {

class Utils {
 public:
  static std::unordered_map<std::string, std::string> ParseCSVHeader(
      const std::string& filename);  // # DEBUG: remove this
  static inline void ValidatePredicate(const Table& table,
                                       const Predicate& predicate) {
    if (!IsPredicateValid(table, predicate)) {
      throw std::runtime_error("Invalid predicate: " + predicate.ToString());
    }
  };

 private:
  static inline const bool IsPredicateValid(const Table& table,
                                            const Predicate& predicate) {
    if (!table.has_column(predicate.get_lhs()) ||
        !table.has_column(predicate.get_rhs()))
      return false;

    return true;
  };
};

}  // namespace cardinality_estimation