#include "hnsw_utils.h"
#include <iostream>
#include <functional>
using namespace std;

HNSWIndex::HNSWIndex(int dim, int max_elements, int M, int ef_construction)
    : dim_(dim), space_(dim), index_(&space_, max_elements, M, ef_construction) {
    cout << "HNSW index created: dim=" << dim 
         << ", max_elements=" << max_elements 
         << ", M=" << M 
         << ", ef_construction=" << ef_construction << endl;
}

void HNSWIndex::addPoint(const string& key, const vector<float>& embedding) {
    if ((int)embedding.size() != dim_) {
        throw runtime_error("Embedding dimension mismatch: expected " + 
                           to_string(dim_) + ", got " + to_string(embedding.size()));
    }

    uint64_t hash_id;
    if (key_to_hash_.count(key)) {
        hash_id = key_to_hash_[key];
    } else {
        hash_id = hash<string>{}(key);
        hash_to_key_[hash_id] = key;
        key_to_hash_[key] = hash_id;
    }

    index_.addPoint(embedding.data(), hash_id);
    cout << "Added point: key=" << key << ", hash_id=" << hash_id << endl;
}

vector<SearchResult> HNSWIndex::search(const vector<float>& query, int k, int ef) {
    if ((int)query.size() != dim_) {
        throw runtime_error("Query dimension mismatch: expected " + 
                           to_string(dim_) + ", got " + to_string(query.size()));
    }

    index_.setEf(ef);
    auto results = index_.searchKnn(query.data(), k);

    vector<SearchResult> output;
    while (!results.empty()) {
        float dist = results.top().first;
        uint64_t hash_id = results.top().second;
        results.pop();

        SearchResult sr;
        sr.distance = dist;
        if (hash_to_key_.count(hash_id)) {
            sr.key = hash_to_key_[hash_id];
        } else {
            sr.key = "unknown_" + to_string(hash_id);
        }
        output.push_back(sr);
    }

    return output;
}

void HNSWIndex::markDelete(const string& key) {
    if (!key_to_hash_.count(key)) {
        throw runtime_error("Key not found in index: " + key);
    }
    uint64_t hash_id = key_to_hash_[key];
    index_.markDelete(hash_id);
    cout << "Marked deleted: key=" << key << ", hash_id=" << hash_id << endl;
}

int HNSWIndex::getCurrentCount() {
    return index_.getCurrentElementCount();
}
