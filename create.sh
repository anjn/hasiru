#!/usr/bin/env bash

if [ $# -lt 1 ] ; then
  echo "usage: $0 EDP_DIRECTORY"
  exit 1
fi

edp=$1

if [ ! -d $edp ] ; then
  echo "error: directory $edp doesn't exsit!"
  exit 1
fi

mkdir -p dat

cat $edp/EIJIRO/EIJI-*.TXT \
  | nkf --ic=Shift_JIS --oc=UTF-8 \
  | bin/create

