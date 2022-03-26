#!/bin/bash

cd demo
for i in *; do
  ../tml "$i" 2>&1 > /dev/null
  out=$?
  if [[ "$i" == *"invalid"* ]] && [[ $out -ne 0 ]]; then
    echo "Pass: $i"
  elif [[ "$i" != *"invalid"* ]] && [[ $out -eq 0 ]]; then
    echo "Pass: $i"
  else
    echo "FAIL: $i"
  fi
done

rm -f app.c tml_events.h
