#!/bin/bash

cd Docs/doxygen

# if no flag is given, build locally
if [ $# -eq 0 ]
then
    echo "Building locally"
    doxygen doxygen-local.config
    exit 0
fi
if [ $1 = "github" ]
then
    echo "Building for github pages"
    doxygen doxygen-github.config
    exit 0
fi
