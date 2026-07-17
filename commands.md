#rocksdb 
g++ -std=c++17 -I/opt/homebrew/include -L/opt/homebrew/lib -o rocksdb_example /Users/kartik/agent-swarm/db/rocksdb_example.cpp -lrocksdb && ./rocksdb_example

#grpc-hello
protoc --grpc_out=. --cpp_out=. -I. hello.proto

#server and clent link them 
c++ -std=c++17 $(pkg-config --cflags --libs grpc++ protobuf) hello.pb.cc hello.grpc.pb.cc server.cpp -o server && \
c++ -std=c++17 $(pkg-config --cflags --libs grpc++ protobuf) hello.pb.cc hello.grpc.pb.cc client.cpp -o client
run serverand clent in different terminals
./server ./client


#siglenode server 
g++ -std=c++17 -I/opt/homebrew/include -L/opt/homebrew/lib \
    -o db-server db-server.cpp -lrocksdb
  ./db-server
