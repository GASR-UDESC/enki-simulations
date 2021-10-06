#!/bin/bash
mkdir results/enki_grouping_ga
./build/projects/enki_grouping_ga/enki_grouping_ga

find . -maxdepth 1 \( -iname \*.ppm \) -exec convert -verbose "{}" "{}.png" \;
