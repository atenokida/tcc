#include "cardinality_estimation/utils.h"

#include <iostream>
#include <fstream>  // ifstream
#include <memory>  // unique_ptr, make_unique
#include <sstream>  // istringstream
#include <stdexcept>  // invalid_argument
#include <string>

#include "cardinality_estimation/predicate.h"
#include "cardinality_estimation/table.h"

namespace cardinality_estimation {

// Example taken from: https://www.geeksforgeeks.org/cpp/how-to-read-from-a-file-in-cpp
struct ExperimentConfig ParseInputFile(const std::string& filename)
{
  std::ifstream file(filename);

  if (!file.is_open())
    throw std::invalid_argument("Error opening the file: " + filename);

  struct ExperimentConfig config;
  config.predicates = std::vector<std::vector<Predicate>>();

  std::string str_line;
  // Before reading the first line, the table is not read yet.
  bool table_read = false;
  while (getline(file, str_line)) {
    
    if (!table_read) [[unlikely]] {
      auto table = std::make_unique<Table>();
      table->load_csv("../../data/" + str_line);
      config.table = std::move(table);
      table_read = true;

    } else [[likely]] {
      std::istringstream iss(str_line);
      std::vector<Predicate> predicates;
      std::string lhs, op, rhs, logical_op;

      while (iss >> lhs >> op >> rhs) {
        // Skip logical operator. Currently only AND is supported.
        iss >> logical_op;
        predicates.emplace_back(Predicate(lhs, op, rhs));
      }

      config.predicates.emplace_back(std::move(predicates));
    }
  }

  return config;
}

void PrintExpression(const std::vector<Predicate>& predicates)
{
  std::cout << "Predicate: ";

  for (size_t i = 0; i < predicates.size(); ++i) {
    std::cout << predicates[i].ToString();
    if (i < predicates.size() - 1)
      // Currently support only conjunctions.
      std::cout << " AND ";
  }

  std::cout << "\n";
}

void ShowConfig(const ExperimentConfig& config)
{
  std::cout << "Table size is: " << config.table->num_rows() << "\n\n";

  for (size_t i = 0; i < config.predicates.size(); ++i) {
    std::cout << "Predicate " << i+1 << "\n";
    PrintExpression(config.predicates[i]);
    std::cout << "\n\n";
  }
}

}  // namespace cardinality_estimation