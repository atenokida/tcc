#pragma once

#include <filesystem>
#include <string>

namespace hlpr {

// name of the dataset based on the input file
std::string dataset_name(const std::filesystem::path& input_file);

// create subtable if necessary and return file name of the subtable
std::filesystem::path subtable_input_file(
    const std::filesystem::path& input_file, unsigned rows, unsigned cols,
    const std::filesystem::path& subtable_dir);

}  // namespace hlpr
