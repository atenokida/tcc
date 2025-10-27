#include "experiment/e_simple_profile.h"

#include <chrono>  // high_resolution_clock, duration
#include <cstddef>  // size_t
#include <fstream>  // ofstream

namespace cardinality_estimation {

void RunSimpleProfileEqual(
    const struct cardinality_estimation::ExperimentConfig& config,
    std::size_t num_runs,
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
    for ( size_t i = 0; i < num_runs; ++i) {
      std::vector<std::string> column_names;
      for (const auto& predicate : experiment) {
        column_names.push_back(predicate.lhs());
        column_names.push_back(predicate.rhs());
      }
      
      // Measuring build time.
      auto build_start = std::chrono::high_resolution_clock::now();
      cardinality_estimation::SimpleProfile sp(*config.table.get(), column_names);
      auto build_end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> build_time = build_end - build_start;

      double estimate;

      // Measuring estimation time.
      auto estimate_start = std::chrono::high_resolution_clock::now();
      if (experiment.size() == 1)
        estimate = sp.EstimateEquality(*config.table.get(), experiment[0]);
      else
        estimate = sp.EstimateEquality(*config.table.get(), experiment);

      auto estimate_end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> estimate_time = estimate_end - estimate_start;
      
      // debug
      // cardinality_estimation::PrintExpression(experiment);
      // std::cout << "Estimate = " << estimate
      //           << "\nTime = " << estimate_time.count() << " ms" << "\n"
      //           << "-------------------------------------------\n";

      ofs << "SIMPLE-PROFILE,"
          << cardinality_estimation::ExpressionToString(experiment) << ","
          << config.table->num_rows() << ","
          << estimate << ","
          << build_time.count() << ","
          << estimate_time.count() << "\n";
    }
  }
  
  auto total_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> total_time = total_end - total_start;
  std::cout << "Total execution time: " << total_time.count() << " ms" << "\n";

  ofs << "SIMPLE-PROFILE,TOTAL," << config.table_name << ",,,"
      << total_time.count() << "\n\n";

  ofs.close();
}

}  // namespace cardinality_estimation