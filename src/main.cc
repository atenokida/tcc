
#include <iostream>

#include "include/cardinality_estimation/count_min_sketch.h"
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

  // Number of groups
  constexpr int tow_depth = 5;
  // Number estimators per group.
  // 10240/4/#groups = 10240/4/5 = 512
  constexpr int tow_width = 10240;
  /*--------------- END OF CONFIG ---------------*/

  const auto config = cardinality_estimation::ParseInputFile
                      ("../../in/" + std::string(argv[1]));

  cardinality_estimation::ShowConfig(config);

  for (size_t i = 2; i < argc; ++i) {
    //todo: Add support for other operators (non-equality and inequalities).

    const std::string estimator = argv[i];

    if (estimator == "tug-of-war" || estimator == "tow") {
      cardinality_estimation::RunTugOfWar(config, tow_depth, tow_width);
    } else if (estimator == "simple-profile" || estimator == "sp") {
      cardinality_estimation::RunSimpleProfileEqual(config);
    } else {
      std::cout << "Unknown estimator: " << estimator << "\n";
      std::cout << "Available estimators: tug-of-war, simple-profile\n";
      return 1;
    }
    
  }

  return 0;
}