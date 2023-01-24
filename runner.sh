#!/bin/bash

proj="proj"$1
main=$proj"_main"
img=$proj"_out.pnm"
out=$proj"_out.png"
check=$proj"_checker"
sol=$proj"_soln.pnm"

echo $proj
echo $main
echo $img
echo $out
echo $check
echo $sol

make clean &&

make $proj &&

./$main &&

pnmtopng < $img > $img &&

./$check &&

./diff $img $sol
