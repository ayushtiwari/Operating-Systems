#! /bin/bash

echo "Cleaning graph..."
awk -f "graph_1.awk" < 1f.graph.edgelist | sort -n -k1 -k2 > 1f_output.graph.edgelist
echo "Cleaned graph written to 1f_output.graph.edgelist"

echo "Calculating degrees..."
awk -f "graph_2.awk" < 1f_output.graph.edgelist | sort -nr -k2 | head -5
echo "Done"
