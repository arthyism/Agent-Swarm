#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "hello.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class GreeterClient {
public:
  GreeterClient(std::shared_ptr<Channel> channel)
    : stub_(greeter::NewStub(channel)) {}

  std::string SayHello(const std::string& name) {
    HelloRequest request;
    request.set_name(name);

    HelloResponse response;
    ClientContext context;

    Status status = stub_->SayHello(&context, request, &response);

    if (status.ok()) {
      return response.message();
    } else {
      return "RPC failed: " + status.error_message();
    }
  }

private:
  std::unique_ptr<greeter::Stub> stub_;
};

int main() {
  GreeterClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

  for (int i=0;i<10;i++){
  std::string reply = client.SayHello("Kartik");
  std::cout << "Received: " << reply << std::endl;
  }
  return 0;
}
