#pragma once

#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <vector>

// maps for each value to a vector of rows with this value
typedef std::unordered_map<std::string_view, std::vector<unsigned>>
    value_to_rows_map;

struct ETable {
  // for each column (vector), map from the value value to a vector of
  // rows with this value
  std::vector<value_to_rows_map> value_to_rows;

  unsigned long nr_cols;
  unsigned long nr_rows;
};

ETable load_essential_table(const std::filesystem::path& input_file);
