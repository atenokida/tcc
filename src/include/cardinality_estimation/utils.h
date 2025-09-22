#pragma once

#include <memory>  // unique_ptr
#include <string>
#include <vector>

#include "predicate.h"
#include "table.h"

namespace cardinality_estimation {

// Struct ExperimentConfig.
//  - table: unique pointer to the table.
//  - predicates: bi-dimensional vector (each internal vector is a experiment).
struct ExperimentConfig {
  std::unique_ptr<Table> table;
  std::vector<std::vector<Predicate>> predicates;
};

// Parse input file for experiment.
// @arg filename: the name of the input file located at folder `in`.
// The first line of the file should contain the name of the dataset
// that is within the `data` folder.
// Subsequent lines should contain predicates.
extern struct ExperimentConfig ParseInputFile(const std::string& filename);

// Prints a `JOIN` condition.
// If `predicates` size is greater than one, predicates will be printed
// as a conjunction.
extern void PrintExpression(const std::vector<Predicate>& predicates);

// Prints configuration.
extern void ShowConfig(const ExperimentConfig& config);

// Converts a vector of predicates to a string representation.
extern std::string ExpressionToString(const std::vector<Predicate>& predicates);

}  // namespace cardinality_estimation