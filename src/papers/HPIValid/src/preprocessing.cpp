#include "preprocessing.hpp"

#include "PLITable.hpp"
#include "config.hpp"
#include "essential_table.hpp"
#include "result_collector.hpp"

namespace hpiv {

PLITable preprocess(const std::filesystem::path& input_file, const Config& cfg,
                    ResultCollector& RC) {
  PLITable res;

  RC.start_timer(timer::read_table);

  ETable tbl = load_essential_table(input_file);
  res.nr_rows = tbl.nr_rows;
  res.nr_cols = tbl.nr_cols;

  RC.stop_timer(timer::read_table);
  RC.start_timer(timer::construct_clusters);

  res.inverse_mapping.clear();
  res.inverse_mapping.resize(
      res.nr_cols, std::vector<unsigned>(res.nr_rows, size_one_cluster));

  res.PLIs.clear();
  res.PLIs.resize(res.nr_cols);

  for (unsigned i_c = 0; i_c < res.nr_cols; ++i_c) {
    unsigned counter = 0;

    for (auto& cluster : tbl.value_to_rows[i_c]) {
      if (cluster.second.size() > 1) {
        for (const unsigned& i_r : cluster.second) {
          res.inverse_mapping[i_c][i_r] = counter;
        }
        counter++;
        if (cfg.copy_PLIs) {
          res.PLIs[i_c].push_back(cluster.second);
        } else {
          res.PLIs[i_c].emplace_back(std::move(cluster.second));
        }
      }
    }

    res.PLIs[i_c].shrink_to_fit();
  }

  RC.stop_timer(timer::construct_clusters);
  return res;
}

}  // namespace hpiv
