/**----------------------------------------------
 * ?                    ABOUT
 * @author      : atenokida
 * @createdOn   : 17/october/2024
 * @lastUpdate  : 21/june/2025
 * @brief       : Header file for query predicate.
 *---------------------------------------------**/

#pragma once

#include <string>

namespace cardinality_estimation {

// Enum class to represent the different types of operators in a predicate.
enum class Operator {
  kEqual,         // ==
  kNotEqual,      // != or <>
  kLess,          // <
  kLessEqual,     // <=
  kGreater,       // >
  kGreaterEqual,  // >=
};

// This class represents a predicate in a query.
// A predicate consists of a left-hand side (lhs), an operator and a right-hand
// side (rhs). The operator can be one of the following: ==, != (or <>), <, <=,
// > and >=.
class Predicate {
  // Constructor.
 public:
  // https://www.geeksforgeeks.org/when-do-we-use-initializer-list-in-c
  Predicate(const std::string &lhs, const std::string &op,
            const std::string &rhs)
      : lhs_(lhs), rhs_(rhs), operator_(StringToOperator(op)){};

  // Member functions.
 public:
  const std::string &get_lhs() const { return lhs_; };
  const std::string &get_rhs() const { return rhs_; };
  const Operator &get_op() const { return operator_; };
  std::string ToString() const {
    return lhs_ + " " + OperatorToString(operator_) + " " + rhs_;
  }

  // Utility functions for operator conversion.
  static const Operator StringToOperator(const std::string &op_str);
  static const std::string OperatorToString(Operator op);

  // Data members.
 private:
  const std::string lhs_;
  const std::string rhs_;
  const Operator operator_;
};

}  // namespace cardinality_estimation