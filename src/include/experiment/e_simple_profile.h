#pragma once

#include <cstddef>  // size_t

#include "cardinality_estimation/simple_profile.h"
#include "cardinality_estimation/utils.h"

namespace cardinality_estimation {

// Run SimpleProfile estimator for only equalities.
// Does support conjunctions.
extern void RunSimpleProfileEqual(
    const struct cardinality_estimation::ExperimentConfig& config,
    std::size_t num_runs = 1,
    const std::string& output_file = "../../results/sp");

}  // namespace cardinality_estimation