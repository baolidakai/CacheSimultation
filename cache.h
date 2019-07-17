#include <iostream>
#include <vector>

class CacheContent {
private:
  // valid bit
  bool v;
  // dirty bit
  bool d;
  // tag
  int tg;
  // data
  std::vector<int> data;
public:
  CacheContent(int tag, std::vector<int> data) : tg(tag), data(data), v(true), d(false) {}
  void PrintDebug() const;
  bool valid() const {return v;}
  bool dirty() const {return d;}
  int tag() const {return tg;}
  int data_at(int i) const {
    return data[i];
  }
  int data_size() const {return data.size();}
  void mark_dirty() {d = true;}
  void set_at(int i, int newval) {
    data[i] = newval;
  }
};

/*
 * Cache for data.
 */
class Cache {
private:
  // log of block size, log of set size, log of lines per set, log of original data.
  int b, s, e, address_range;
  // tag size.
  int t;
  // block size, set size, lines per set.
  int B, S, E;
  // Original data.
  std::vector<int> orig_data;
  // Cache.
  std::vector<std::vector<CacheContent> > cache_contents;
  // number of fetch.
  int fetch_count;
  // number of misses.
  int miss_count;
public:
  Cache(int b, int s, int e, int address_range);
  Cache(int b, int s, int e, int address_range, const std::vector<int>& orig_data);
  void PrintSummary();
  void PrintDebug();
  // Fetch the data at index idx, returns whether it's cached.
  bool fetch_data(int idx, int* ans);
  // Write new_data to index idx, returns whether it's cached already.
  bool write_data(int idx, int new_data);
};

