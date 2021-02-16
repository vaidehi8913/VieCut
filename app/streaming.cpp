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
#include "data_structure/edge_sampler.h"
#include "io/streaming_graph_io.h"
#include "data_structure/graph_stream.h"
#include "algorithms/misc/strongly_connected_components.h"
#include "common/configuration.h"
#include "tlx/cmdline_parser.hpp"
#include "tlx/logger.hpp"
#include "tools/timer.h"

int main(int argn, char** argv) {

    tlx::CmdlineParser cmdl;

    auto cfg = configuration::getConfig();

    cmdl.add_param_string("graph", cfg->graph_filename, "path to graph file");

    cmdl.add_size_t('r', "seed", cfg->seed, "random seed");

    if (!cmdl.process(argn, argv)) return -1;


    int fracture_trials = 1000;
    int fracture_success_tally = 0;
    int fracture_failure_tally = 0;

    //RUN IT OVER WITH DIFFERENT SEEDS
    for (size_t seed = 1; seed <= fracture_trials; seed++) {

    std::cout << "seed: " << seed << std::endl;

    //random_functions::setSeed(cfg->seed);
    random_functions::setSeed(seed);

    graph_stream *S = streaming_graph_io::readUnweightedGraph(
        configuration::getConfig()->graph_filename, 
	false); // do we want to scramble the edges after we read them in?

    NodeID nmbNodes = S->number_of_nodes();

    edge_sampler **samplers = new edge_sampler*[nmbNodes];

    // initialize the samplers
    for (uint64_t i = 0; i < nmbNodes; i++) {
        // each sampler takes two samples
        samplers[i] = new edge_sampler((NodeID) i, 2);
    }

    //stream one pass of edges into samplers
    NodeID edge_start;
    NodeID edge_end;

     while (S->next_edge(&edge_start, &edge_end)) {

	//std::cout << "streaming edge: (" << edge_start << ", " << edge_end << ")" << std::endl;

        samplers[edge_start]->stream_in(edge_end);
        samplers[edge_end]->stream_in(edge_start); 
    }

    std::cout << std::endl;


    // print out the samples for testing purposes
    /*
    std::cout << "SAMPLES:" << std::endl;
    for (uint64_t i = 0; i < nmbNodes; i++) {
        auto samples = samplers[i]->get_samples();
	//we know that there are two samples
	std::cout << "node " << i << ": [" << 
    	samples[0] << ", " << samples[1] << "]" << std::endl;
    }
    std::cout << std::endl;
    */

    // enter node pairs into vector
    // (each edge is entered in both directions for compatibility with the
    //  graph format)
    std::vector<std::pair<NodeID, NodeID>> sampled_edges_vec;
    NodeID d_min = nmbNodes;
    NodeID min_deg_node = UNDEFINED_NODE;

    for (uint64_t i = 0; i < nmbNodes; i++) {
        uint64_t sample_count = samplers[i]->get_number_of_samples();
        NodeID *i_samples = samplers[i]->get_samples();

        for (uint64_t j = 0; j < sample_count; j++) {
                // (assuming that edges have to be entered in both directions)
                sampled_edges_vec.push_back(std::make_pair(i_samples[j], i));
                sampled_edges_vec.push_back(std::make_pair(i, i_samples[j]));
        }

        NodeID i_deg = samplers[i]->get_degree();
        if (i_deg < d_min) {
            d_min = i_deg;
            min_deg_node = i;
        }
    }

    // TESTING
    
    if (seed == fracture_trials) {
        std::cout << "MIN DEGREE NODE: " << min_deg_node << std::endl;
        std::cout << "MIN DEGREE: " << d_min << std::endl << std::endl;
    }

    /*
    std::cout << "SAMPLED EDGES VEC (pre sorting):" << std::endl;
    for (std::pair<NodeID, NodeID> edge : sampled_edges_vec) {
	std::cout << "(" << edge.first << ", " << edge.second << ")" << std::endl;
    }
    */

    std::sort(sampled_edges_vec.begin(), sampled_edges_vec.end(),
                [](std::pair<NodeID, NodeID> e1, std::pair<NodeID, NodeID> e2) {
                    if (e1.first == e2.first) {
                        return e1.second < e2.second;
                    }
                    
                    return e1.first < e2.first;
                });

    /*
    std::cout << "SAMPLED EDGES VEC (post sort, pre duplicate removal):" << std::endl;
    for (std::pair<NodeID, NodeID> edge : sampled_edges_vec) {
	std::cout << "(" << edge.first << ", " << edge.second << ")" << std::endl;
    }
    */

    // throw out duplicates
    // (need to do this now so that we know the number of edges to 
    // initialize the graph with)
    uint64_t i = 0;

    while(i + 1 < sampled_edges_vec.size()) {
        if (sampled_edges_vec[i].second == sampled_edges_vec[i+1].second 
            && sampled_edges_vec[i].first == sampled_edges_vec[i+1].first) {
                sampled_edges_vec.erase(sampled_edges_vec.begin() + i);
        } else {
            i++;
        }
    }

    /*
    std::cout << "SAMPLED EDGES VEC (post duplicate removal):" << std::endl;
    for (std::pair<NodeID, NodeID> edge : sampled_edges_vec) {
	std::cout << "(" << edge.first << ", " << edge.second << ")" << std::endl;
    }
    */

    int nmbSampledEdges = sampled_edges_vec.size();

    // initialize graph
    std::shared_ptr<graph_access> subsampled_graph = std::make_shared<graph_access>();
    // need this to be a shared pointer to use strongly_connected_components.h
    
    subsampled_graph->start_construction(nmbNodes, nmbSampledEdges);

    // enter sampled edges into a graph
    NodeID vec_last_src = UNDEFINED_NODE;
    NodeID graph_last_src = UNDEFINED_NODE;

    for (std::pair<NodeID, NodeID> e : sampled_edges_vec) {
        NodeID u = e.first;
        NodeID v = e.second;

        if (u != vec_last_src) {
            graph_last_src = subsampled_graph->new_node();
            vec_last_src = u;
        }

        subsampled_graph->new_edge(graph_last_src, v);
    }

    subsampled_graph->finish_construction();
    subsampled_graph->computeDegrees(); 

    // find the connected components of the subsampled graph
    // (example: app/temporal_largest_cc.cpp:49)
    strongly_connected_components scc;
    std::vector<int32_t> components(subsampled_graph->number_of_nodes());
    size_t component_count = scc.strong_components(subsampled_graph, &components);

    /*
    std::cout << "CONNECTED COMPONENTS: " << std::endl;
    for (uint32_t i = 0; i < subsampled_graph->number_of_nodes(); i++) {
	std::cout << "node " << i << " in component " << components[i] << std::endl;
    }
    std::cout << std::endl;
    */

    // check that number of connected components is not too high
    if (component_count == 1) {
	fracture_failure_tally++;
	std::cout << "Failure: only one connected component!" << std::endl << std::endl;
    } else if (component_count > (100 * nmbNodes) / d_min) {
	fracture_failure_tally++;
	std::cout << "Failure: too many connected components!" << std::endl << std::endl;
    } else {
        fracture_success_tally++;
	std::cout << "Success! Graph fractured into " << component_count << " components!" 
	    << " (Below the max of " << (100 * nmbNodes) / d_min << ")"
	    << std::endl << std::endl;
    }
    //second pass
    S->new_pass();

    // should initialize d_min new graphs on component_count many vertices each


    // delete the samplers
    }

    std::cout << "OUT OF " << fracture_trials << " TRIALS, " << fracture_success_tally 
	    << " SUCCESSES AND " << fracture_failure_tally << " FAILURES" << std::endl;
}
