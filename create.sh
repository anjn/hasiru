#!/usr/bin/env bash

cat ../EDP-133/EIJIRO/EIJI-133.TXT \
  | nkf --ic=Shift_JIS --oc=UTF-8 \
  | ./create

#head -n 100000 ../EDP-133/EIJIRO/EIJI-133.TXT \
#  | nkf --ic=Shift_JIS --oc=UTF-8 \
#  | ./create

