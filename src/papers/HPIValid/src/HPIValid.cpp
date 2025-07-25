#include "HPIValid.hpp"

#include "PLITable.hpp"
#include "TreeSearch.hpp"
#include "config.hpp"
#include "preprocessing.hpp"
#include "result_collector.hpp"

namespace hpiv {

void HPIValid(const std::filesystem::path& input_file, const Config& cfg,
              ResultCollector& RC) {
  RC.start_timer(timer::total);
  RC.start_timer(timer::total_preprocessing);

  PLITable tab = preprocess(input_file, cfg, RC);
  RC.set_size(tab.nr_rows, tab.nr_cols);

  RC.stop_timer(timer::total_preprocessing);
  RC.start_timer(timer::total_enum_algo);

  TreeSearch TS(tab, cfg, RC);
  TS.run();

  RC.stop_timer(timer::total_enum_algo);
  RC.stop_timer(timer::total);
}

}  // namespace hpiv
