/**----------------------------------------------
 * ?                    ABOUT
 * @author      : XXXX-1
 * @createdOn   : 21/june/2025
 * @brief       : Implementation file for `predicate.h`.
 *---------------------------------------------**/

#include "cardinality_estimation/predicate.h"

#include <stdexcept>  // invalid_argument
#include <string>
#include <unordered_map>

namespace cardinality_estimation {

const Operator Predicate::StringToOperator(const std::string &op_str)
{
  static const std::unordered_map<std::string, Operator> kStringToOperator = {
      {"=", Operator::kEqual},        {"!=", Operator::kNotEqual},
      {"<>", Operator::kNotEqual},    {"<", Operator::kLess},
      {"<=", Operator::kLessEqual},   {">", Operator::kGreater},
      {">=", Operator::kGreaterEqual}};

  auto it = kStringToOperator.find(op_str);
  if (it != kStringToOperator.end()) {
    return it->second;
  }

  throw std::invalid_argument("Unknown operator: " + op_str);
}

const std::string Predicate::OperatorToString(Operator op)
{
  switch (op) {
    case Operator::kEqual:
      return "=";
    case Operator::kNotEqual:
      return "!=";
    case Operator::kLess:
      return "<";
    case Operator::kLessEqual:
      return "<=";
    case Operator::kGreater:
      return ">";
    case Operator::kGreaterEqual:
      return ">=";
    default:
      throw std::invalid_argument("Unknown operator enum value");
  }
}

}  // namespace cardinality_estimation