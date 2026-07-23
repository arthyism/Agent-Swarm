import grpc
import sys
sys.path.insert(0, '.')

from single_node import agent_swarm_pb2
from single_node import agent_swarm_pb2_grpc

def run():
    channel = grpc.insecure_channel('localhost:50051')
    stub = agent_swarm_pb2_grpc.AgentSwarmStub(channel)

    # Test Put
    print("=== Testing Put ===")
    request = agent_swarm_pb2.PutRequest(
        key="agent_001_mem001",
        payload="red ball at 10, 30",
        timestamp="2024-01-01",
        embedding=[0.1, 0.2, 0.3]
    )
    response = stub.Put(request)
    print(f"Put success: {response.success}, message: {response.message}")

    # Put more data
    requests = [
        ("agent_001_mem002", "blue box at 50, 60", "2024-01-02", [0.4, 0.5, 0.6]),
        ("agent_002_mem001", "red ball near wall", "2024-01-03", [0.15, 0.25, 0.35]),
        ("agent_002_mem002", "green circle at 5, 10", "2024-01-04", [0.9, 0.8, 0.7]),
    ]
    for key, payload, ts, emb in requests:
        req = agent_swarm_pb2.PutRequest(key=key, payload=payload, timestamp=ts, embedding=emb)
        resp = stub.Put(req)
        print(f"Put {key}: {resp.success}")

    # Test Get
    print("\n=== Testing Get ===")
    get_req = agent_swarm_pb2.GetRequest(key="agent_001_mem001")
    get_resp = stub.Get(get_req)
    print(f"Get found: {get_resp.found}")
    print(f"Key: {get_resp.key}")
    print(f"Payload: {get_resp.payload}")
    print(f"Timestamp: {get_resp.timestamp}")
    print(f"Embedding: {list(get_resp.embedding)}")

    # Test VectorSearch
    print("\n=== Testing VectorSearch ===")
    search_req = agent_swarm_pb2.VectorSearchRequest(
        query_embedding=[0.12, 0.22, 0.32],
        k=2
    )
    search_resp = stub.VectorSearch(search_req)
    print(f"Found {len(search_resp.results)} results:")
    for r in search_resp.results:
        print(f"  Key: {r.key}, Distance: {r.distance:.4f}, Payload: {r.payload}")

    channel.close()

if __name__ == '__main__':
    run()
