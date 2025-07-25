#pragma once

#include <filesystem>
#include <vector>

// #include "global.hpp"

typedef unsigned long natNumber;
typedef std::vector<natNumber> record;

class Table {
 private:
  std::vector<record> m_Records;
  std::vector<natNumber> m_nr_of_values;

 public:
  // Table() = delete;
  Table() {}
  explicit Table(const std::filesystem::path& path);

  static void splitLine(std::vector<std::string>& strvec,
                        const std::string& line);

  std::vector<record>::size_type numRecords() const { return m_Records.size(); }
  record::size_type numColumns() const { return m_Records.front().size(); };

  // the table is parsed so that each column has integer values
  // ranging from 0 to x for some value x; this method returns x + 1,
  // i.e., the number of different values
  natNumber nrOfValues(natNumber column) const {
    return m_nr_of_values[column];
  }

  void writeToFile(const std::filesystem::path& path) const;

  // operators and related

  inline record& operator[](std::vector<record>::size_type i_e) {
    return m_Records[i_e];
  }
  inline const record& operator[](std::vector<record>::size_type i_e) const {
    return m_Records[i_e];
  }

  inline std::vector<record>::iterator begin() { return m_Records.begin(); }
  inline std::vector<record>::const_iterator begin() const {
    return m_Records.begin();
  }

  inline std::vector<record>::iterator end() { return m_Records.end(); }
  inline std::vector<record>::const_iterator end() const {
    return m_Records.end();
  }

  // create subtable given by vectors of row and column indices; it is
  // assumed that all indices are less than numRecords and numColumns,
  // respectively
  Table subTable(const std::vector<natNumber>& row_indices,
                 const std::vector<natNumber>& col_indices);

  // create subtable using the first max_rows records and the first
  // max_cols attributes; a 0 for max_rows/max_cols is interpreted to
  // keep all rows/cols
  Table subTable(natNumber max_rows, natNumber max_cols);
};
