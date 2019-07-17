#include "cache.h"

#include <iostream>
#include <cstdlib>
#include <vector>

Cache::Cache(int b, int s, int e, int address_range) : b(b), s(s), e(e), address_range(address_range), cache_contents(1 << s), fetch_count(0), miss_count(0) {
  t = address_range - s - b;
  B = (1 << b);
  S = (1 << s);
  E = (1 << e);
  orig_data = std::vector<int>((1 << address_range), 0);
}

Cache::Cache(int b, int s, int e, int address_range, const std::vector<int>& orig_data) : b(b), s(s), e(e), address_range(address_range), cache_contents(1 << s), fetch_count(0), miss_count(0), orig_data(orig_data) {
  t = address_range - s - b;
  B = (1 << b);
  S = (1 << s);
  E = (1 << e);
}

bool Cache::write_data(int idx, int newval) {
  ++fetch_count;
  // Extract tag, set index, block offset.
  int tag = idx >> (s + b);
  int set_idx = (idx - (tag << (s + b))) >> b;
  int block_offset = idx & ((1 << b) - 1);
  int cache_idx = -1;
  std::vector<CacheContent>& row = cache_contents[set_idx];
  for (int i = 0; i < row.size(); ++i) {
    if (row[i].valid() && row[i].tag() == tag) {
      cache_idx = i;
      break;
    }
  }
  if (cache_idx == -1) {
    // Not found.
    ++miss_count;
    // Update the newval and pull in.
    orig_data[idx] = newval;
    // Add to cache set.
    std::vector<int> data;
    int start_idx = idx - (idx & ((1 << b) - 1));
    for (int i = 0; i < B; ++i) {
      data.push_back(orig_data[start_idx + i]);
    }
    row.insert(row.begin(), CacheContent(tag, data));
    // Evict the least recently used.
    // Populate if dirty.
    while (row.size() > E) {
      const CacheContent& cc = row.back();
      row.pop_back();
      if (cc.dirty()) {
        int start_idx = (cc.tag() << (s + b)) + (set_idx << s);
        for (int i = 0; i < B; ++i) {
          orig_data[start_idx + i] = cc.data_at(i);
        }
      }
    }
    return false;
  }
  const CacheContent cc = row[cache_idx];
  row.erase(row.begin() + cache_idx);
  row.insert(row.begin(), cc);
  row[0].mark_dirty();
  row[0].set_at(block_offset, newval);
  return true;
}

bool Cache::fetch_data(int idx, int* ans) {
  ++fetch_count;
  // Extract tag, set index, block offset.
  int tag = idx >> (s + b);
  int set_idx = (idx - (tag << (s + b))) >> b;
  int block_offset = idx & ((1 << b) - 1);
  int cache_idx = -1;
  std::vector<CacheContent>& row = cache_contents[set_idx];
  for (int i = 0; i < row.size(); ++i) {
    if (row[i].valid() && row[i].tag() == tag) {
      cache_idx = i;
      break;
    }
  }
  if (cache_idx == -1) {
    // Not found.
    ++miss_count;
    // Add to cache set.
    std::vector<int> data;
    int start_idx = idx - (idx & ((1 << b) - 1));
    for (int i = 0; i < B; ++i) {
      data.push_back(orig_data[start_idx + i]);
    }
    row.insert(row.begin(), CacheContent(tag, data));
    *ans = row[0].data_at(block_offset);
    // Evict the least recently used.
    // Populate if dirty.
    while (row.size() > E) {
      const CacheContent& cc = row.back();
      row.pop_back();
      if (cc.dirty()) {
        int start_idx = (cc.tag() << (s + b)) + (set_idx << s);
        for (int i = 0; i < B; ++i) {
          orig_data[start_idx + i] = cc.data_at(i);
        }
      }
    }
    return false;
  }
  const CacheContent cc = row[cache_idx];
  row.erase(row.begin() + cache_idx);
  row.insert(row.begin(), cc);
  *ans = row[0].data_at(block_offset);
  return true;
}

void CacheContent::PrintDebug() const {
  printf("v: %d, d: %d, tag: %b, data: ", v, d, tg);
  for (int x : data) printf("%d", x);
}

void Cache::PrintSummary() {
  printf("fetch %d, miss %d, miss rate: %.2f%%\n", fetch_count, miss_count, miss_count ? static_cast<double>(miss_count) / fetch_count * 100 : 0);
}

void Cache::PrintDebug() {
  printf("b = %d, s = %d, e = %d, t = %d, address_range = %d\n", b, s, e, t, address_range);
  printf("B = %d, S = %d, E = %d\n", B, S, E);
  PrintSummary();
  printf("Cache entries:\n");
  for (const std::vector<CacheContent>& row : cache_contents) {
    for (const CacheContent& cc : row) {
      cc.PrintDebug();
      printf(" ");
    }
    printf("\n");
  }
  printf("original data:");
  for (int i = 0; i < (1 << address_range); ++i) {
    if (i % B == 0) {
      if (i % (B * 4) == 0) printf("\n");
      else printf(" ");
    }
    printf("%d", orig_data[i]);
  }
  printf("\n");
}
