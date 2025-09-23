#include "experiment/e_count_min_sketch.h"

#include <algorithm>  // std::all_of
#include <chrono>   // high_resolution_clock, duration
#include <fstream>  // ofstream

namespace cardinality_estimation {

void RunCountMinSketch(
    const struct cardinality_estimation::ExperimentConfig& config,
    const unsigned int depth,
    const unsigned int width,
    const std::string& output_file) 
{
  std::ofstream ofs(output_file, std::ios::app);
  if (!ofs.is_open()) {
    std::cerr << "Failed to open output file: " << output_file << "\n";
    return;
  }

  auto total_start = std::chrono::high_resolution_clock::now();

  // Experiments.
  for (const auto& experiment : config.predicates) {
    
    bool col_homogeneous = false;
    // Check if all predicates are column homogeneous.
    if (std::all_of(experiment.begin(), experiment.end(),
    [](const Predicate& p) { return p.lhs() == p.rhs(); })) {
      col_homogeneous = true;
    }

    auto start = std::chrono::high_resolution_clock::now();
    double estimate;

    estimate = CMSInnerProduct(*config.table.get(), experiment, depth, width, col_homogeneous);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    
    cardinality_estimation::PrintExpression(experiment);

    std::cout << "Estimate = " << estimate
              << "\nTime = " << elapsed.count() << " ms" << "\n"
              << "-------------------------------------------\n";

    ofs << "COUNT-MIN-SKETCH,"
        << cardinality_estimation::ExpressionToString(experiment) << ","
        << config.table->num_rows() << ","
        << estimate << ","
        << elapsed.count() << "\n";
  }

  auto total_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> total_time = total_end - total_start;
  std::cout << "Total execution time: " << total_time.count() << " ms" << "\n";

  ofs << "COUNT-MIN-SKETCH,TOTAL,,," << total_time.count() << "\n";

  ofs.close();
}

}  // namespace cardinality_estimation