#!/bin/sh

# Check the number of arguments
if [ "$#" -ne 2 ]; then
    echo "Illegal number of arguments."
	exit 1
fi

# Check if the directory exists
if [ ! -d "$1" ]; then
	echo "The directory does not exist."
	exit 1
fi

files=$(find "$1" -type f)
numFiles=$(echo $files | wc -w)

matchingLines=$(cat $files | grep -c $2) 

echo "The number of files are $numFiles and the number of matching lines are $matchingLines."
