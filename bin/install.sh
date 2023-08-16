#!/bin/bash

# ----------------------------------------------------------------
#
# VARIABLES
#
# ----------------------------------------------------------------


BOLD=`tput bold`
RED=`tput setaf 1`
GREEN=`tput setaf 10`
RESET=`tput sgr0`

tegia_folder=$(realpath ../../../)
configuration_folder=$(realpath ../)

mysql_host=$(awk -F= '/^host/ {print $2; exit}' $tegia_folder/tegia.cnf)
mysql_port=$(awk -F= '/^port/ {print $2; exit}' $tegia_folder/tegia.cnf)
mysql_user=$(awk -F= '/^user/ {print $2; exit}' $tegia_folder/tegia.cnf)
mysql_password=$(awk -F= '/^password/ {print $2; exit}' $tegia_folder/tegia.cnf)


# ----------------------------------------------------------------
#
# CONFIG FILE
#
# ----------------------------------------------------------------


sed -e "s/{CONFIGURATION_FOLDER}/$(echo $configuration_folder | sed -E 's/(\W)/\\\1/g')/g" \
    -e "s/{MYSQL_HOST}/$(echo $mysql_host | sed -E 's/(\W)/\\\1/g')/g" \
    -e "s/{MYSQL_PORT}/$mysql_port/g" \
    -e "s/{MYSQL_USER}/$(echo $mysql_user | sed -E 's/(\W)/\\\1/g')/g" \
    -e "s/{MYSQL_PASSWORD}/$(echo $mysql_password | sed -E 's/(\W)/\\\1/g')/g" \
    base.config-example.json > base.config.json

# validate

if jq -e . base.config.json >/dev/null; then
    echo "base.config.json is all good!"
else
    echo "ERROR IN base.config.json! CANNOT VALIDATE THE JSON!"
    exit 1
fi


# ----------------------------------------------------------------
#
# VENDORS
#
# ----------------------------------------------------------------


mkdir -p ../vendors
cd ../vendors
git clone https://github.com/staticlibs/ccronexpr.git


# ----------------------------------------------------------------
#
# DATABASE
#
# ----------------------------------------------------------------


for file in ../db/install/*.sql
do
	mysql --defaults-extra-file=$tegia_folder/tegia.cnf < "$file"
done


for file in ../db/update/*.sql
do
	mysql --defaults-extra-file=$tegia_folder/tegia.cnf < "$file"
done


exit 0;
