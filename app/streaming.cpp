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
#include "data_structure/union_find.h"

uint64_t exponent(uint64_t base, uint64_t exp);
uint64_t brute_force_mincut(NodeID nmbNodes, std::vector<std::pair<NodeID, NodeID>> edges);


int main(int argn, char** argv) {

    tlx::CmdlineParser cmdl;

    auto cfg = configuration::getConfig();
    size_t fracture_trials = 1;
    cfg->seed = 0;

    cmdl.add_param_string("graph", cfg->graph_filename, "path to graph file");

    cmdl.add_size_t('r', "seed", cfg->seed, "random seed");
    cmdl.add_size_t('t', "trials", fracture_trials, "number of trials");

    if (!cmdl.process(argn, argv)) return -1;

    int fracture_success = 0;
    int fracture_one_component = 0;
    int fracture_too_many_components = 0;

    NodeID nmbNodes = UNDEFINED_NODE;
    EdgeID nmbEdges = UNDEFINED_EDGE;
    NodeID d_min = UNDEFINED_NODE;
    NodeID min_deg_node = UNDEFINED_NODE;

    std::cout << "seed, fractured_components, cut_size, elapsed_time" << std::endl;

    //RUN IT OVER WITH DIFFERENT SEEDS
    for (size_t seed = 1; seed <= fracture_trials; seed++) {

    timer t;

    // print seed;
    std::cout << cfg->seed + seed << ", ";

    random_functions::setSeed(cfg->seed + seed);

    graph_stream *S = streaming_graph_io::readUnweightedGraph(
        configuration::getConfig()->graph_filename, 
	true); // do we want to scramble the edges after we read them in?

    nmbNodes = S->number_of_nodes();
    nmbEdges = S->number_of_edges(); // only for comparison purposes

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
        samplers[edge_start]->stream_in(edge_end);
        samplers[edge_end]->stream_in(edge_start); 
    }

    // enter node pairs into vector
    // (each edge is entered in both directions for compatibility with the
    //  graph format)
    std::vector<std::pair<NodeID, NodeID>> sampled_edges_vec;
    d_min = nmbNodes;
    min_deg_node = UNDEFINED_NODE;

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

	// we never look at the sampler again after this
	delete samplers[i];
    }

    delete [] samplers;

    std::sort(sampled_edges_vec.begin(), sampled_edges_vec.end(),
                [](std::pair<NodeID, NodeID> e1, std::pair<NodeID, NodeID> e2) {
                    if (e1.first == e2.first) {
                        return e1.second < e2.second;
                    }                    
                    return e1.first < e2.first;
                });

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
    std::vector<int32_t> subsampled_components(subsampled_graph->number_of_nodes());
    size_t subsampled_component_count = scc.strong_components(subsampled_graph, 
		    					      &subsampled_components);

    // do shared pointers have to be explicitly deallocated?
    //delete subsampled_graph;

    // check that number of connected components is not too high
    // print fractured_components
    if (subsampled_component_count == 1) {
	fracture_one_component++;
	//std::cout << "Failure: only one connected component!" << std::endl << std::endl;
	std::cout << 1 << ", xx" << std::endl;
	continue; // At this point we have to give up
    } else if (subsampled_component_count > (100 * nmbNodes) / d_min) {
	fracture_too_many_components++;
	//std::cout << "Failure: too many connected components!" << std::endl << std::endl;
	std::cout << subsampled_component_count << ", xx" << std::endl;
	continue; // At this point we have to give up
    } else {
        fracture_success++;
	/*std::cout << "Success! Graph fractured into " 
	    << subsampled_component_count << " components!" 
	    << " (Below the max of " << (100 * nmbNodes) / d_min << ")"
	    << std::endl; */
	std::cout << subsampled_component_count << ", ";
    }

    //second pass
    S->new_pass();

    // should initialize d_min new graphs on component_count many vertices each
    // just need union find for this, not the full graph data structure
    union_find **Fs = new union_find*[d_min];
    
    // once we decide to include an edge, it's not important which 
    // subgraph it came from
    std::vector<std::pair<NodeID, NodeID>> F_edges;

    for (unsigned i = 0; i < d_min; i++) {
	Fs[i] = new union_find(subsampled_component_count);
    }

    edge_start = UNDEFINED_NODE;
    edge_end = UNDEFINED_NODE;

    std::vector<std::pair<NodeID, NodeID>> surviving_edges;

    // see an edge
    while(S->next_edge(&edge_start, &edge_end)) {
        // contract the edge into H
        NodeID contracted_edge_start = subsampled_components[edge_start];
	NodeID contracted_edge_end = subsampled_components[edge_end];

	if (contracted_edge_start == contracted_edge_end) {
	    // this is a self-loop that should be ignored
	    continue;
	}

	surviving_edges.push_back(std::make_pair(contracted_edge_start, contracted_edge_end));

    	// iterate through the union find data structures to find the smallest 
    	// number that supports this edge
	for (unsigned i = 0; i < d_min; i++) {
	    unsigned start_component = Fs[i]->Find(contracted_edge_start);
	    unsigned end_component = Fs[i]->Find(contracted_edge_end);

	    if (start_component != end_component) {
		// this is the minimum valid F
		Fs[i]->Union(start_component, end_component);
		F_edges.push_back(std::make_pair(contracted_edge_start, 
					            contracted_edge_end));
		F_edges.push_back(std::make_pair(contracted_edge_end, 
						    contracted_edge_start));
		break;
	    }	    
	}
    } 

    for (unsigned i = 0; i < d_min; i++) {
	delete Fs[i];
    }
    delete [] Fs;

    std::sort(F_edges.begin(), F_edges.end(),
               [](std::pair<NodeID, NodeID> e1, std::pair<NodeID, NodeID> e2) {
                   if (e1.first == e2.first) {
                       return e1.second < e2.second;
                   }
                   return e1.first < e2.first;
               });

    // remove duplicates
    // (assuming that the algorithm does not want us to create a multigraph)
    i = 0;

    while(i + 1 < F_edges.size()) {
        if (F_edges[i].second == F_edges[i+1].second 
            && F_edges[i].first == F_edges[i+1].first) {
                F_edges.erase(F_edges.begin() + i);
        } else {
            i++;
        }
    }
 
    uint64_t F_mincut = brute_force_mincut(subsampled_component_count, F_edges);

    // calculate the size of this cut in the original graph for comparison purpose
    uint64_t returned_cut_size = 0;

    for (std::pair<NodeID, NodeID> e : surviving_edges) {
	NodeID u = e.first;
	NodeID v = e.second;

	if ( ((F_mincut >> u) & 1) ^ ((F_mincut >> v) & 1) ) {
	    returned_cut_size++;
	}
    }

    // print cut_size
    std::cout << returned_cut_size << ", ";

    // print elapsed_time
    std::cout << t.elapsed() << std::endl;

    delete S;
    }

    std::cout << std::endl << "---------------------------------------------" << std::endl
	    << "SUMMARY" << std::endl
	    << "Nodes: " << nmbNodes << ", Edges: " << nmbEdges << std::endl
	    << "Min degree node: " << min_deg_node << ", of degree: " << d_min << std::endl
	    << "Max feasible component count: " << (100 * nmbNodes) / d_min << std::endl
	    << "Trials: " << fracture_trials << std::endl
	    << "Successes: " << fracture_success << std::endl
	    << "Failures (only one component): " << fracture_one_component << std::endl
	    << "Failures (too many components): " << fracture_too_many_components << std::endl
	    << "---------------------------------------------" << std::endl;
}

// this only works for graphs under 64 nodes!!!
uint64_t brute_force_mincut(NodeID nmbNodes, std::vector<std::pair<NodeID, NodeID>> edges) {

    uint64_t mincut = 0;
    int mincut_value = exponent(nmbNodes, 2);

    for (uint64_t cut = 1; cut < exponent(2, nmbNodes-1); cut++) {
	
	int cut_value = 0;

	for (std::pair<NodeID, NodeID> e : edges) {
	    NodeID u = e.first;
	    NodeID v = e.second;

	    if ( ((cut >> u) & 1) ^ ((cut >> v) & 1) ) {
		cut_value++;
	    }
	}

	if (cut_value < mincut_value) {
	    mincut = cut;
	    mincut_value = cut_value;
	}
    }

    return mincut;
}


// this will easily overflow on large inputs
uint64_t exponent(uint64_t base, uint64_t exp) {
    
    uint64_t working_exp = exp;
    uint64_t result = 1;
    uint64_t working_base = base;

    while(working_exp > 0) {
	uint64_t least_significant_digit = working_exp & 1; 
        working_exp >>= 1;

	if (least_significant_digit > 0) {
	    result *= working_base;
	}

	working_base *= working_base;
    }

    return result;
}
