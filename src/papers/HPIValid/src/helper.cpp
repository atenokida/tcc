#include "helper.hpp"

#include <filesystem>
#include <iostream>

#include "table.hpp"

namespace hlpr {

std::string dataset_name(const std::filesystem::path& input_file) {
  return input_file.stem();
}

std::filesystem::path subtable_input_file(
    const std::filesystem::path& input_file, unsigned rows, unsigned cols,
    const std::filesystem::path& subtable_dir) {
  if (rows == 0 && cols == 0) {
    return input_file;
  }

  std::string dataset = dataset_name(input_file);
  std::string sub_name =
      "r" + std::to_string(rows) + "_c" + std::to_string(cols) + ".csv";
  std::filesystem::path subtable_file = subtable_dir / dataset / sub_name;

  if (std::filesystem::exists(subtable_file)) {
    return subtable_file;
  }

  // std::cerr << "INFO: creating subtable for " << dataset << "(r = " << rows
  //           << "c = " << cols << ") -> " << subtable_file << std::endl;

  std::filesystem::create_directory(subtable_file.parent_path());
  Table table(input_file);
  Table subtable = table.subTable(rows, cols);
  subtable.writeToFile(subtable_file);

  return subtable_file;
}

}  // namespace hlpr
