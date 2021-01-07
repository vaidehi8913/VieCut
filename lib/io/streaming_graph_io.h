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


class streaming_graph_io {

public:
    graph_io() { }

    virtual ~graph_io() { }

    // mostly copied from lib/io/graph_io.h
    static graph_stream readUnweightedGraph(std::string file, bool scramble_array) {

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

        NodeID *edge_starts = new NodeID[nmbEdges];
        NodeID *edge_ends = new NodeID[nmbEdges];
        
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
                if (!target) break;

                // check for self-loops
                if (target - 1 == node) {
                    LOG0 << "The graph file contains self-loops. "
                         << "This is not supported. "
                         << "Please remove them from the file. ";
                    continue;
                }

                // only enter edges when the dest node > source node, 
                // to avoid duplicates
                if (node_counter < target) {
                    edge_starts[edge_counter] = node_counter;
                    edge_ends[edge_counter] = target;
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
            std::cerr << node_counter << " " < nmbNodes << std::endl;
            exit(4);
        }

        if (scramble_array) {
            // TO DO: scramble the array
        }

        graph_stream S = new graph_stream((NodeID) nmbNodes, (EdgeID) nmbEdges,
                                          edge_starts, edge_ends);

        return S;

    }

}