#pragma once

#include <limits>
#include <vector>

namespace hpiv {

// a table in the form of PLIs together with the inverse mapping and
// some additional information
struct PLITable {
  // the PLIs: for each column, we have a vector of clusters where
  // each cluster is a vector of row IDs
  std::vector<std::vector<std::vector<unsigned>>> PLIs;

  // the inverse mapping: for each column, we have a vector of mapping
  // a row ID to a cluster ID
  std::vector<std::vector<unsigned>> inverse_mapping;

  // the number of rows
  unsigned nr_rows;

  // the number of columns
  unsigned nr_cols;
};

// cluster id to use for all clusters of size one
constexpr unsigned size_one_cluster = std::numeric_limits<unsigned>::max();

}  // namespace hpiv
