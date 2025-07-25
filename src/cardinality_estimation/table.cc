#include "cardinality_estimation/table.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>

namespace cardinality_estimation {

// CSV Loading Implementation
void Table::load_csv(const std::string& filename, bool has_header,
                     char delimiter) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  clear();

  std::vector<std::vector<std::string>> raw_data;
  std::string line;

  // Read all lines
  while (std::getline(file, line)) {
    std::vector<std::string> row;
    std::stringstream ss(line);
    std::string cell;

    while (std::getline(ss, cell, delimiter)) {
      // Trim whitespace
      cell.erase(0, cell.find_first_not_of(" \t\r\n"));
      cell.erase(cell.find_last_not_of(" \t\r\n") + 1);
      row.push_back(cell);
    }

    if (!row.empty()) {
      raw_data.push_back(row);
    }
  }

  if (raw_data.empty()) {
    throw std::runtime_error("Empty CSV file");
  }

  size_t start_row = 0;

  // Handle header
  if (has_header) {
    column_names_ = raw_data[0];
    start_row = 1;
  } else {
    // Generate default column names
    for (size_t i = 0; i < raw_data[0].size(); ++i) {
      column_names_.push_back("col_" + std::to_string(i));
    }
  }

  if (raw_data.size() <= start_row) {
    throw std::runtime_error("No data rows found");
  }

  num_rows_ = raw_data.size() - start_row;
  size_t num_cols = column_names_.size();

  // Analyze column types and convert data
  for (size_t col = 0; col < num_cols; ++col) {
    std::vector<std::string> column_data;

    // Extract column data
    for (size_t row = start_row; row < raw_data.size(); ++row) {
      if (col < raw_data[row].size()) {
        column_data.push_back(raw_data[row][col]);
      } else {
        column_data.push_back("");  // Handle missing values
      }
    }

    // Try to determine column type
    ColumnType type = detect_column_type(column_data);

    switch (type) {
      case ColumnType::Double: {
        xt::xarray<double> arr = xt::empty<double>({num_rows_});
        for (size_t i = 0; i < num_rows_; ++i) {
          try {
            // TODO: precision handling
            arr(i) = std::stod(column_data[i]);
          } catch (...) {
            arr(i) = std::numeric_limits<double>::quiet_NaN();
          }
        }
        columns_[column_names_[col]] = arr;
        break;
      }
      case ColumnType::Integer: {
        xt::xarray<int> arr = xt::empty<int>({num_rows_});
        for (size_t i = 0; i < num_rows_; ++i) {
          try {
            arr(i) = std::stoi(column_data[i]);
          } catch (...) {
            arr(i) = 0;  // Default for failed conversion
          }
        }
        columns_[column_names_[col]] = arr;
        break;
      }
      case ColumnType::String: {
        xt::xarray<std::string> arr = xt::empty<std::string>({num_rows_});
        for (size_t i = 0; i < num_rows_; ++i) {
          arr(i) = column_data[i];
        }
        columns_[column_names_[col]] = arr;
        break;
      }
    }
  }
}

// CSV Saving Implementation
void Table::save_csv(const std::string& filename, char delimiter) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot create file: " + filename);
  }

  // Write header
  for (size_t i = 0; i < column_names_.size(); ++i) {
    if (i > 0) file << delimiter;
    file << column_names_[i];
  }
  file << "\n";

  // Write data rows
  for (size_t row = 0; row < num_rows_; ++row) {
    for (size_t col = 0; col < column_names_.size(); ++col) {
      if (col > 0) file << delimiter;

      const auto& column_name = column_names_[col];
      const auto& column_var = columns_.at(column_name);

      std::visit([&file, row](const auto& column) { file << column(row); },
                 column_var);
    }
    file << "\n";
  }
}

// Column Addition Methods
void Table::add_column(const std::string& name,
                       const xt::xarray<double>& data) {
  validate_column_size(data);
  columns_[name] = data;

  // Add to column names if new
  if (std::find(column_names_.begin(), column_names_.end(), name) ==
      column_names_.end()) {
    column_names_.push_back(name);
  }

  if (num_rows_ == 0) {
    num_rows_ = data.size();
  }
}

void Table::add_column(const std::string& name, const xt::xarray<int>& data) {
  validate_column_size(data);
  columns_[name] = data;

  if (std::find(column_names_.begin(), column_names_.end(), name) ==
      column_names_.end()) {
    column_names_.push_back(name);
  }

  if (num_rows_ == 0) {
    num_rows_ = data.size();
  }
}

void Table::add_column(const std::string& name,
                       const xt::xarray<std::string>& data) {
  validate_column_size(data);
  columns_[name] = data;

  if (std::find(column_names_.begin(), column_names_.end(), name) ==
      column_names_.end()) {
    column_names_.push_back(name);
  }

  if (num_rows_ == 0) {
    num_rows_ = data.size();
  }
}

// Column Access Methods
const Table::ColumnVariant& Table::get_column(const std::string& name) const {
  auto it = columns_.find(name);
  if (it == columns_.end()) {
    throw std::runtime_error("Column not found: " + name);
  }
  return it->second;
}

Table::ColumnVariant& Table::get_column(const std::string& name) {
  auto it = columns_.find(name);
  if (it == columns_.end()) {
    throw std::runtime_error("Column not found: " + name);
  }
  return it->second;
}

const xt::xarray<double>& Table::get_double_column(
    const std::string& name) const {
  const auto& column = get_column(name);
  if (!std::holds_alternative<xt::xarray<double>>(column)) {
    throw std::runtime_error("Column '" + name + "' is not of double type");
  }
  return std::get<xt::xarray<double>>(column);
}

const xt::xarray<int>& Table::get_int_column(const std::string& name) const {
  const auto& column = get_column(name);
  if (!std::holds_alternative<xt::xarray<int>>(column)) {
    throw std::runtime_error("Column '" + name + "' is not of integer type");
  }
  return std::get<xt::xarray<int>>(column);
}

const xt::xarray<std::string>& Table::get_string_column(
    const std::string& name) const {
  const auto& column = get_column(name);
  if (!std::holds_alternative<xt::xarray<std::string>>(column)) {
    throw std::runtime_error("Column '" + name + "' is not of string type");
  }
  return std::get<xt::xarray<std::string>>(column);
}

bool Table::has_column(const std::string& name) const {
  return columns_.find(name) != columns_.end();
}

// Sorting Implementation
Table Table::sort_by(const std::string& column_name) const {
  return sort_by(column_name, true);
}

Table Table::sort_by(const std::string& column_name, bool ascending) const {
  if (!has_column(column_name)) {
    throw std::runtime_error("Column not found: " + column_name);
  }

  const auto& sort_column = get_column(column_name);
  xt::xarray<size_t> indices;

  // Get sort indices based on column type
  std::visit(
      [&indices, ascending, this](const auto& column) {
        indices = get_sort_indices(column, ascending);
      },
      sort_column);

  // Create new table with reordered data
  Table result(num_rows_);
  result.column_names_ = column_names_;

  for (const auto& col_name : column_names_) {
    const auto& column_var = columns_.at(col_name);

    std::visit(
        [&result, &col_name, &indices, this](const auto& column) {
          auto reordered = reorder_column(column, indices);
          result.columns_[col_name] = reordered;
        },
        column_var);
  }

  return result;
}

// Slicing Implementation
Table Table::slice_rows(size_t start_row, size_t end_row) const {
  if (start_row >= num_rows_ || end_row > num_rows_ || start_row >= end_row) {
    throw std::runtime_error("Invalid row range");
  }

  size_t new_rows = end_row - start_row;
  Table result(new_rows);
  result.column_names_ = column_names_;

  for (const auto& col_name : column_names_) {
    const auto& column_var = columns_.at(col_name);

    std::visit(
        [&result, &col_name, start_row, end_row](const auto& column) {
          auto sliced = xt::view(column, xt::range(start_row, end_row));
          result.columns_[col_name] =
              xt::xarray<typename std::decay_t<decltype(column)>::value_type>(
                  sliced);
        },
        column_var);
  }

  return result;
}

// Column Selection Implementation
Table Table::select_columns(
    const std::vector<std::string>& column_names) const {
  Table result(num_rows_);

  for (const auto& col_name : column_names) {
    if (!has_column(col_name)) {
      throw std::runtime_error("Column not found: " + col_name);
    }
    result.column_names_.push_back(col_name);
    result.columns_[col_name] = columns_.at(col_name);
  }

  return result;
}

// Print Implementation
void Table::print(std::ostream& os, int max_rows) const {
  if (empty()) {
    os << "Empty table\n";
    return;
  }

  // Print header
  for (size_t i = 0; i < column_names_.size(); ++i) {
    if (i > 0) os << "\t";
    os << std::setw(12) << column_names_[i];
  }
  os << "\n";

  // Print separator
  for (size_t i = 0; i < column_names_.size(); ++i) {
    if (i > 0) os << "\t";
    os << std::string(12, '-');
  }
  os << "\n";

  // Print data rows
  size_t rows_to_print =
      (max_rows < 0) ? num_rows_
                     : std::min(static_cast<size_t>(max_rows), num_rows_);

  for (size_t row = 0; row < rows_to_print; ++row) {
    for (size_t col = 0; col < column_names_.size(); ++col) {
      if (col > 0) os << "\t";

      const auto& column_name = column_names_[col];
      const auto& column_var = columns_.at(column_name);

      std::visit(
          [&os, row](const auto& column) {
            os << std::setw(12) << column(row);
          },
          column_var);
    }
    os << "\n";
  }

  if (max_rows >= 0 && num_rows_ > static_cast<size_t>(max_rows)) {
    os << "... (" << (num_rows_ - max_rows) << " more rows)\n";
  }

  os << "\nShape: " << num_rows_ << " rows × " << column_names_.size()
     << " columns\n";
}

// Utility Methods
void Table::remove_column(const std::string& name) {
  auto it = columns_.find(name);
  if (it != columns_.end()) {
    columns_.erase(it);
    column_names_.erase(
        std::remove(column_names_.begin(), column_names_.end(), name),
        column_names_.end());
  }
}

void Table::clear() {
  columns_.clear();
  column_names_.clear();
  num_rows_ = 0;
}

// Private Helper Methods
template <typename T>
void Table::validate_column_size(const xt::xarray<T>& data) const {
  if (num_rows_ > 0 && data.size() != num_rows_) {
    throw std::runtime_error("Column size mismatch. Expected " +
                             std::to_string(num_rows_) + " rows, got " +
                             std::to_string(data.size()));
  }
}

template <typename T>
xt::xarray<size_t> Table::get_sort_indices(const xt::xarray<T>& column,
                                           bool ascending) const {
  xt::xarray<size_t> indices = xt::arange<size_t>(column.size());

  if (ascending) {
    std::sort(indices.begin(), indices.end(),
              [&column](size_t a, size_t b) { return column(a) < column(b); });
  } else {
    std::sort(indices.begin(), indices.end(),
              [&column](size_t a, size_t b) { return column(a) > column(b); });
  }

  return indices;
}

template <typename T>
xt::xarray<T> Table::reorder_column(const xt::xarray<T>& column,
                                    const xt::xarray<size_t>& indices) const {
  xt::xarray<T> result = xt::empty<T>({column.size()});

  for (size_t i = 0; i < indices.size(); ++i) {
    result(i) = column(indices(i));
  }

  return result;
}

// Column Type Detection Helper
// enum class Table::ColumnType { Double, Integer, String };
Table::ColumnType Table::detect_column_type(
    const std::vector<std::string>& data) const {
  bool all_integers = true;
  bool all_numbers = true;

  bool test = false;  // DEBUG: remove this

  for (const auto& value : data) {
    if (value.empty()) continue;

    if (value == "2000-11-01") test = true;  // DEBUG: remove this

    // Try double conversion first
    try {
      size_t pos;
      std::stod(value, &pos);

      // Check if entire string was consumed
      // during string to double conversion
      if (pos != value.length()) {
        all_numbers = false;
        all_integers = false;
        break;
      }

      // If it's a valid number, check if it's an integer
      if (all_integers) {
        // Check if the string contains a decimal point or scientific notation.
        // npos is an constant static member of the class string that
        // represents the maximum possible value for an element of type
        // size_t, which is an unsigned integral type used for sizes and indices
        // in string operations. See:
        // https://www.tutorialspoint.com/string-npos-in-cplusplus-with-examples
        if (value.find('.') != std::string::npos ||
            value.find('e') != std::string::npos ||
            value.find('E') != std::string::npos) {
          all_integers = false;
        } else {
          // Try integer conversion to ensure it's within integer range
          try {
            std::stoll(value);  // Use stoll for better range coverage
          } catch (...) {
            all_integers = false;
          }
        }
      }
    } catch (...) {
      all_numbers = false;
      break;
    }
  }

  // if (test) {
  //   std::cout << "DEBUG: " << all_integers << "\n";  // DEBUG: remove this
  //   std::cout << "DEBUG: " << all_numbers << "\n";   // DEBUG: remove this
  // }

  if (all_integers) return ColumnType::Integer;
  if (all_numbers) return ColumnType::Double;
  return ColumnType::String;
}

}  // namespace cardinality_estimation