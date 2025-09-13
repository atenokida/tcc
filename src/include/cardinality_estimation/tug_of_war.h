/**----------------------------------------------
 * ?                    ABOUT
 * @author      : atenokida
 * @createdOn   : 29/june/2025
 * @brief       : Header file for the Tug of War Algorithm.
 *                Adapted from:
 *                 - "JoinSketch: A Sketch Algorithm for Accurate and Unbiased Inner-Product Estimation"
 *                    by Wang et al. at PACMMOD Vol. 1 (2023).
 *                 - "Finding Frequent Items in Data Streams" by Graham and Mario at PVLDB Vol. 1 (2008).
 *---------------------------------------------**/

#pragma once

#include <memory>  // shared_ptr
#include <vector>

#include "predicate.h"
#include "table.h"

namespace cardinality_estimation {

// Global configuration variables for the Tug-of-War Estimator.
inline constexpr int kToWMaxHashNum = 512;
inline constexpr int kToWKeyLen = 4;

/**---------------------------------
 * !           WARNING
 * 
 * ! TUG-OF-WAR SKETCH ONLY SUPPORTS
 * !  COL. HOMOGENEOUS PREDICATES
 *----------------------------------**/
class TugOfWar {
 public:
  TugOfWar(const unsigned int depth, 
           const unsigned int width, // memory in bytes
           const uint32_t hash_seed = 1000);

  const double EstimateEquality(const Table& table, 
                                const Predicate& predicate);

  // Tug-of-War only supports single equality predicates.
  // For conjunctive predicates, we hash combined attribute values
  // as one distinct value.
  const double EstimateEquality(const Table& table, 
                                const std::vector<Predicate>& predicates);

  // Estimate the second frequency moment (= self-join size).
  static const double F2Est(const TugOfWar& sketch);

  // Overloaded function for single equality predicate estimate in case the 
  // sketch is already built.
  // UNCOMMENT WHEN IMPLEMENTING INCREMENTABILITY COMPARISONS
  // static const double EstimateEquality(const Table& table, 
  //                                      const Predicate& predicate,
  //                                      const TugOfWar& sketch){return 0.0;};

  // Overloaded function for complex equalities predicate estimate in case the
  // sketch is already built.
  // UNCOMMENT WHEN IMPLEMENTING INCREMENTABILITY COMPARISONS
  // static const double EstimateEquality(const Table& table, 
  //                                      const Predicate& predicate,
  //                                      const std::vector<TugOfWar>& sketches){return 0.0;};

 private:
  // Insert element into summary.
  void Update(const void* str);

  const unsigned int depth_;
  const unsigned int width_;
  const uint32_t hash_seed_;
  std::shared_ptr<int[]> counter_[kToWMaxHashNum];
};

}  // namespace cardinality_estimation