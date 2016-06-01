# experiment:

param[0]="1 0 0"
param[1]="2 0 0 0"
param[2]="3 0 0 0 0"
param[3]="4 0 0 0 0 0"
param[4]="5 0 0 0 0 0 0"
param[5]="6 0 0 0 0 0 0 0"
param[6]="7 0 0 0 0 0 0 0 0"
param[7]="8 0 0 0 0 0 0 0 0 0"
param[8]="9 0 0 0 0 0 0 0 0 0 0"
param[9]="15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"

rm ../src/algorithms/included_algorithms.h

echo \#include \"qsort1is.h\" >> ../src/algorithms/included_algorithms.h;
echo \#include \"qsort2v5.h\" >> ../src/algorithms/included_algorithms.h;
echo \#include \"qsort3.h\" >> ../src/algorithms/included_algorithms.h;

for i in "${param[@]}";
do
    FILENAME=qs-${i}.h;
    echo Generating ${FILENAME};
    ruby gen_k_pivot_algorithm.rb $i > "../src/algorithms/$FILENAME";
    echo \#include \"$FILENAME\" >> ../src/algorithms/included_algorithms.h;
done

