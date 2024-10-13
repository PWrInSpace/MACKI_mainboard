#!/bin/bash

FILES="\
src/main/main.c
src/components
"

EXCLUDE="\
src/components/tmc2209_arduino
"

OPTIONS="\
--recursive
"

FILTERS="\
-build/include_subdir,\
-build/header_guard,\
-readability/casting,\
-whitespace/braces,\
-readability/multiline_string
"

cpplint --exclude=${EXCLUDE} ${OPTIONS} --linelength=80 --filter=${FILTERS} ${FILES}