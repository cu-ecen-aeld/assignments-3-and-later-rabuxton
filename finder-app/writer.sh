#!/bin/bash

# Check the number of arguments
if [ "$#" -ne 2 ]; then
    echo "Illegal number of arguments."
	exit 1
fi

# Create the directory if it does not exist
if [ ! -d "$1" ]; then
	mkdir -p "$(dirname "$1")"
fi

echo $2 >> "$1"

if [ $? -ne 0 ]; then
	echo "Unable to write to file."
	exit 1
fi
