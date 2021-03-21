#!/bin/bash

echo eu-2005.graph_core_10

echo inexact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_10 inexact > test_results/inexact-eu-2005-gc-10.txt

echo exact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_10 exact > test_results/exact-eu-2005-gc-10.txt


echo eu-2005.graph_core_100

echo inexact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_100 inexact > test_results/inexact-eu-2005-gc-100.txt

echo exact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_100 exact > test_results/exact-eu-2005-gc-100.txt


echo eu-2005.graph_core_25

echo inexact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_25 inexact > test_results/inexact-eu-2005-gc-25.txt

echo exact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_25 exact > test_results/exact-eu-2005-gc-25.txt


echo eu-2005.graph_core_60

echo inexact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_60 inexact > test_results/inexact-eu-2005-gc-60.txt

echo exact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_10 exact > test_results/exact-eu-2005-gc-60.txt


echo com-orkut.metis_core_100

echo inexact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_100 inexact > test_results/inexact-orkut-gc-100.txt

echo exact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_100 exact > test_results/exact-orkut-gc-100.txt


echo com-orkut.metis_core_16

echo inexact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_16 inexact > test_results/inexact-orkut-gc-16.txt

echo exact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_16 exact > test_results/exact-orkut-gc-16.txt


echo com-orkut.metis_core_95

echo inexact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_95 inexact > test_results/inexact-orkut-gc-95.txt

echo exact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_95 exact > test_results/exact-orkut-gc-95.txt


echo com-orkut.metis_core_98

echo inexact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_98 inexact > test_results/inexact-orkut-gc-98.txt

echo exact

./build/mincut_parallel -i 10 /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_98 exact > test_results/exact-orkut-gc-98.txt


