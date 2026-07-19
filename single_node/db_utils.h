#ifndef DB_UTILS_H
#define DB_UTILS_H

#include <memory>
#include <string>
#include "rocksdb/db.h"

struct LogData {
    std::string payload;
    std::string time_stamp;
    std::string embedding;
};

std::string serialize_data(const LogData& data);
LogData deserialize_data(const std::string& input);
int putDB(const std::string& key, const LogData& agentlog, rocksdb::DB* db);
LogData getDB(const std::string& key, rocksdb::DB* db);
std::unique_ptr<rocksdb::DB> initialise_db(const std::string& db_name);

#endif
