#include "experiment/e_tug_of_war.h"

#include <chrono>  // high_resolution_clock, duration
#include <cstddef>  // size_t
#include <cstdint>  // uint32_t
#include <fstream>  // ofstream

namespace cardinality_estimation {

void RunTugOfWar(const struct cardinality_estimation::ExperimentConfig& config,
                 const unsigned int depth,
                 const unsigned int width,
                 std::size_t num_runs,
                 const uint32_t hash_seed,
                 const std::string& output_file)
{
  // Just instantiating the object. Build is later
  cardinality_estimation::TugOfWar tow(depth, width, hash_seed);

  std::ofstream ofs(output_file, std::ios::app);
  if (!ofs.is_open()) {
    std::cerr << "Failed to open output file: " << output_file << "\n";
    return;
  }

  auto total_start = std::chrono::high_resolution_clock::now();

  // Experiments.
  for (const auto& experiment : config.predicates) {
    for (size_t i = 0; i < num_runs; ++i) {
      auto start = std::chrono::high_resolution_clock::now();
      double estimate;
      std::chrono::duration<double, std::milli> build_time;
      std::chrono::duration<double, std::milli> estimation_time;

      if (experiment.size() == 1)
        estimate = tow.EstimateEquality(*config.table.get(), experiment[0],
                                        build_time, estimation_time);
      else
        estimate = tow.EstimateEquality(*config.table.get(), experiment,
                                        build_time, estimation_time);

      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> elapsed = end - start;
      
      // debug
      // cardinality_estimation::PrintExpression(experiment);
      // std::cout << "Estimate = " << estimate
      //           << "\nTime = " << elapsed.count() << " ms" << "\n"
      //           << "-------------------------------------------\n";

      ofs << "TUG-OF-WAR,"
          << cardinality_estimation::ExpressionToString(experiment) << ","
          << config.table->num_rows() << ","
          << estimate << ","
          << build_time.count() << ","
          << estimation_time.count() << "\n";
    }
  }

  auto total_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> total_time = total_end - total_start;
  std::cout << "Total execution time: " << total_time.count() << " ms" << "\n";

  ofs << "TUG-OF-WAR,TOTAL," << config.table_name << ",,,"
      << total_time.count() << "\n\n";

  ofs.close();
}

}  // namespace cardinality_estimation