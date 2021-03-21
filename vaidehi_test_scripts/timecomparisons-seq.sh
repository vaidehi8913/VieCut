#!/bin/bash

echo eu-2005.graph_core_10

./build/mincut -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_10 vc > test_results/vc-eu-2005-gc-10.txt


echo eu-2005.graph_core_100

./build/mincut -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_100 vc > test_results/vc-eu-2005-gc-100.txt


echo eu-2005.graph_core_25

./build/mincut -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_25 vc > test_results/vc-eu-2005-gc-25.txt


echo eu-2005.graph_core_60

./build/mincut -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_60 vc > test_results/vc-eu-2005-gc-60.txt


echo com-orkut.metis_core_100

./build/mincut -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_100 vc > test_results/vc-orkut-gc-100.txt


echo com-orkut.metis_core_16

./build/mincut -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_16 vc > test_results/vc-orkut-gc-16.txt


echo com-orkut.metis_core_95

./build/mincut -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_95 vc > test_results/vc-orkut-gc-95.txt

echo com-orkut.metis_core_98

./build/mincut -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_98 vc > test_results/vc-orkut-gc-98.txt

