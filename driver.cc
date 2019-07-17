#include "cache.h"

#include <iostream>

void PrintMatrix(const std::vector<std::vector<int> >& m) {
  for (int i = 0; i < m.size(); ++i) {
    for (int j = 0; j < m[i].size(); ++j) {
      printf("%d\t", m[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

int GetInt(Cache& cache, int idx, int width) {
  int rtn = 0;
  for (int i = 0; i < width; ++i) {
    rtn *= 2;
    int ans;
    cache.fetch_data(idx + i, &ans);
    rtn += ans;
  }
  return rtn;
}

void UpdateInt(Cache& cache, int idx, int x, int width) {
  // Update the digits at idx to x.
  for (int d = 0; d < width; ++d) {
    cache.write_data(idx + d, (x & (1 << (width - 1 - d))) ? 1 : 0);
  }
}

#define P 100

void ComputeMultiplication(Cache& cache, int B, int a_base, int b_base, int c_base, int n, int width) {
  int i, j, k;
  for (i = 0; i < n; i += B) {
    for (j = 0; j < n; j += B) {
      for (k = 0; k < n; k += B) {
        for (int i1 = i; i1 < std::min(n, i + B); ++i1) {
          for (int j1 = j; j1 < std::min(n, j + B); ++j1) {
            for (int k1 = k; k1 < std::min(n, k + B); ++k1) {
              int a = GetInt(cache, a_base + (i1 * n + k1) * width, width);
              int b = GetInt(cache, b_base + (k1 * n + j1) * width, width);
              int c = GetInt(cache, c_base + (i1 * n + j1) * width, width);
              c += a * b;
              c %= P;
              UpdateInt(cache, c_base + (i1 * n + j1) * width, c, width);
            }
          }
        }
      }
    }
  }
}

// Cache friendly summary.
void CacheFriendlyVsUnfriendly() {
  int n = 30; // Can change this n.
  int width = 32;
  int a_base = 0;
  int b_base = n * n * width;
  int c_base = n * n * width * 2;
  // Set up matrix A, B.
  std::vector<std::vector<int> > values(n, std::vector<int>(n));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      values[i][j] = (i * j) % P;
    }
  }
  std::vector<int> orig_data((1 << 20), 0);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      for (int d = 0; d < width; ++d) {
        orig_data[a_base + (i * n + j) * width + width - 1 - d] = ((values[i][j] & (1 << d)) ? 1 : 0);
        orig_data[b_base + (i * n + j) * width + width - 1 - d] = ((values[i][j] & (1 << d)) ? 1 : 0);
      }
    }
  }
  // Need 100 * 100 * 8 * 3 < 2 ** address_range.
  Cache cache_friendly(3, 3, 4, 18, orig_data);
  Cache cache_unfriendly(3, 3, 4, 18, orig_data);
  // Cache friendly implementation.
  ComputeMultiplication(cache_friendly, 8, a_base, b_base, c_base, n, width);
  ComputeMultiplication(cache_unfriendly, 1, a_base, b_base, c_base, n, width);
  printf("Cache friendly:\n");
  cache_friendly.PrintSummary();
  printf("Cache unfriendly:\n");
  cache_unfriendly.PrintSummary();
  // Print C for both.
  std::vector<std::vector<int> > C_friendly(n, std::vector<int>(n));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      C_friendly[i][j] = GetInt(cache_friendly, c_base + (i * n + j) * width, width);
    }
  }
  // PrintMatrix(C_friendly);
  std::vector<std::vector<int> > C_unfriendly(n, std::vector<int>(n));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      C_unfriendly[i][j] = GetInt(cache_unfriendly, c_base + (i * n + j) * width, width);
    }
  }
  // PrintMatrix(C_unfriendly);
}

int main(int argc, char* argv[]) {
  char instruction;
  int b = 3;
  int s = 3;
  int e = 4;
  int address_range = 8;
  Cache cache(b, s, e, address_range);
  cache.PrintDebug();
  while (true) {
    scanf(" %c", &instruction);
    if (instruction == 'e') {
      printf("Exit.\n");
      CacheFriendlyVsUnfriendly();
      exit(0);
    } else if (instruction == 'r') {
      int idx, ans;
      scanf("%d", &idx);
      bool cached = cache.fetch_data(idx, &ans);
      cache.PrintDebug();
      if (cached) printf("Cached.\n");
      else printf("Not cached.\n");
      printf("data[%d] = %d\n", idx, ans);
    } else if (instruction == 'w') {
      int idx, newval;
      scanf("%d%d", &idx, &newval);
      bool cached = cache.write_data(idx, newval);
      cache.PrintDebug();
      if (cached) printf("Cached.\n");
      else printf("Not cached.\n");
    } else {
      printf("Unsupported instruction!\n");
    }
  }
  return 0;
}

