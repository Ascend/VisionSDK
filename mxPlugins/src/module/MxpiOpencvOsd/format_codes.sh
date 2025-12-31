#!/bin/bash
CLANG_FORMAT=$(which clang-format)
if [[ ${CLANG_FORMAT} = "" ]]; then
  echo "Please install 'clang-format' tool first"
  exit 1
fi

CLANG_FORMAT_VERSION=$("${CLANG_FORMAT}" --version | sed -n "s/.*\ \([0-9]*\)\.[0-9]*\.[0-9]*.*/\1/p")
if [[ "${CLANG_FORMAT_VERSION}" -lt "8" ]]; then
  echo "clang-format's version must be at least 8.0.0"
  exit 1
fi

DOS2UNIX=$(which dos2unix)
if [[ ${DOS2UNIX} = "" ]]; then
  echo "Please install 'dos2unix' tool first"
  exit 1
fi

IGNORE=("build" "output" "treeir" "third_party" ".git" "tests/third_party")

CLANG_FORMAT_EXTENSION=(".c" ".cc" ".cpp" ".h")
DOS2UNIX_EXTENSION=(".c" ".cc" ".cpp" ".cmake" ".h" ".md" ".mlir" ".sh" ".sv" ".td" ".txt" ".v")

BASE_PATH=$(cd $(dirname "$0"); pwd)
ROOT_PATH=$(cd ${BASE_PATH}; pwd)
echo "ROOT_PATH = ${ROOT_PATH}"

# print usage message
function usage() {
  echo "Format the specified source files to conform the code style."
  echo "USAGE:"
  echo "bash $0 [-a] [-c] [-l] [-h]"
  echo "e.g. $0 -c"
  echo ""
  echo "OPTIONS:"
  echo "  -a format of all files"
  echo "  -c format of the files changed compared to last commit (default case)"
  echo "  -l format of the files changed in last commit"
  echo "  -h Print usage"
}

# check and set options
function checkopts() {
  # default check all files
  mode="changed"    

  # Process the options
  while getopts 'aclh' opt
  do
    case "${opt}" in
      a)  mode="all"
          ;;
      c)  mode="changed"
          ;;
      l)  mode="last_commit"
          ;;
      h)  usage
          exit 0
          ;;
      ?)  echo "Unknown option ${opt}!"
          usage
          exit 1
    esac
  done
}

# generate source file list
# $1: SOURCE_FILE_LIST
# $2: SOURCE_FILE_EXTENSION
function gen_source_file_list() {
  regex1=$([[ ${IGNORE} ]] && __=${IGNORE//./\\.}; echo "^${__// /|^}" || echo "^/")
  regex2=$([[ $2 ]] && __=${2//./\\.}; echo "${__// /$|}$" || echo "^/")
  if [[ ${mode} = "all" ]]; then
    find . -type f | cut -c 3- | grep -Ev ${regex1} | grep -E ${regex2} > $1 || true
  elif [[ ${mode} = "changed" ]]; then
    # --diff-filter=ACMRTUXB will ignore deleted files in commit
    git diff --diff-filter=ACMRTUXB --name-only HEAD | grep -Ev ${regex1} | grep -E ${regex2} > $1 || true
  elif [[ ${mode} = "last_commit" ]]; then
    git diff --diff-filter=ACMRTUXB --name-only HEAD~ HEAD | grep -Ev ${regex1} | grep -E ${regex2} > $1 || true
  fi
}

# clang-format
function clang_format() {
  cd ${ROOT_PATH}
  
  local SOURCE_FILE_LIST="__source_file_list__"
  gen_source_file_list ${SOURCE_FILE_LIST} "${CLANG_FORMAT_EXTENSION}"

  while read line; do
    if [[ -f ${line} ]]; then
      ${CLANG_FORMAT} -i ${line}
    fi
  done < ${SOURCE_FILE_LIST}

  rm ${SOURCE_FILE_LIST}
}

# dos2unix
function dos2unix() {
  cd ${ROOT_PATH}

  local SOURCE_FILE_LIST="__source_file_list__"
  gen_source_file_list ${SOURCE_FILE_LIST} "${DOS2UNIX_EXTENSION}"

  while read line; do
    if [[ -f ${line} ]]; then
      ${DOS2UNIX} -q ${line}
    fi
  done < ${SOURCE_FILE_LIST}

  rm ${SOURCE_FILE_LIST}
}

# array -> string
IGNORE=${IGNORE[*]}
CLANG_FORMAT_EXTENSION=${CLANG_FORMAT_EXTENSION[*]}
DOS2UNIX_EXTENSION=${DOS2UNIX_EXTENSION[*]}

# check options
checkopts "$@"

# clang-format
clang_format

# dos2unix
dos2unix

echo "Specified cpp source files have been format successfully."
