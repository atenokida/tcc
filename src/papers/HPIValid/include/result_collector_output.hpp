#pragma once

#include <fstream>

#include "result_collector.hpp"

namespace hpiv {

class ResultCollectorOutput : public ResultCollector {
 public:
  ResultCollectorOutput(const std::string& name,
                        const std::filesystem::path& output_dir,
                        double timeout = 3600.0);

  // Writes each found UCC to an output file.
  bool ucc_found(const edge& ucc) override;

  // Writes the final hypergraph to an output file.
  void final_hypergraph(const Hypergraph& hg) override;

 private:
  std::string file_name();

  std::string m_name;
  std::filesystem::path m_output_dir;
  std::ofstream m_ucc_output;
};

}  // namespace hpiv
