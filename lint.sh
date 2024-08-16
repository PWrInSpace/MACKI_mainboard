#!/bin/bash

FILES="\
src/main/main.c
src/components
"

OPTIONS="\
--recursive
"

FILTERS="\
-build/include_subdir,\
-build/header_guard,\
-readability/casting,\
-whitespace/braces,\
-readability/multiline_string,\
-runtime/printf\
"

cpplint ${OPTIONS} --linelength=80 --filter=${FILTERS} ${FILES}