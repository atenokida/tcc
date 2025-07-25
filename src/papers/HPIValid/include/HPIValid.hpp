#pragma once

#include <filesystem>

namespace hpiv {

struct Config;
class ResultCollector;

// run HPIValid:
//  * `input_file`: the csv file containing the database
//  * `cfg`: the algorithm configuration
//  * `RC`: the result collector gathering statistics and the result
void HPIValid(const std::filesystem::path& input_file, const Config& cfg,
              ResultCollector& RC);

}  // namespace hpiv
