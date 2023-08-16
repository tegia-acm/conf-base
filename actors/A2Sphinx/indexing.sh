# !/bin/bash

sphinx_path=${1}
config_path=${2}
index=${3}

${sphinx_path}/indexer ${index} --config ${config_path}/_all_index.conf --rotate
