#!/bin/bash
for f in "topol.h" "elements.h" "strFuncts.h" \
         "debug.h" "scene.h" "palettes.h"
do echo $f
if [[ -e ../../$f ]]
then
    if [[ -e $f ]]
    then
        echo "Target exists. Deleting existing link."
        rm $f
        echo "Creating new link."
        ln ../../$f $f
    fi
else
echo "Target does not exist. Doing nothing."
fi

done

