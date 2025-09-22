/**----------------------------------------------
 * ?                    ABOUT
 * @author      : atenokida
 * @createdOn   : 21/september/2025
 * @brief       : Header file for the Count-min sketch.
 *                Adapted from: github.com/alabid/countminsketch.
 *---------------------------------------------**/

#pragma once

#include <vector>

#include "predicate.h"
#include "table.h"

namespace cardinality_estimation {

inline constexpr long int kCountMinLongPrime = 4294967311l;

/**----------------------------------------------
 * !                  WARNING
 *   Currently only supports predicates with
 *   attributes of type string and integer.
 *   TODO: Add support to others fundamental types.
 *
 *---------------------------------------------**/
class CountMinSketch {
 public:
  CountMinSketch(float eps, float gamma);
  ~CountMinSketch();

  // Update item of type `int` by count c.
  void Update(int item, int c);
  // Update item of type `string` by count c.
  void Update(const char *item, int c);

  // Return estimate count of item `i`.
  unsigned int Estimate(int item);
  unsigned int Estimate(const char* item);

  // Return the total count of all items in the sketch.
  unsigned int TotalCount() { return total_; };

  // Generates a hash value for a string.
  // Same as djb2 hash function.
  unsigned int HashStr(const char *str);

 private:
  // REFACTOR THIS
  // Generate "new" a_j, b_j.
  void GenAjBj(int** hashes, int i);

  unsigned int width_;
  unsigned int depth_;

  // Eps (for error), 0.01 < eps < 1.
  // The smaller the better.
  float eps_;

  // Gamma (probability for accuracy), 0 < gamma < 1
  // The bigger the better.
  float gamma_;

  // a_j, b_j \in Z_p.
  // Both elements of field Z_p used in generation of hash function.
  unsigned int aj_;
  unsigned int bj_;

  // REMOVE THIS?
  // Total count so far.
  unsigned int total_;

  // REFACTOR THIS
  // Array of arrays of counters.
  int** C_;

  // REFACTOR THIS
  // Array of hash values for a particular item.
  // Contains two element arrays {a_j, b_j}.
  int** hashes_;

/**------------------------------------------------------------------------
**                       Adapted code below
*------------------------------------------------------------------------**/
 public:
  // Inner Product = Join Size estimation.
  // Cf. section 4.2 from the original paper: 
  //  - "The Count-min sketch and its applications".
  static double EstimateInnerProduct(const Table& table, 
                                     const Predicate& predicate);
  static double EstimateInnerProduct(const Table& table, 
                              const std::vector<Predicate>& predicates) { return 0.0; };

};

}  // namespace cardinality_estimation