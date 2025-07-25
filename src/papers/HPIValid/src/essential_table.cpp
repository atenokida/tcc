#include "essential_table.hpp"

#include <fstream>
#include <iostream>

unsigned long parse_next_line(std::vector<std::string_view>& strvec,
                              const std::string_view& contents,
                              unsigned long start_pos) {
  strvec.clear();
  unsigned long last_start_pos = start_pos;
  unsigned long pos = start_pos;
  bool flag = true;
  while (contents[pos] != '\n') {
    if (contents[pos] == ',' && flag) {
      strvec.push_back(contents.substr(last_start_pos, pos - last_start_pos));
      last_start_pos = pos + 1;
    } else if (contents[pos] == '\"' &&
               (pos == 0 || contents[pos - 1] != '\\')) {
      // non-escaped quotation mark
      flag = !flag;
    }
    ++pos;
  }
  strvec.push_back(contents.substr(last_start_pos, pos - last_start_pos));

  return pos + 1;
}

ETable load_essential_table(const std::filesystem::path& input_file) {
  std::ifstream file(input_file);
  std::string file_as_string;
  file.seekg(0, std::ios::end);
  file_as_string.resize(file.tellg());
  file.seekg(0, std::ios::beg);
  file.read(&file_as_string[0], file_as_string.size());
  file.close();
  std::string_view contents(file_as_string);

  // reading the first line
  std::vector<std::string_view> strvec;
  unsigned long start_pos = 0;
  parse_next_line(strvec, contents, start_pos);

  ETable res = {std::vector<value_to_rows_map>(strvec.size()), strvec.size(),
                0};

  unsigned int row = 0;
  // reading more liens
  do {
    start_pos = parse_next_line(strvec, contents, start_pos);

    if (strvec.size() != res.nr_cols) {
      std::cout << "warning: wrong number of entries in line " << row
                << std::endl;
      std::cout << "number of entries in first line: " << res.nr_cols
                << std::endl;
      std::cout << "number of entries in current line: " << strvec.size()
                << std::endl;
      std::cout << "line was skipped" << std::endl;
      continue;
    }

    for (unsigned int col = 0; col < res.nr_cols; ++col) {
      auto it = res.value_to_rows[col].find(strvec[col]);
      if (it == res.value_to_rows[col].end()) {
        res.value_to_rows[col][strvec[col]] = std::vector<unsigned>(1, row);
      } else {
        (*it).second.push_back(row);
      }
      // value_to_rows[col][strvec[col]].push_back(row);
    }
    row++;
  } while (start_pos < contents.size());

  res.nr_rows = row;

  return res;
}
