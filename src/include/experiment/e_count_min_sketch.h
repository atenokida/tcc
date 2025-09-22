#pragma once

#include "cardinality_estimation/count_min_sketch.h"
#include "cardinality_estimation/utils.h"

namespace cardinality_estimation {

// Run Count-min sketch (only equalities supported).
// Does support conjunctions (cf. Count-min sketch paper at Sec. 4.2).
extern void RunCountMinSketch(const struct cardinality_estimation::ExperimentConfig& config, 
                              const std::string& output_file = "../../results/cms");

}  // namespace cardinality_estimation