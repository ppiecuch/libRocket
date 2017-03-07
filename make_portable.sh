#!/bin/bash

function merge {

    in=$1

    echo "/** $0: `date`*/" > "$C"
    echo "#define _BUILD_SINGLE_SOURCE" >> "$C"
    echo "" >> "$C"
    echo "/** $0: `date`*/" > "$H"
    echo "" >> "$H"

    line_offset=3

    for c in $theoraplayer_c; do
	if [ -e "$c" ]; then
	    echo "/** $0: $c*/" >> "$C"
	    echo "" >> "$C"
	    #line=`cat "$C"|wc -l`
	    echo "#line $line_offset \"$c\"" >> "$C"
	    cat $c \
		| sed "s/\([^_]\)ilog/\1ilog${cnt}/g" \
		| sed "s/FLOOR1_fromdB_LOOKUP/FLOOR1_fromdB_LOOKUP${cnt}/g" \
		| sed "s/bitreverse/bitreverse${cnt}/g" \
		>> "$C"
	    echo "" >> "$C"
	    ((cnt++))
	else
	    # use '' for space
	    echo "" >> "$C"
	    echo "${c//\'\'/ }" >> "$C"
	    echo "" >> "$C"
	fi
    done
}

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

for i in vorbis/lib/modes/*.h; do
    inl=`basename "$i"`
    sed -i "" -e "/#include \"modes\/$inl\"/r $i" -e "/#include \"modes\/$inl\"/d" "$C"
done

echo "** Done (with $cnt files)"
