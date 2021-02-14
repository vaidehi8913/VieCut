/*
    a structure that samples edges uniformly from a stream
*/

#include <algorithm>
#include <bitset>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <random>

#include "common/definitions.h"
#include "tools/random_functions.h"

class edge_sampler {

public:
    edge_sampler(NodeID assoc_node, uint64_t sample_count) {
        associated_node = assoc_node;
        nmbSamples = sample_count;
        samples = new NodeID[nmbSamples];
        degree = 0;
    }

    virtual ~edge_sampler() {
        // TO DO: should the array be deallocated here?
        delete [] samples;
    }

    // assume we are seeing an edge between associated_node and 
    // dest_node
    void stream_in(NodeID dest_node) {
        degree++;

        for (int i = 0; i < nmbSamples; i++) {
            r = random_functions::next_Int(0, degree);

            if (r == 0) {
                samples[i] = dest_node;
            }
        }
    }

    NodeID get_associated_node() {
        return associated_node;
    }

    NodeID get_degree() {
        return degree;
    }

    uint64_t get_number_of_samples() {
        return nmbSamples;
    }

    NodeID *get_samples() {
        return samples;
    }


    NodeID associated_node;
    uint64_t nmbSamples;
    NodeID *samples;
    NodeID degree;
}