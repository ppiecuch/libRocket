#!/bin/bash

mkdir -p Portable
# add headers
cp -r Include/Rocket Portable/

files=0
cpp_files=""

for dir in Controls Core Debugger Ext; do
  echo ""
done

for c in $cpp_files; do
    echo "/** $0: $c*/" >> "$C"
    echo "" >> "$C"
    echo "#line 0 \"$c\"" >> "$C"
    cat $c >> "$C"
    echo "" >> "$C"
    ((cnt++))
done

echo "** Done (with $cnt files)"
