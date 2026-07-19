#include <cassert>
#include <iostream>
#include <cstdint>
#include <cstring>
using namespace std;

void appendString(std::string& output, const std::string& value) {
    uint32_t length = static_cast<uint32_t>(value.size());
    output.append(reinterpret_cast<const char*>(&length), sizeof(length));
    output.append(value);
}

std::string serialize_data(const LogData& data) {
    std::string result;
    appendString(result, data.payload);
    appendString(result, data.time_stamp);
    appendString(result, data.embedding);
    return result;
}

std::string readString(const std::string& input, std::size_t& offset) {
    if (offset + sizeof(uint32_t) > input.size()) {
        throw std::runtime_error("Invalid serialized data");
    }
    uint32_t length;
    std::memcpy(&length, input.data() + offset, sizeof(length));
    offset += sizeof(length);
    if (offset + length > input.size()) {
        throw std::runtime_error("Invalid serialized data");
    }
    std::string value = input.substr(offset, length);
    offset += length;
    return value;
}

LogData deserialize_data(const std::string& input) {
    std::size_t offset = 0;
    LogData result{
        readString(input, offset),
        readString(input, offset),
        readString(input, offset)
    };
    if (offset != input.size()) {
        throw std::runtime_error("Unexpected extra data");
    }
    return result;
}

int putDB(const string& key, const LogData& agentlog, rocksdb::DB* db) {
    string ser_value = serialize_data(agentlog);
    rocksdb::Status st = db->Put(rocksdb::WriteOptions(), key, ser_value);
    cout << "PutDB status for Key: " << key << " is " << st.ToString() << endl;
    return 0;
}

LogData getDB(const string& key, rocksdb::DB* db) {
    string value;
    db->Get(rocksdb::ReadOptions(), key, &value);
    LogData deser_val = deserialize_data(value);
    return deser_val;
}

std::unique_ptr<rocksdb::DB> initialise_db(const string& db_name) {
    rocksdb::DB* db_raw = nullptr;                     
    std::unique_ptr<rocksdb::DB> db;                   
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, db_name, &db_raw);
    assert(status.ok());
    if (!status.ok()) {
        std::cerr << status.ToString() << std::endl;
        return nullptr;
    }
    db.reset(db_raw);
    std::cout << "Database opened successfully!" << std::endl;
    return db;
}
