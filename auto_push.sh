#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Commit message required"
else
	git add -A .
	git commit -m "$1"
	git push
fi
