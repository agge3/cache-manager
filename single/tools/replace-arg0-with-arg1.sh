#!/usr/bin/env bash

# Set project root to one directory back (ran from /tools).
root="../"

# Check if the correct number of arguments is provided.
if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <search_pattern> <replace_pattern> <file>"
  exit 1
fi

# Assign arguments to variables for better readability.
search_pattern="${1}"	# The pattern to search for in the file.
replace_pattern="${2}"	# The text to replace the search pattern with.
file="${root}${3}"		# The file in which to perform the replacement.

# sed performs the replacement.
sed -i "s/${search_pattern}/${replace_pattern}/g" "${file}"

# Print a success message.
echo "Replaced all occurrences of ${search_pattern} with ${replace_pattern} in ${file}."
