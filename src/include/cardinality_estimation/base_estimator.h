/**----------------------------------------------
 * ?                    ABOUT
 * @author      : atenokida
 * @createdOn   : 21/june/2025
 * @brief       : Header file base declarations for estimators.
 *---------------------------------------------**/

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "predicate.h"
#include "table.h"

namespace cardinality_estimation {

// This class provides a common interface for all estimators.
class BaseEstimator {
 public:
  // Single predicate cardinality estimation member functions.
  virtual const double EstimateEquality(const Table& table,
                                        const Predicate& predicate) const = 0;

  // Complex predicate (multiple predicates) cardinality estimation member
  // functions.
  virtual const double EstimateEquality(
      const Table& table, std::vector<Predicate>& predicates) const = 0;
};

}  // namespace cardinality_estimation