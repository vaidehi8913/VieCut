/*
 * Read edges one at a time without duplicates in a stream
 * interface
 *
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

class stream_from_file_io {

public:
    stream_from_file_io(std::string file) {
	file_name = file;
	new_pass();
    }

    virtual ~stream_from_file_io() { 
        close_file_stream();
    }

    NodeID number_of_nodes() {
	return nmbNodes;
    }

    EdgeID number_of_edges() {
	return nmbEdges;
    }

    bool next_edge(NodeID *start, NodeID *end) {
        NodeID inner_start = 0;
	NodeID inner_end = 0;
	    
	bool inner_next_edge = read_next_edge(&inner_start, &inner_end);

	//std::cout << "inner_start: " << inner_start 
	//	<< ", inner_end: " << inner_end << std::endl;

	*start = inner_start - 1;
	*end = inner_end - 1;

        //std::cout << "Streaming edge: (" << *start << ", " << *end << "), " 
	//	<< inner_next_edge << std::endl;

	return inner_next_edge;
    }

    void new_pass() {
	open_file();
	nmbPasses++;
    }

    unsigned number_of_passes() {
	return nmbPasses;
    }

private:
    void open_file() {
	//std::ifstream instream(file_name.c_str());
	file_stream.open(file_name.c_str());

	if (!file_stream) {
	    std::cerr << "Error opening " << file_name << std::endl;
	    exit(2);
	}

	file_is_open = true;

	std::getline(file_stream, line);

	//skip comments
	while (line[0] == '%') {
	    std::getline(file_stream, line);
	}

	int ew = 0; // are the edges weighted in the metis file
	std::stringstream ss(line);
	ss >> nmbNodes;
	ss >> nmbEdges;
	ss >> ew;

	// removing duplicates
	nmbEdges = nmbEdges / 2;

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

	// load the first line (node 1)
	read_next_line();

	//file_stream = instream;
    }

    bool read_next_edge (NodeID *start, NodeID *end) {

	//std::cout << "read_next_edge" << std::endl;

        if (file_is_open) {
	    if (line_ptr < line.size()) {
	        NodeID target = fast_atoi(line, &line_ptr);

	        if (!target) return read_next_edge(start, end);

	        if (target == line_index) {
		    LOG0 << "The graph file contains self-loops. "
		         << "This is not supported. "
		         << "Please remove them from the file. ";
		    return read_next_edge(start, end);
	        }

	        // only return edges with the dest node > source node
	        // to avoid duplicates
	        if (line_index < target) {
		    //std::cout << "line_index: " << line_index 
		    //	    << ", target: " << target << std::endl;

		    *start = line_index;
		    *end = target;
		    return true;
	        } else {
		    return read_next_edge(start, end);
	        }
	
	    } else {
	        if (file_stream.eof()) {
		    close_file_stream();
		    return false;
	        } else {
	    	    read_next_line();
	    	    return read_next_edge(start, end);
	        }	
	    }
	} else {
	    return false;
	}
    }

    void read_next_line() {

	//std::cout << "read_next_line" << std::endl;
	
	if (std::getline(file_stream, line)) {
	    
            if (line[0] == '%') {
		read_next_line();
	    }

	    line_ptr = 0;

	    // remove leading whitespaces
	    while (line[line_ptr] == ' ')
	        ++line_ptr;

	    line_index++;

	} else {
	    close_file_stream();
	}
    }

    void close_file_stream() {
	file_stream.close();
	line_index = 0;
	line_ptr = 0;
	file_is_open = false;
    }

    // orig. from http://tinodidricksen.com/uploads/code/cpp/speed-string-to-int.cpp
    static uint64_t fast_atoi(const std::string& str, size_t* line_ptr) {
	uint64_t x = 0;

	while(str[*line_ptr] >= '0' && str[*line_ptr] <= '9') {
	    x = (x * 10) + (str[*line_ptr] - '0');
	    ++(*line_ptr);
	}
	++(*line_ptr);

	//std::cout << "fast_atoi: " << x << std::endl;

	return x;
    }

    std::string file_name;
    std::ifstream file_stream;
    std::string line;
    NodeID line_index = 0; // metis nodes are 1 indexed
    size_t line_ptr = 0;
    unsigned nmbPasses = 0;
    bool file_is_open = false;
    NodeID nmbNodes;
    EdgeID nmbEdges;
};

