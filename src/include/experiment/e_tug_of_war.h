#pragma once

#include <cstdint>  // uint32_t

#include "cardinality_estimation/tug_of_war.h"
#include "cardinality_estimation/utils.h"

namespace cardinality_estimation {

// Run Tug-of-War estimator.
// Note that Tug-of-War was originally designed to support a single equality
// predicate. Hence, the estimator implemented only supports equality operator
// and col. homogenous predicates (e.g., t.A = t'.A).
// See header file for more information about complex predicates.
extern void RunTugOfWar(const struct cardinality_estimation::ExperimentConfig& config,
                        const unsigned int depth,
                        const unsigned int width,
                        const uint32_t hash_seed = 1000);

}  // namespace cardinality_estimation