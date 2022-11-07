#!/bin/bash

# ------ WOLF license_manager script ------
#
# This script is used for managing the license headers of code files (.h, .c, .cpp, .hpp)
# This file is automatically called by the CI in gitlab.

line_start_mark="// Copyright (C)"
line_end_mark="// along with this program.  If not, see <http://www.gnu.org/licenses/>."

#options
tmp=false
mode="none"
path=""
exclude_folder="include/yaml-schema-cpp/exprtk"
#recursive=true
license=""

for i in "$@"; do
  case $i in
    --temp)
      tmp=true
      shift # past argument=value
      ;;
    --path=*)
      path="${i#*=}"
      shift # past argument=value
      ;;
    --license-header=*)
      license="${i#*=}"
      shift # past argument=value
      ;;
    --add)
      mode="add"
      if [ $mode == "update" ]; then
        echo "Error: Script cannot be called with both options: --update or --add"
        exit 1
      fi
      shift # past argument=value
      ;;
    --update)
      mode="update"
      if [ $mode == "add" ]; then
        echo "Error: Script cannot be called with both options: --update or --add"
        exit 1
      fi
      shift # past argument=value
      ;;
    --exclude=*)
      exclude_folder="${i#*=}"
      shift # past argument=value
      ;;
    *)
      # unknown option
      ;;
  esac
done

# check options
if [ "$path" == "" ]; then
  echo 'Please, provide the path to the folder containing the code with --path=/my/folder/code'
  exit 1
else
  if [ -d "$path" ]; then
    echo "Valid path: ${path}"
  else
    echo "Error: ${path} not found. Can not continue."
  exit 1
  fi
fi

if [ "$license" == "" ]; then
  echo 'Error: Please, provide the path to the folder containing the license header with --license-header=/my/license/header/file.txt'
  exit 1
else
  if [ -f "$license" ]; then
    echo "Valid license header file: ${license} containing:"
    cat ${license}
  else
    echo "Error: License header file ${license} not found. Can not continue."
  exit 1
  fi
fi

if [ "$exclude_folder" == "" ]; then
  echo "No folders will be excluded"
else
  if [ -d "${path}/${exclude_folder}" ]; then
    echo "Valid remove folder path: ${path}/${exclude_folder}"
  fi
fi

if [ $mode == "none" ]; then
  echo "Error: Script should be called with one of the following options: --update or --add"
  exit 1
else
  echo "mode: ${mode}"
fi

# PATH (AND tmp FOLDER)
folder=$path
if [ $tmp == true ]; then
  echo "Creating temporary folder: ${path}_tmp"
  mkdir -pv ${path}_tmp
  cp -a $path/. ${path}_tmp
  folder=${path}_tmp
fi

# LIST OF FILES TO BE ITERATED ON
if [ "$exclude_folder" == "" ]; then
  file_list=$(find $folder -name '*.c' -or -name '*.cpp' -or -name '*.h' -or -name '*.hpp' -or -name '*.h.in' -or -name '*.hpp.in')
else
  file_list=$(find $folder -path ${folder}/${exclude_folder} -prune -name '*.c' -or -name '*.cpp' -or -name '*.h' -or -name '*.hpp' -or -name '*.h.in' -or -name '*.hpp.in')
fi

# DETECT AND REMOVE EXISTING LICENSE
if [ "$mode" == "update" ]; then
  echo "Recursely removing license header from all files (.c, .cpp, .h, .hpp):"
  for i in $file_list
  do
    if grep -Fq "${line_start_mark}" $i 
    then
      echo " - ${i}"
      line_start="$(grep -wn "$line_start_mark" ${i} | head -n 1 | cut -d: -f1)"
      line_end="$(grep -wn "$line_end_mark" ${i} | head -n 1 | cut -d: -f1)"
      # echo ${line_start}
      # echo ${line_end}
      awk -v m=$line_start -v n=$line_end 'm <= NR && NR <= n {next} {print}' $i > tmpfile && mv tmpfile $i 
      #cat $i
    fi
  done
fi

# ADD CONTENT OF license-file AT THE BEGINNING OF CODE FILES
echo "Recursively adding license header to all files (.c, .cpp, .h, .hpp):"
for i in $file_list
do
  if grep -Fq "${line_start_mark}" $i; then
    :
  else
    echo " - ${i}"
    ( cat ${license}; cat $i ) > temp_file
    mv temp_file $i
  fi
done