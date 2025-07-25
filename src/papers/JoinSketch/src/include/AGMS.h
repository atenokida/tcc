#include <mmintrin.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include <random>

#include "MurmurHash.h"
#include "params.h"
using namespace std;

class est : public Sketch {
 public:
  int w, d;
  int* counter[MAX_HASH_NUM];
  int hash_seed;

 public:
  est(int _w, int _d, int _hash_seed = 1000) {
    d = _d, w = _w / 4 / _d;
    // printf("%d\n",w);
    hash_seed = _hash_seed;
    for (int i = 0; i < d; i++) {
      counter[i] = new int[w];
      memset(counter[i], 0, sizeof(int) * w);
    }
  }

  void Insert(const char* str) {
    unsigned g = 0;
    for (int i = 0; i < d; i++) {
      for (int j = 0; j < w; j++) {
        if (!g) g = ((unsigned)MurmurHash32(str, KEY_LEN, hash_seed + i * j));
        if (g & 1)
          counter[i][j]++;
        else
          counter[i][j]--;
        g >>= 1;
      }
    }
  }
  long double Join(Sketch* _other) {
    const est* other = (est*)_other;
    long double res[MAX_HASH_NUM];
    for (int i = 0; i < d; i++) {
      long double k = 0;
      for (int j = 0; j < w; j++)
        k += 1ll * counter[i][j] * other->counter[i][j];
      res[i] = 1.0 * k / w;
    }
    long double re = 0;
    for (int i = 0; i < d; i++) re += res[i];
    return 1.0 * re / d;
  }

  int Query(const char* str) const {
    puts("est can't query!");
    exit(-1);
  }

  ~est() {
    for (int i = 0; i < d; i++) delete[] counter[i];
  }
};
