#include <iostream>
#include <vector>
#include "db_utils.h"
#include "hnsw_utils.h"
using namespace std;

int main() {
    // Initialize RocksDB
    string db_name = "./agent_log";
    auto db = initialise_db(db_name);

    // Initialize HNSW index: 3 dimensions, max 1000 elements
    int dim = 3;
    HNSWIndex hnsw(dim, 1000);

    // Store some test data
    LogData log1 = {"red ball at 10, 30", "2024-01-01", "0.1,0.2,0.3"};
    LogData log2 = {"blue box at 50, 60", "2024-01-02", "0.4,0.5,0.6"};
    LogData log3 = {"red ball near wall", "2024-01-03", "0.15,0.25,0.35"};
    LogData log4 = {"green circle at 5, 10", "2024-01-04", "0.9,0.8,0.7"};

    // Store in RocksDB
    putDB("agent_001_mem001", log1, db.get());
    putDB("agent_001_mem002", log2, db.get());
    putDB("agent_002_mem001", log3, db.get());
    putDB("agent_002_mem002", log4, db.get());

    // Store embeddings in HNSW
    hnsw.addPoint("agent_001_mem001", {0.1f, 0.2f, 0.3f});
    hnsw.addPoint("agent_001_mem002", {0.4f, 0.5f, 0.6f});
    hnsw.addPoint("agent_002_mem001", {0.15f, 0.25f, 0.35f});
    hnsw.addPoint("agent_002_mem002", {0.9f, 0.8f, 0.7f});

    cout << "\n=== Vector Search ===" << endl;
    cout << "Index has " << hnsw.getCurrentCount() << " points\n" << endl;

    // Search: find 2 nearest to a query vector
    vector<float> query = {0.12f, 0.22f, 0.32f};  // close to log1 and log3
    cout << "Query: {0.12, 0.22, 0.32}" << endl;
    cout << "Looking for 2 nearest:\n" << endl;

    auto results = hnsw.search(query, 2);

    for (const auto& r : results) {
        LogData data = getDB(r.key, db.get());
        cout << "Key: " << r.key << endl;
        cout << "Distance: " << r.distance << endl;
        cout << "Payload: " << data.payload << endl;
        cout << "Timestamp: " << data.time_stamp << endl;
        cout << "---" << endl;
    }

    return 0;
}
