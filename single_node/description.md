##single node architecture

So in simgle words we are bulding a server and a client 

#Server:
gets the request from the client(agents), 
two kinds of requests 

	1. put 
	- server gets put (key, payload, embedding) 

	2. get
	- server gets get (quyery_embedding, k)

more descriptive
Client calls Put(key, payload, embedding)
  → gRPC server receives it
  → Write to RocksDB
  → Add to HNSW index
  → Return success

Client calls VectorSearch(query_embedding, k)
  → gRPC server receives it
  → Query HNSW index → get top-k keys
  → Fetch payloads from RocksDB
  → Return results


