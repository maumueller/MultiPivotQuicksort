# experiment:
# which is fastest two/three pivot algorithm? 

param[0]="4 0 0 0 0 0"
param[1]="4 0 0 1 1 0"
param[2]="4 1 1 1 1 1"
param[3]="4 0 0 2 2 0"
param[4]="4 2 2 2 2 2 "
param[5]="4 0 1 3 3 1 "
param[6]="4 0 1 4 4 1 "
param[7]="4 1 2 4 4 2"
param[8]="4 0 1 5 5 1"
param[9]="4 0 0 3 3 0"

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

cd ../bin
make
mv src/qstest ../experiments/qs-four-sample
