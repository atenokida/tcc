
#include "result_collector_output.hpp"

namespace hpiv {

ResultCollectorOutput::ResultCollectorOutput(
    const std::string& name, const std::filesystem::path& output_dir,
    double timeout)
    : ResultCollector(timeout),
      m_name(name),
      m_output_dir(output_dir),
      m_ucc_output(m_output_dir / (m_name + "_UCCs.hg")) {}

bool ResultCollectorOutput::ucc_found(const edge& ucc) {
  // output
  for (edge::size_type v = ucc.find_first(); v != edge::npos;
       v = ucc.find_next(v)) {
    if (v != ucc.find_first()) m_ucc_output << ",";
    m_ucc_output << v;
  }
  m_ucc_output << "\n";

  // call base class
  return ResultCollector::ucc_found(ucc);
}

void ResultCollectorOutput::final_hypergraph(const Hypergraph& hg) {
  std::ofstream out(m_output_dir / (m_name + "_diff_sets.hg"));
  for (const edge& e : hg) {
    for (edge::size_type v = e.find_first(); v != edge::npos;
         v = e.find_next(v)) {
      if (v != e.find_first()) out << ",";
      out << v;
    }
    out << "\n";
  }
  ResultCollector::final_hypergraph(hg);

  // add actual number of rows/cols to file name (so that different
  // subtables of the same table have different names)
  if (std::filesystem::exists(m_output_dir / (m_name + "_UCCs.hg"))) {
    std::filesystem::rename(m_output_dir / (m_name + "_UCCs.hg"),
                            m_output_dir / (file_name() + "_UCCs.hg"));
  }
  if (std::filesystem::exists(m_output_dir / (m_name + "_diff_sets.hg"))) {
    std::filesystem::rename(m_output_dir / (m_name + "_diff_sets.hg"),
                            m_output_dir / (file_name() + "_diff_sets.hg"));
  }
}

std::string ResultCollectorOutput::file_name() {
  return m_name + "_r" + std::to_string(nr_rows()) + "_c" +
         std::to_string(nr_cols());
}

}  // namespace hpiv
