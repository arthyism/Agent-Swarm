#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "agent_swarm.grpc.pb.h"
#include "db_utils.h"
#include "hnsw_utils.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using agentswarm::AgentSwarm;
using agentswarm::PutRequest;
using agentswarm::PutResponse;
using agentswarm::GetRequest;
using agentswarm::GetResponse;
using agentswarm::VectorSearchRequest;
using agentswarm::VectorSearchResponse;
using agentswarm::VectorSearchResult;

class AgentSwarmServiceImpl final : public AgentSwarm::Service {
public:
    AgentSwarmServiceImpl(const std::string& db_path, int dim, int max_elements)
        : hnsw_(dim, max_elements) {
        db_ = initialise_db(db_path);
        if (!db_) {
            throw std::runtime_error("Failed to initialize database");
        }
        std::cout << "Server initialized with db=" << db_path 
                  << ", dim=" << dim << std::endl;
    }

    Status Put(ServerContext* context, const PutRequest* request,
               PutResponse* response) override {
        try {
            // Convert repeated float to vector
            std::vector<float> embedding(request->embedding().begin(),
                                         request->embedding().end());

            // Store in RocksDB
            LogData data{request->payload(), request->timestamp(), 
                         vectorToString(embedding)};
            int db_result = putDB(request->key(), data, db_.get());

            // Store in HNSW index
            hnsw_.addPoint(request->key(), embedding);

            response->set_success(true);
            response->set_message("OK");
            return Status::OK;
        } catch (const std::exception& e) {
            response->set_success(false);
            response->set_message(e.what());
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    Status Get(ServerContext* context, const GetRequest* request,
               GetResponse* response) override {
        try {
            LogData data = getDB(request->key(), db_.get());

            response->set_found(true);
            response->set_key(request->key());
            response->set_payload(data.payload);
            response->set_timestamp(data.time_stamp);

            // Parse embedding string back to floats
            auto embedding = stringToVector(data.embedding);
            for (float f : embedding) {
                response->add_embedding(f);
            }

            return Status::OK;
        } catch (const std::exception& e) {
            response->set_found(false);
            return Status::OK;
        }
    }

    Status VectorSearch(ServerContext* context, const VectorSearchRequest* request,
                        VectorSearchResponse* response) override {
        try {
            std::vector<float> query(request->query_embedding().begin(),
                                     request->query_embedding().end());

            auto results = hnsw_.search(query, request->k());

            for (const auto& r : results) {
                auto* result = response->add_results();
                result->set_key(r.key);
                result->set_distance(r.distance);

                // Fetch full data from RocksDB
                try {
                    LogData data = getDB(r.key, db_.get());
                    result->set_payload(data.payload);
                    result->set_timestamp(data.time_stamp);
                } catch (...) {
                    result->set_payload("error fetching");
                }
            }

            return Status::OK;
        } catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

private:
    std::unique_ptr<rocksdb::DB> db_;
    HNSWIndex hnsw_;

    std::string vectorToString(const std::vector<float>& vec) {
        std::string result;
        for (size_t i = 0; i < vec.size(); i++) {
            if (i > 0) result += ",";
            result += std::to_string(vec[i]);
        }
        return result;
    }

    std::vector<float> stringToVector(const std::string& str) {
        std::vector<float> result;
        size_t start = 0;
        size_t end = str.find(',');
        while (end != std::string::npos) {
            result.push_back(std::stof(str.substr(start, end - start)));
            start = end + 1;
            end = str.find(',', start);
        }
        result.push_back(std::stof(str.substr(start)));
        return result;
    }
};

void RunServer(const std::string& port, const std::string& db_path, 
               int dim, int max_elements) {
    std::string server_address = "0.0.0.0:" + port;
    AgentSwarmServiceImpl service(db_path, dim, max_elements);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    std::string port = "50051";
    std::string db_path = "./agent_log";
    int dim = 128;
    int max_elements = 10000;

    if (argc > 1) port = argv[1];
    if (argc > 2) db_path = argv[2];
    if (argc > 3) dim = std::stoi(argv[3]);
    if (argc > 4) max_elements = std::stoi(argv[4]);

    RunServer(port, db_path, dim, max_elements);
    return 0;
}
