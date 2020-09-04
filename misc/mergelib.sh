#!/bin/sh

# merges two libraries - written by Christian Zietz - placed into public domain

# print usage
if [ $# -ne 2 ]; then
	echo "Usage: $0 tolib fromlib" 1>&2
	exit 1
fi

tmpdir=tmp.$$

# delete temp dir if program is aborted

trap "rm -rf $tmpdir; exit 1" 1 2 15

# fail if temp dir exists

if [ -d $tmpdir ]; then
	echo "$tmpdir already exists" 1>&2
	exit 1
fi

# make temp dir and check for success

mkdir $tmpdir

if [ ! -d $tmpdir ]; then
	echo "unable to create $tmpdir" 1>&2
	exit 1
fi

# check whether paths are absolute or relative

case "$2" in
	/?*) fromlib=$2 ;;
	*) fromlib=../$2 ;;
esac

case "$1" in
	/?*) tolib=$1 ;;
	*) tolib=../$1 ;;
esac

# extract an rename files

cd $tmpdir || exit 1;
ar x $fromlib
for i in *.o; do
	mv $i _$i
done

# merge in

ar clq $tolib *.o
ranlib $tolib
cd ..
rm -rf $tmpdir
exit 0