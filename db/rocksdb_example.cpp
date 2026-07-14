#include <cassert>
#include <iostream>
#include <memory>         
#include <string>     // for std::unique_ptr
#include "rocksdb/db.h"
using namespace std;

int putDB(const string& key, const string& value, rocksdb::DB* db) {
    rocksdb::Status st = db->Put(rocksdb::WriteOptions(), key, value);
    cout << st.ToString() << endl;   
    return 0;
}


int main() {
    rocksdb::DB* db_raw = nullptr;                     
    std::unique_ptr<rocksdb::DB> db;                   
    rocksdb::Options options;
    options.create_if_missing = true;

    rocksdb::Status status = rocksdb::DB::Open(options, "./testdb", &db);
    assert(status.ok());

    if (!status.ok()) {
        std::cerr << status.ToString() << std::endl;
        return 1;
    }

    std::cout << "Database opened successfully!" << std::endl;
    
    cout<<status.ToString()<<endl;
    // string key1 = "my_key_1";
    // string value5 = "hemllo_1";
    // cout << "putting an item in db" << endl;
    // putDB(key1, value5, db.get());

    string key = "my_key_2";
    // string value = "hemllo_2";
    // cout << "putting an item in db" << endl;
    // putDB(key, value, db.get()); 


    cout<<"doing get db"<<endl;
    string value1;
    db->Get(rocksdb::ReadOptions(), key, &value1);
    cout<<"VAlue1 :" << value1<<endl;
    return 0;
}