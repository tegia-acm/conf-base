#!/bin/bash


# ////////////////////////////////////////////////////////////////////////////////
#
# VARIABLES
#
# ////////////////////////////////////////////////////////////////////////////////


RED=`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`

_OK_="${GREEN}[OK]  ${RESET}"
_ERR_="${RED}[ERR] ${RESET}"


# ////////////////////////////////////////////////////////////////////////////////
#
# SPHINX
#
# ////////////////////////////////////////////////////////////////////////////////

#
# TEGIA FOLDER
#

conf_base_path=$(realpath ../)
echo "$conf_base_path"

read -rp "Укажите каталог установки tegia-node [/home/$USER/tegia]: " tegia_folder
tegia_folder="/tegia"

#
# Install Sphinx Search
#

cd $tegia_folder
mkdir -p ./sphinx
cd ./sphinx
wget -N http://sphinxsearch.com/files/sphinx-3.1.1-612d99f-linux-amd64.tar.gz
tar zxf sphinx-3.1.1-612d99f-linux-amd64.tar.gz

#
# COPY DATA
#

cp -avr $conf_base_path/actors/A2Sphinx/sphinx/sphinxdata $tegia_folder/sphinx/sphinxdata
cp -avr $conf_base_path/actors/A2Sphinx/sphinx/configs $tegia_folder/sphinx/configs

#

ln -s $conf_base_path/actors/A2Sphinx/sphinx/indexing.sh $tegia_folder/sphinx/indexing.sh
ln -s $conf_base_path/actors/A2Sphinx/sphinx/generate_conf.sh $tegia_folder/sphinx/generate_conf.sh


# ////////////////////////////////////////////////////////////////////////////////
#
# VENDORS
#
# ////////////////////////////////////////////////////////////////////////////////


cd $conf_base_path
mkdir -p vendors
cd vendors
git clone https://github.com/staticlibs/ccronexpr.git


echo "${_OK_}Установка успешно завершена"
exit 0


