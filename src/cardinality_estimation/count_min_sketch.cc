#include "cardinality_estimation/count_min_sketch.h"

#include <algorithm>  // std::min
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>  // numeric_limits

#include "cardinality_estimation/predicate.h"
#include "cardinality_estimation/table.h"

namespace cardinality_estimation {

// ep -> error 0.01 < ep < 1 (the smaller the better)
// gamma_ -> probability for error (the smaller the better) 0 < gamm < 1
CountMinSketch::CountMinSketch(float ep, float gamm)
{
  if (!(0.009 <= ep && ep < 1)) { // ??
    std::cout << "eps_ must be in this range: [0.01, 1)" << "\n";
    exit(EXIT_FAILURE);
  } else if (!(0 < gamm && gamm < 1)) {
    std::cout << "gamma_ must be in this range: (0,1)" << "\n";
    exit(EXIT_FAILURE);
  }
  eps_ = ep;
  gamma_ = gamm;
  width_ = ceil(exp(1) / eps_);
  depth_ = ceil(log(1 / gamma_));
  total_ = 0;
  // initialize counter array of arrays, C_
  C_ = new int *[depth_];
  unsigned int i, j;
  for (i = 0; i < depth_; i++) {
    C_[i] = new int[width_];
    for (j = 0; j < width_; j++) {
      C_[i][j] = 0;
    }
  }
  // initialize depth_ pairwise independent hashes
  srand(time(NULL));
  hashes_ = new int *[depth_];
  for (i = 0; i < depth_; i++) {
    hashes_[i] = new int[2];
    GenAjBj(hashes_, i);
  }
}

// CountMinSkectch destructor
CountMinSketch::~CountMinSketch()
{
  // free array of counters, C_
  unsigned int i;
  for (i = 0; i < depth_; i++) {
    delete[] C_[i];
  }
  delete[] C_;

  // free array of hash values
  for (i = 0; i < depth_; i++) {
    delete[] hashes_[i];
  }
  delete[] hashes_;
}

// countMinSketch update item count (int)
void CountMinSketch::Update(int item, int c)
{
  total_ = total_ + c;
  unsigned int hashval = 0;
  for (unsigned int j = 0; j < depth_; j++) {
    hashval = ((long)hashes_[j][0] * item + hashes_[j][1]) % kCountMinLongPrime % width_;
    C_[j][hashval] = C_[j][hashval] + c;
  }
}

// countMinSketch update item count (string)
void CountMinSketch::Update(const char* str, int c)
{
  int hashval = HashStr(str);
  Update(hashval, c);
}

// CountMinSketch estimate item count (int)
unsigned int CountMinSketch::Estimate(int item)
{
  int minval = std::numeric_limits<int>::max();
  unsigned int hashval = 0;
  for (unsigned int j = 0; j < depth_; j++) {
    hashval = ((long)hashes_[j][0] * item + hashes_[j][1]) % kCountMinLongPrime % width_;
    minval = std::min(minval, C_[j][hashval]);
  }
  return minval;
}

// CountMinSketch estimate item count (string)
unsigned int CountMinSketch::Estimate(const char *str)
{
  int hashval = HashStr(str);
  return Estimate(hashval);
}

// generates aj,bj from field Z_p for use in hashing
void CountMinSketch::GenAjBj(int** hashes, int i)
{
  hashes[i][0] = int(float(rand()) * float(kCountMinLongPrime) / float(RAND_MAX) + 1);
  hashes[i][1] = int(float(rand()) * float(kCountMinLongPrime) / float(RAND_MAX) + 1);
}

// generates a hash value for a sting
// same as djb2 hash function
unsigned int CountMinSketch::HashStr(const char *str)
{
  unsigned long hash = 5381;
  int c;
  while (c = *str++) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash;
}

/**------------------------------------------------------------------------
**                       Adapted code below
*------------------------------------------------------------------------**/

double EstimateInnerProduct(const Table& table, const Predicate& predicate)
{
  return 0.0;
}

}  // namespace cardinality_estimation