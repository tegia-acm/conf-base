# !/bin/bash

config_path=${1}
datapath=${2}
sphinx_path=${3}

rm -f ${config_path}/_all_index.conf

for file in `find ${config_path}/indexes -name "*.conf"`
do
   cat $file >> ${config_path}/_all_index.conf
   sh -c "echo '\n\n' >> ${config_path}/_all_index.conf"
done

cat ${config_path}/_indexer_searchd.conf >> ${config_path}/_all_index.conf

VALUE=$(echo ${datapath} | sed -e "s#/#\\\/#g")
sed -i "s/{DATA_PATH}/$VALUE/g" ${config_path}/_all_index.conf

${sphinx_path}/searchd --stopwait --config ${config_path}/_all_index.conf
${sphinx_path}/searchd --config ${config_path}/_all_index.conf
