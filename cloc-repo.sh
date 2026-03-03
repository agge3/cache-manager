#!/usr/bin/env bash

# list of files/directories to cloc
INCLUDE=(
	build.sh
	cloc-repo.sh
	CMakeLists.txt
	config
	Dockerfile
	Dockerfile.build
	include
	Makefile
	README.md
	run-clang-format.sh
	.clang-format
	.gitignore
	run-podman.sh
	run.sh
	src
	test-runner.hpp
)

mapfile -t DIRS < <(find . -mindepth 1 -maxdepth 1 -printf "%P\n")
EXCLUDE=($(printf "%s\n" "${DIRS[@]}" | grep -Fxv -f <(printf "%s\n" "${INCLUDE[@]}")))

# flatten into comma-separated list
printf -v EXCLUDE_LIST "%s," "${EXCLUDE[@]}"
EXCLUDE_LIST=${EXCLUDE_LIST%,}

echo "DEBUG: EXCLUDE_LIST: $EXCLUDE_LIST"

# NOTE: --by-file to see counted files
cloc . --exclude_dir="$EXCLUDE_LIST"
