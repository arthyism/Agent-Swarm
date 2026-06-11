#include "../hnswlib/hnswlib/hnswlib.h"
#include <random>
#include <iostream>
#include <queue>

int main(){
    int dim =16;
    int max_elements = 10000;
    int M = 16;

    int ef_construction = 200;

    hnswlib::L2Space space(dim);
    hnswlib::HierarchicalNSW<float>* alg_hnsw = new hnswlib::HierarchicalNSW<float>(&space, max_elements,M, ef_construction);

    std::mt19937 rng;
    rng.seed(47);
    std::uniform_real_distribution <> distrib_real;
    float* data = new float[dim * max_elements];

    for (int i =0;i<dim*max_elements;i++){
        // if(i!=0){
        //     std::cout<<data[i-1]<<std::endl;
        // }
        data[i]= distrib_real(rng);
    }

    //add data to index
    for (int i=0;i<max_elements;i++){
        alg_hnsw->addPoint(data+i*dim,i);
    }

    //test : query each element
    float correct =0;
    for (int i=0;i<max_elements;i++){
        std::priority_queue<std::pair<float,hnswlib::labeltype>> result=
            alg_hnsw->searchKnn(data+i*dim,1);
            hnswlib::labeltype label = result.top().second;
            if(label==i)correct++;
    }
    float recall = correct/max_elements;
    std::cout<<recall<<std::endl;

// ========== QUERY FOR ELEMENT 1 ==========
    float query_label = 0.456 ;                           // element we want neighbours of
    int k = 10;                                    // number of neighbours to retrieve
    // If you want ALL neighbours: set k = max_elements (but this is slow)

    auto result = alg_hnsw->searchKnn(data + query_label * dim, k);

    std::cout << "Neighbours of element " << query_label
              << " (from closest to farthest):\n";
    int rank = 1;
    while (!result.empty()) {
        auto [dist, label] = result.top();
        result.pop();
        std::cout << "#" << rank++ << "  label = " << label
                  << "  distance = " << dist << "\n";
    }

    delete[] data;
    delete alg_hnsw;
    return 0;
}