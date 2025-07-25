#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "xtensor.hpp"

namespace cardinality_estimation {

/**
 * @brief A table class built on top of xtensor for handling mixed-type tabular
 * data
 *
 * Supports columns of different types (double, int, string) with named column
 * access and basic table operations like sorting, filtering, and CSV I/O.
 */
class Table {
 public:
  using ColumnVariant = std::variant<xt::xarray<double>, xt::xarray<int>,
                                     xt::xarray<std::string>>;

 private:
  enum class ColumnType { Double, Integer, String };

  std::unordered_map<std::string, ColumnVariant> columns_;
  std::vector<std::string> column_names_;
  size_t num_rows_;

 public:
  /**
   * @brief Default constructor
   */
  Table() : num_rows_(0) {}

  /**
   * @brief Constructor with specified number of rows
   */
  explicit Table(size_t rows) : num_rows_(rows) {}

  /**
   * @brief Load table from CSV file
   * @param filename Path to CSV file
   * @param has_header Whether the first row contains column names
   * @param delimiter CSV delimiter character
   */
  void load_csv(const std::string& filename, bool has_header = true,
                char delimiter = ',');

  /**
   * @brief Save table to CSV file
   * @param filename Output CSV file path
   * @param delimiter CSV delimiter character
   */
  void save_csv(const std::string& filename, char delimiter = ',') const;

  /**
   * @brief Add a column of doubles
   * @param name Column name
   * @param data Column data
   */
  void add_column(const std::string& name, const xt::xarray<double>& data);

  /**
   * @brief Add a column of integers
   * @param name Column name
   * @param data Column data
   */
  void add_column(const std::string& name, const xt::xarray<int>& data);

  /**
   * @brief Add a column of strings
   * @param name Column name
   * @param data Column data
   */
  void add_column(const std::string& name, const xt::xarray<std::string>& data);

  /**
   * @brief Get column by name
   * @param name Column name
   * @return Column data as variant
   */
  const ColumnVariant& get_column(const std::string& name) const;

  /**
   * @brief Get column by name (non-const)
   * @param name Column name
   * @return Column data as variant
   */
  ColumnVariant& get_column(const std::string& name);

  /**
   * @brief Get specific column as double array
   * @param name Column name
   * @return Double array reference
   * @throws std::runtime_error if column doesn't exist or isn't double type
   */
  const xt::xarray<double>& get_double_column(const std::string& name) const;

  /**
   * @brief Get specific column as integer array
   * @param name Column name
   * @return Integer array reference
   * @throws std::runtime_error if column doesn't exist or isn't integer type
   */
  const xt::xarray<int>& get_int_column(const std::string& name) const;

  /**
   * @brief Get specific column as string array
   * @param name Column name
   * @return String array reference
   * @throws std::runtime_error if column doesn't exist or isn't string type
   */
  const xt::xarray<std::string>& get_string_column(
      const std::string& name) const;

  /**
   * @brief Check if column exists
   * @param name Column name
   * @return True if column exists
   */
  bool has_column(const std::string& name) const;

  /**
   * @brief Get column names
   * @return Vector of column names in order
   */
  const std::vector<std::string>& column_names() const { return column_names_; }

  /**
   * @brief Get number of rows
   * @return Number of rows
   */
  size_t num_rows() const { return num_rows_; }

  /**
   * @brief Get number of columns
   * @return Number of columns
   */
  size_t num_columns() const { return columns_.size(); }

  /**
   * @brief Sort table by column (ascending)
   * @param column_name Name of column to sort by
   * @return New sorted table
   */
  Table sort_by(const std::string& column_name) const;

  /**
   * @brief Sort table by column
   * @param column_name Name of column to sort by
   * @param ascending Sort order (true for ascending, false for descending)
   * @return New sorted table
   */
  Table sort_by(const std::string& column_name, bool ascending) const;

  /**
   * @brief Get a subset of rows
   * @param start_row Starting row index
   * @param end_row Ending row index (exclusive)
   * @return New table with subset of rows
   */
  Table slice_rows(size_t start_row, size_t end_row) const;

  /**
   * @brief Select specific columns
   * @param column_names Names of columns to select
   * @return New table with selected columns
   */
  Table select_columns(const std::vector<std::string>& column_names) const;

  /**
   * @brief Print table to output stream
   * @param os Output stream
   * @param max_rows Maximum rows to print (-1 for all)
   */
  void print(std::ostream& os = std::cout, int max_rows = 10) const;

  /**
   * @brief Remove a column
   * @param name Column name to remove
   */
  void remove_column(const std::string& name);

  /**
   * @brief Clear all data
   */
  void clear();

  /**
   * @brief Check if table is empty
   * @return True if table has no rows or columns
   */
  bool empty() const { return num_rows_ == 0 || columns_.empty(); }

 private:
  /**
   * @brief Validate that a column has the correct number of rows
   */
  template <typename T>
  void validate_column_size(const xt::xarray<T>& data) const;

  /**
   * @brief Get sort indices for a column
   */
  template <typename T>
  xt::xarray<size_t> get_sort_indices(const xt::xarray<T>& column,
                                      bool ascending) const;

  /**
   * @brief Apply row indices to reorder a column
   */
  template <typename T>
  xt::xarray<T> reorder_column(const xt::xarray<T>& column,
                               const xt::xarray<size_t>& indices) const;

  /**
   * @brief Detect the most appropriate column type from string data
   */
  ColumnType detect_column_type(const std::vector<std::string>& data) const;
};

}  // namespace cardinality_estimation