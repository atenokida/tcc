
#include <iostream>

#include "include/cardinality_estimation/predicate.h"
#include "include/cardinality_estimation/simple_profile.h"
#include "include/cardinality_estimation/table.h"
#include "include/cardinality_estimation/tug_of_war.h"
#include "include/cardinality_estimation/utils.h"
#include "include/experiment/e_count_min_sketch.h"
#include "include/experiment/e_simple_profile.h"
#include "include/experiment/e_tug_of_war.h"

int main(const int argc, const char** argv)
{
  if (argc < 2) {
    std::cout << "Malformated input. Please specify input configuration file.\n";
    return 1;
  }

  /**-------------- CONFIG. VALUES --------------*/
  //todo: Add parsing on main loop to read configuration values and avoid the need to recompile the program.
  // !EXPERIMENT RUNNING OPTIONS
  constexpr std::size_t kNumRuns = 5; // Number of times each estimator is run.

  // !TUG-OF-WAR
  // Number of groups
  constexpr int kToWDepth = 5;
  // Number estimators per group.
  // 10240/4/#groups = 10240/4/5 = 512
  constexpr int kToWWidth = 10240;

  // !COUNT-MIN SKETCH
  constexpr unsigned int kCMSDepth = 7;
  constexpr unsigned int kCMSWidth = 10000;
  /*--------------- END OF CONFIG ---------------*/

  const auto config = cardinality_estimation::ParseInputFile
                      ("../../in/" + std::string(argv[1]));

  cardinality_estimation::ShowConfig(config);

  // For every algorithm specified in the command line, run the experiments.
  for (size_t i = 2; i < argc; ++i) {
    //todo: Add support for other operators (non-equality and inequalities).

    const std::string estimator = argv[i];

    std::cout << "Running estimator: " << estimator << "\n";
  
    if (estimator == "tug-of-war" || estimator == "tow") {
      cardinality_estimation::RunTugOfWar(config, kToWDepth, kToWWidth, kNumRuns);
    } else if (estimator == "simple-profile" || estimator == "sp") {
      cardinality_estimation::RunSimpleProfileEqual(config, kNumRuns);
    } else if (estimator == "count-min-sketch" || estimator == "cms") {
      cardinality_estimation::RunCountMinSketch(config, kCMSDepth, kCMSWidth, kNumRuns);
    } else {
      std::cout << "Unknown estimator: " << estimator << "\n";
      std::cout << "Available estimators: tug-of-war, simple-profile, count-min-sketch\n";
      return 1;
    }
    
  }

  return 0;
}