#include "experiment/e_tug_of_war.h"

#include <cstdint>  // uint32_t

namespace cardinality_estimation {

void RunTugOfWar(const struct cardinality_estimation::ExperimentConfig& config,
                 const unsigned int depth,
                 const unsigned int width,
                 const uint32_t hash_seed)
{
  cardinality_estimation::TugOfWar tow(depth, width, hash_seed);

  auto total_start = std::chrono::high_resolution_clock::now();

  // Experiments.
  for (const auto& experiment : config.predicates) {
    auto start = std::chrono::high_resolution_clock::now();
    double estimate;

    if (experiment.size() == 1)
      estimate = tow.EstimateEquality(*config.table.get(), experiment[0]);
    else
      estimate = tow.EstimateEquality(*config.table.get(), experiment);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    
    cardinality_estimation::PrintExpression(experiment);
    std::cout << "Estimate = " << estimate
              << "\nTime = " << elapsed.count() << " ms" << "\n"
              << "-------------------------------------------\n";
  }

  auto total_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> total_time = total_end - total_start;
  std::cout << "Total execution time: " << total_time.count() << " ms" << "\n";
}

}  // namespace cardinality_estimation