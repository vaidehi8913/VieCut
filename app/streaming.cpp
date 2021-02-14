/*
    Main function for streaming min cut
*/

#include <stdlib.h>
#include <time.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/definitions.h"
#include "lib/data_structure/edge_sampler.h"
#include "lib/algorithms/misc/strongly_connected_components.h"

int main(int argn, char** argv) {

    cmdl.add_param_string("graph", configuration::getConfig()->graph_filename,
                          "path to graph file");

    graph_stream S = streaming_graph_io::readUnweightedGraph(
        configuration::getConfig()->graph_filename, false);

    NodeID nmbNodes = S->number_of_nodes();
    edge_sampler *samplers = new edge_sampler[nmbNodes];

    // initialize the samplers
    for (int i = 0; i < nmbNodes; i++) {
        // each sampler takes two samples
        samplers[i] = new edge_sampler((NodeID) i, 2);
    }

    //stream one pass of edges into samplers
    NodeID edge_start;
    NodeID edge_end;

    while (S->next_edge(&edge_start, &edge_end)) {
        samplers[edge_start]->stream_in(edge_end);
        samplers[edge_end]->stream_in(edge_start);
    }

    // enter node pairs into vector
    // (each edge is entered in both directions for compatibility with the
    //  graph format)
    std::vector<std::pair<NodeID, NodeID>> sampled_edges_vec;
    NodeID d_min = nmbNodes;
    NodeID min_deg_node = UNDEFINED_NODE;

    for (int i = 0; i < nmbNodes; i++) {
        uint64_t sample_count = samplers[i].get_number_of_samples();
        NodeID *i_samples = samplers[i].get_samples();

        for (j = 0; j < sample_count; j++) {
                // (assuming that edges have to be entered in both directions)
                sampled_edges_vec.push_back(std::make_pair(i_samples[j], i));
                sampled_edges_vec.push_back(std::make_pair(i, i_samples[j]));
        }

        NodeID i_deg = samplers[i].get_degree();
        if (i_deg < d_min) {
            d_min = i_deg;
            min_deg_node = i;
        }
    }

    std::sort(sampled_edges_vec.begin(), sampled_edges_vec.end(),
                [](std::pair<NodeID, NodeID>> e1, std::pair<NodeID, NodeID>> e2) {
                    if (e1.first() == e2.first()) {
                        return e1.second() > e2.second();
                    }
                    
                    return e1.first() > e1.first();
                });

    // throw out duplicates
    // (need to do this now so that we know the number of edges to 
    // initialize the graph with)
    int i = 0;

    while(i + 1 < sampled_edges_vec.size()) {
        if (sampled_edges_vec[i].second() == sampled_edges_vec[i+1].second() 
            && sampled_edges_vec[i].first() == sampled_edges_vec[i+1].first()) {
                sampled_edges_vec.erase(sampled_edges_vec.begin() + i);
        } else {
            i++;
        }
    }

    int nmbSampledEdges = sampled_edges_vec.size();

    // initialize graph
    std::shared_ptr<graph_access> subsampled_graph = std::make_shared<graph_access>();
        // need this to be a shared pointed to use strongly_connected_components.h
        //graph_access subsampled_graph = new graph_access();
    subsampled_graph->start_construction(nmbNodes, nmbSampledEdges);

    // enter sampled edges into a graph
    NodeID vec_last_src = UNDEFINED_NODE;
    NodeID graph_last_src = UNDEFINED_NODE;

    for (std::pair<NodeID, NodeID> e : sampled_edges_vec) {
        NodeID u = e.first();
        NodeID v = e.second();

        if (u != vec_last_src) {
            graph_last_src = G.new_node();
            vec_last_src = u;
        }

        subsampled_graph->new_edge(graph_last_src, v);
    }

    subsampled_graph->finish_constructions();
    subsampled_graph->compute_degrees();

    // find the connected components of the subsampled graph
    // (example: app/temporal_largest_cc.cpp:49)
    strongly_connected_components scc;
    std::vector<int32_t> components(subsampled_graph->number_of_nodes());
    size_t component_count = scc.strong_components(subsampled_graph, &components);

    // check that number of connected components is not too high
    if (component_count > (100 * nmbNodes) / d_min) {
        // some sort of failure behavior should occur here
    }

    //second pass
    S->new_pass();

    // should initialize d_min new graphs on component_count many vertices each


    // delete the samplers

}