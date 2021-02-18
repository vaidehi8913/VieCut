/* 
    Represents a stream of unweighted edges in a simple graph
*/

#pragma once

#include <algorithm>
#include <bitset>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/definitions.h"

class graph_stream {

// METHODS TO CREATE THE GRAPH STREAM

public:
    graph_stream(NodeID nodeCount, EdgeID edgeCount, 
                 std::pair<NodeID, NodeID> *edge_array) { 
        nmbPasses = 1;
        nmbNodes = nodeCount;
        nmbEdges = edgeCount;
        edges = edge_array;
        edge_pointer = 0;
    }

    virtual ~graph_stream() {
        delete [] edges;
    }

    /* ============================================================= */
    /* stream access methods */
    /* ============================================================= */

    NodeID number_of_nodes() {
        return nmbNodes;
    }

    // there must be a better way to return the edge info
    // see the edge without advancing the stream
    // note that this shows what would be the next edge, so to speak
    void view_edge(NodeID *start, NodeID *end) {
        // should not ever be possible for the edge_pointer 
        // to leave the array
        *start = edges[edge_pointer - 1].first;
        *end = edges[edge_pointer - 1].second;
    }

    // move to the next edge in the stream
    // return false and last edge again if pointer cannot be updated
    bool next_edge(NodeID *start, NodeID *end) {
        if (edge_pointer == nmbEdges) {
            view_edge(start, end);
            return false;
        }

	edge_pointer++;
        view_edge(start, end);
	return true;
    }

    // start a new pass in the same order
    void new_pass() {
        nmbPasses++;
        edge_pointer = 0;
    }

    uint64_t number_of_passes() {
        return nmbPasses; 
    }

    
    uint64_t nmbPasses;
    NodeID nmbNodes;
    EdgeID nmbEdges;
    std::pair<NodeID, NodeID> *edges;
    EdgeID edge_pointer;
};
