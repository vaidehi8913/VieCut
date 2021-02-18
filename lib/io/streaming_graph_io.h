/* 
    Read metis format into a "stream"
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "common/definitions.h"
#include "data_structure/graph_stream.h"
#include "tools/string.h"

class streaming_graph_io {

public:
    streaming_graph_io() { }

    virtual ~streaming_graph_io() { }

    // mostly copied from lib/io/graph_io.h
    static graph_stream *readUnweightedGraph(std::string file, bool scramble_array) {

        std::string line;

        //open file for reading
        std::ifstream instream(file.c_str());
        if (!instream) {
            std::cerr << "Error opening " << file << std::endl;
            exit(2);
        }

        uint64_t nmbNodes;
        uint64_t nmbEdges;

        std::getline(instream, line);

        // skip comments
        while (line[0] == '%') {
            std::getline(instream, line);
        }

        int ew = 0; // are the edges weighted in the metis file?
        std::stringstream ss(line);
        ss >> nmbNodes;
        ss >> nmbEdges;
        ss >> ew;

        bool read_ew = false;

        if (ew == 1) {
            read_ew = true;
            // graph has edge weights
        } else if (ew == 11) {
            read_ew = true;
            // graph has vertex weights and edge weights
        }

        if (read_ew) {
            std::cerr << "Weighted graphs not supported for streams." << std::endl;
            exit(4);
        }
      
	std::pair<NodeID, NodeID> *edges = new std::pair<NodeID, NodeID>[nmbEdges];

        EdgeID edge_counter = 0;
        NodeID node_counter = 0;

        while (std::getline(instream, line)) {

            // skip comments
            if (line[0] == '%') {
                continue;
            }

            size_t line_ptr = 0;

            // remove leading whitespaces
            while (line[line_ptr] == ' ')
                ++line_ptr;

            while (line_ptr < line.size()) {
                NodeID target = fast_atoi(line, &line_ptr);
	
		// DEBUG	
		//std::cout << "reading target: " << target << std::endl;

                if (!target) break;

                // check for self-loops
                if (target - 1 == node_counter) {
                    LOG0 << "The graph file contains self-loops. "
                         << "This is not supported. "
                         << "Please remove them from the file. ";
                    continue;
                }

                // only enter edges when the dest node > source node, 
                // to avoid duplicates
                if (node_counter < target - 1) {
                    //edge_starts[edge_counter] = node_counter;
                    //edge_ends[edge_counter] = target - 1;
                    edges[edge_counter] = std::make_pair(node_counter, target-1);
		    edge_counter++;
                }
            }

            node_counter++;

            if (instream.eof()) {
                break;
            }
        }

        // Check to make sure file is consistent
        if (edge_counter != (EdgeID) nmbEdges) {
            std::cerr << "number of specified edges mismatch" << std::endl;
            std::cerr << edge_counter << " " << nmbEdges << std::endl;
            // Don't exit, as it could just be bc we threw out self loops
        }

        if (node_counter != (NodeID) nmbNodes) {
            std::cerr << "number of specified nodes mismatch" << std::endl;
            std::cerr << node_counter << " " << nmbNodes << std::endl;
            exit(4);
        }

        if (scramble_array) {
            for (unsigned i = 0; i < nmbEdges-1; i++) {
		int swap_with = random_functions::nextInt(i + 1, nmbEdges - 1);

		std::pair<NodeID, NodeID> temp = edges[i];
		edges[i] = edges[swap_with];
		edges[swap_with] = temp;
	    }
        }


	// DEBUG
	/*
	for (int i = 0; i < nmbEdges; i++) {
	    std::cout << "read edge " << i << ": (" << edge_starts[i] << ", " 
		    << edge_ends[i] << ")" << std::endl;
	}

	std::cout << "nmbEdges: " << nmbEdges << std::endl;
	std::cout << "final edge_counter: " << edge_counter << std::endl;
	*/

        graph_stream *S = new graph_stream((NodeID) nmbNodes, (EdgeID) nmbEdges,
                                          edges);

        return S;

    }

    // copied from graph_io 
    // (there must be a way to make this a child class or something to avoid this copy paste)
    
private:
    // orig. from http://tinodidricksen.com/uploads/code/cpp/speed-string-to-int.cpp
    static uint64_t fast_atoi(const std::string& str, size_t* line_ptr) {
	uint64_t x = 0;

	while (str[*line_ptr] >= '0' && str[*line_ptr] <= '9') {
	    x = (x * 10) + (str[*line_ptr] - '0');
	    ++(*line_ptr);
	}
	++(*line_ptr);
	return x;
    }

};
