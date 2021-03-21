#!/bin/bash

echo eu-2005.graph_core_100

./build/streaming -t 10 -l 1 -S 1 -s -o test_results/eu-2005-gc100-s1e.txt /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_100


echo eu-2005.graph_core_60

./build/streaming -t 10 -l 1 -S 1 -s -o test_results/eu-2005-gc60-s1e.txt /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_60


echo eu-2005.graph_core_25

./build/streaming -t 10 -l 1 -S 1 -s -o test_results/eu-2005-gc25-s1e.txt /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_25


echo eu-2005.graph_core_10

./build/streaming -t 10 -l 1 -S 1 -s -o test_results/eu-2005-gc10-s1e.txt /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_10


echo com-orkut.metis_core_100

./build/streaming -t 10 -l 70 -S 1 -s -o test_results/orkut-core-gc100-s1e.txt /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_100


echo com-orkut.metis_core_98

./build/streaming -t 10 -l 76 -S 1 -s -o test_results/orkut-gc98-s1e.txt /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_98


echo com-orkut.metis_core_95

./build/streaming -t 10 -l 89 -S 1 -s -o test_results/orkut-gc95-s1e.txt /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_95


echo com-orkut.metis_core_16

./build/streaming -t 10 -l 14 -S 1 -s -o test_results/orkut-gc16-s1e.txt /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_16


