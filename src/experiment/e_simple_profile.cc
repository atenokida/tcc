#include "experiment/e_simple_profile.h"

#include <chrono>  // high_resolution_clock, duration
#include <fstream>  // ofstream

namespace cardinality_estimation {

void RunSimpleProfileEqual(const struct cardinality_estimation::ExperimentConfig& config, 
                           const std::string& output_file)
{
  std::ofstream ofs(output_file, std::ios::app);
  if (!ofs.is_open()) {
    std::cerr << "Failed to open output file: " << output_file << "\n";
    return;
  }

  // `column_names` hold all attribute names that some statistics
  // will be available for estimations. Those statistics are built uppon
  // calling the constructor of SimpleProfile class.
  std::vector<std::string> column_names;
  for (const auto& experiment : config.predicates) {
    for (const auto& predicate : experiment) {
      column_names.push_back(predicate.lhs());
      column_names.push_back(predicate.rhs());
    }
  }

  auto total_start = std::chrono::high_resolution_clock::now();

  cardinality_estimation::SimpleProfile sp(*config.table.get(), column_names);

  // Experiments.
  for (const auto& experiment : config.predicates) {
    auto start = std::chrono::high_resolution_clock::now();
    double estimate;

    if (experiment.size() == 1)
      estimate = sp.EstimateEquality(*config.table.get(), experiment[0]);
    else
      estimate = sp.EstimateEquality(*config.table.get(), experiment);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    
    cardinality_estimation::PrintExpression(experiment);

    std::cout << "Estimate = " << estimate
              << "\nTime = " << elapsed.count() << " ms" << "\n"
              << "-------------------------------------------\n";

    ofs << "SIMPLE-PROFILE,"
        << cardinality_estimation::ExpressionToString(experiment) << ","
        << config.table->num_rows() << ","
        << estimate << ","
        << elapsed.count() << "\n";
  }
  
  auto total_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> total_time = total_end - total_start;
  std::cout << "Total execution time: " << total_time.count() << " ms" << "\n";

  ofs << "SIMPLE-PROFILE,TOTAL,,," << total_time.count() << "\n";

  ofs.close();
}

}  // namespace cardinality_estimation