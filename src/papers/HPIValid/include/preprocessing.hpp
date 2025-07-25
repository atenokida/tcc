#pragma once

#include <filesystem>

namespace hpiv {

struct Config;
struct PLITable;
class ResultCollector;

// run the preprocessing: load the table, compute the PLIs and the
// inverse mapping, collect run time stats with the result collector
PLITable preprocess(const std::filesystem::path& input_file, const Config& cfg,
                    ResultCollector& RC);
}  // namespace hpiv
