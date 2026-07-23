#ifndef HNSW_UTILS_H
#define HNSW_UTILS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "hnswlib/hnswlib.h"

struct SearchResult {
    float distance;
    std::string key;
};

class HNSWIndex {
public:
    HNSWIndex(int dim, int max_elements, int M = 16, int ef_construction = 200);
    
    void addPoint(const std::string& key, const std::vector<float>& embedding);
    std::vector<SearchResult> search(const std::vector<float>& query, int k, int ef = 50);
    void markDelete(const std::string& key);
    int getCurrentCount();

private:
    int dim_;
    hnswlib::L2Space space_;
    hnswlib::HierarchicalNSW<float> index_;
    std::unordered_map<uint64_t, std::string> hash_to_key_;
    std::unordered_map<std::string, uint64_t> key_to_hash_;
};

#endif
