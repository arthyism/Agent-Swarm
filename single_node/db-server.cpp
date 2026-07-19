#include <iostream>
#include "db_utils.h"
using namespace std;

int main() {
    string db_name = "./agent_log";
    std::unique_ptr<rocksdb::DB> db = initialise_db(db_name);

    LogData agentlog = {"red ball at 10, 30", "01-01-2001", "123,121,347"};
    if (putDB("agent_001_mem001", agentlog, db.get())) {
        cout << "successfull put" << endl;
    }

    LogData answer = getDB("agent_001_mem001", db.get());
    cout << answer.payload << endl << answer.time_stamp << endl << answer.embedding << endl;
    return 0;
}
