#!/bin/bash

echo eu-2005.graph_core_10

./build/streaming -t 10 -l 1 -s -o test_results/eu-2005-core-10-10-iters.txt /data/noea45cs/huge_mincut/bench/eu-2005.graph_core_10

echo com-orkut.metis_core_100

./build/streaming -t 10 -l 70 -s -o test_results/orkut-core-100-10-iters.txt /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_100

echo com-orkut.metis_core_16

./build/streaming -t 10 -l 14 -s -o test_results/orkut-core-16-10-iters.txt /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_16

echo com-orkut.metis_core_95

./build/streaming -t 10 -l 89 -s -o test_results/orkut-core-95-10-iters.txt /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_95

echo com-orkut.metis_core_98

./build/streaming -t 10 -l 76 -s -o test_results/orkut-core-98-10-iters.txt /data/noea45cs/huge_mincut/bench/com-orkut.metis_core_98

echo gsh-2015-host.metis_core_10

./build/streaming -t 10 -l 1 -s -o test_results/gsh-2015-core-10-10-iters.txt /data/noea45cs/huge_mincut/bench/gsh-2015-host.metis_core_10

echo gsh-2015-host.metis_core_100

./build/streaming -t 10 -l 1 -s -o test_results/gsh-2015-core-100-10-iters.txt /data/noea45cs/huge_mincut/bench/gsh-2015-host.metis_core_100

echo gsh-2015-host.metis_core_1000

./build/streaming -t 10 -l 1 -s -o test_results/gsh-2015-core-1000-10-iters.txt /data/noea45cs/huge_mincut/bench/gsh-2015-host.metis_core_1000

echo gsh-2015-host.metis_core_50

./build/streaming -t 10 -l 1 -s -o test_results/gsh-2015-core-50-10-iters.txt /data/noea45cs/huge_mincut/bench/gsh-2015-host.metis_core_50

echo hollywood-2011.metis_core_100

./build/streaming -t 10 -l 77 -s -o test_results/hollywood-2011-core-100-10-iters.txt /data/noea45cs/huge_mincut/bench/hollywood-2011.metis_core_100

echo hollywood-2011.metis_core_20

./build/streaming -t 10 -l 1 -s -o test_results/hollywood-2011-core-20-10-iters.txt /data/noea45cs/huge_mincut/bench/hollywood-2011.metis_core_20

echo hollywood-2011.metis_core_200

./build/streaming -t 10 -l 27 -s -o test_results/hollywood-2011-core-200-10-iters.txt /data/noea45cs/huge_mincut/bench/hollywood-2011.metis_core_200

echo hollywood-2011.metis_core_60

./build/streaming -t 10 -l 6 -s -o test_results/hollywood-2011-core-60-10-iters.txt /data/noea45cs/huge_mincut/bench/hollywood-2011.metis_core_60

echo twitter-2010.metis_core_25

./build/streaming -t 10 -l 1 -s -o test_results/twitter-2010-core-25-10-iters.txt /data/noea45cs/huge_mincut/bench/twitter-2010.metis_core_25

echo twitter-2010.metis_core_30

./build/streaming -t 10 -l 1 -s -o test_results/twitter-2010-core-30-10-iters.txt /data/noea45cs/huge_mincut/bench/twitter-2010.metis_core_30

echo twitter-2010.metis_core_50

./build/streaming -t 10 -l 3 -s -o test_results/twitter-2010-core-50-10-iters.txt /data/noea45cs/huge_mincut/bench/twitter-2010.metis_core_50

echo twitter-2010.metis_core_60

./build/streaming -t 10 -l 3 -s -o test_results/twitter-2010-core-60-10-iters.txt /data/noea45cs/huge_mincut/bench/twitter-2010.metis_core_60

echo uk-2002.graph_core_10

./build/streaming -t 10 -l 1 -s -o test_results/uk-2002-core-10-10-iters.txt /data/noea45cs/huge_mincut/bench/uk-2002.graph_core_10

echo uk-2002.graph_core_100

./build/streaming -t 10 -l 1 -s -o test_results/uk-2002-core-100-10-iters.txt /data/noea45cs/huge_mincut/bench/uk-2002.graph_core_100

echo uk-2002.graph_core_30

./build/streaming -t 10 -l 1 -s -o test_results/uk-2002-core-30-10-iters.txt /data/noea45cs/huge_mincut/bench/uk-2002.graph_core_30

echo uk-2002.graph_core_50

./build/streaming -t 10 -l 1 -s -o test_results/uk-2002-core-50-10-iters.txt /data/noea45cs/huge_mincut/bench/uk-2002.graph_core_50

echo uk-2007-05.graph_core_10

./build/streaming -t 10 -l 1 -s -o test_results/uk-2007-core-10-10-iters.txt /data/noea45cs/huge_mincut/bench/uk-2007-05.graph_core_10

echo uk-2007-05.graph_core_100

./build/streaming -t 10 -l 1 -s -o test_results/uk-2007-core-100-10-iters.txt /data/noea45cs/huge_mincut/bench/uk-2007-05.graph_core_100

echo uk-2007-05.graph_core_1000

./build/streaming -t 10 -l 1 -s -o test_results/uk-2007-core-1000-10-iters.txt /data/noea45cs/huge_mincut/bench/uk-2007-05.graph_core_1000

echo uk-2007-05.graph_core_50

./build/streaming -t 10 -l 1 -s -o test_results/uk-2007-core-50-10-iters.txt /data/noea45cs/huge_mincut/bench/uk-2007-05.graph_core_50
