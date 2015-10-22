#!/usr/bin/env bash

basedir="../.."
endl=''
if [ "$1" == "qt" ]; then
  file=Build/qt/FileList.pri
  src='lib_SRC_FILES = \'
  hdr='lib_HDR_FILES = \'
  masterpubhdr='MASTER_lib_PUB_HDR_FILES = \'
  pubhdr='lib_PUB_HDR_FILES = \'
  ends='\n'
  endl='\'
else
  file=Build/cmake/FileList.cmake
  src='set(lib_SRC_FILES'
  hdr='set(lib_HDR_FILES'
  masterpubhdr='set(MASTER_lib_PUB_HDR_FILES'
  pubhdr='set(lib_PUB_HDR_FILES'
  ends=')\n'
fi
if [ "$1" == "qt" ]; then
  srcdir='$$PROJECT_SOURCE_DIR'
else
  srcdir='${PROJECT_SOURCE_DIR}'
fi
srcpath=Source
hdrpath=Include/Rocket
pypath=Python
luapath=Lua

printfiles() {
    dr=$(echo $1 | tr ";" "\n")
    # Print headers
    main=''; for x in $dr; do
	  if [ ".$main" == "." ]; then
	    echo ${hdr/lib/$x} >>$file
	    find  $srcpath/$x -maxdepth 1 -iname "*.h" -exec echo '    '$srcdir/{} $endl \; >>$file
	    main=$x
	  else
	    find  $srcpath/$main/$x -maxdepth 1 -iname "*.h" -exec echo '    '$srcdir/{} $endl \; >>$file
	  fi
    done
    echo -e $ends >>$file
    # Print master header for library
    echo ${masterpubhdr/lib/$main} >>$file
    echo '    '$srcdir/Include/Rocket/$main.h >>$file
    echo -e $ends >>$file
    # Print public headers sub directory
    main=''; for x in $dr; do
	  if [ ".$main" == "." ]; then
	    echo ${pubhdr/lib/$x} >>$file
	    find  $hdrpath/$x -maxdepth 1 -iname "*.h" -exec echo '    '$srcdir/{} $endl \; >>$file
	    main=$x
	  else
	    find  $hdrpath/$main/$x -maxdepth 1 -iname "*.h" -exec echo '    '$srcdir/{} $endl \; >>$file
	  fi
    done
    echo -e $ends >>$file
    # Print source files
    main=''; for x in $dr; do
	  if [ ".$main" == "." ]; then
	    echo ${src/lib/$x} >>$file
	    find  $srcpath/$x -maxdepth 1 -iname "*.cpp" -exec echo '    '$srcdir/{} $endl \; >>$file
	    main=$x
	  else
	    find  $srcpath/$main/$x -maxdepth 1 -iname "*.cpp" -exec echo '    '$srcdir/{} $endl \; >>$file
	  fi
    done
    echo -e $ends >>$file
}

printpyfiles() {
    # Print headers
    echo ${hdr/lib/Py${1}} | sed 's/PyCo/Pyco/' >>$file
    find  $srcpath/$1/$pypath -iname "*.h" -exec echo '    '$srcdir/{} $endl \; 2>/dev/null | sort -f >>$file
    echo -e $ends >>$file
    # Print public headers
    echo ${pubhdr/lib/Py${1}} | sed 's/PyCo/Pyco/' >>$file
    find  $hdrpath/$1/$pypath -iname "*.h" -exec echo '    '$srcdir/{} $endl \; 2>/dev/null | sort -f >>$file 2>/dev/null
    echo -e $ends >>$file
    # Print source files
    echo ${src/lib/Py${1}} | sed 's/PyCo/Pyco/' >>$file
    find  $srcpath/$1/$pypath -iname "*.cpp" -exec echo '    '$srcdir/{} $endl \; 2>/dev/null | sort -f >>$file
    echo -e $ends >>$file
}

printluafiles() {
    # Print headers
    echo ${hdr/lib/Lua${1}} >>$file
    find  $srcpath/$1/$luapath -iname "*.h" -exec echo '    '$srcdir/{} $endl \; 2>/dev/null | sort -f >>$file
    echo -e $ends >>$file
    # Print public headers
    echo ${pubhdr/lib/Lua${1}} >>$file
    find  $hdrpath/$1/$luapath -iname "*.h" -exec echo '    '$srcdir/{} $endl \; 2>/dev/null | sort -f >>$file 2>/dev/null
    echo -e $ends >>$file
    # Print source files
    echo ${src/lib/Lua${1}} >>$file
    find  $srcpath/$1/$luapath -iname "*.cpp" -exec echo '    '$srcdir/{} $endl \; 2>/dev/null | sort -f >>$file
    echo -e $ends >>$file
}

pushd $basedir
echo -e "# This file was auto-generated with gen_filelists.sh\n" >$file
echo -e "isEmpty(PROJECT_SOURCE_DIR):PROJECT_SOURCE_DIR = ../../..\n" >>$file
for lib in "Core;BitmapFont;FreeType" "Controls" "Debugger" "Ext"; do
    printfiles $lib
done

for lib in "Core" "Controls"; do
    printpyfiles $lib
done
for lib in "Core" "Controls"; do
    printluafiles $lib
done

echo -e "" >>$file
echo -e "INCLUDEPATH += $$PROJECT_SOURCE_DIR/Include" >>$file
echo -e "LIBS += -lm" >>$file

popd
