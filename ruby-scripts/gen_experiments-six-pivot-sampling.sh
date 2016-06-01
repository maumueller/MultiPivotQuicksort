# experiment:
# which is fastest two/three pivot algorithm? 

param[0]="6 0 0 0 0 0 0 0"
param[1]="6 0 0 0 1 1 0 0"
param[2]="6 1 1 1 1 1 1 1"
param[3]="6 0 0 0 2 2 0 0"
param[4]="6 0 0 1 2 2 1 0 "
param[5]="6 2 2 2 2 2 2 2 "
param[6]="6 0 0 1 3 3 1 0 "
param[7]="6 0 0 1 4 4 1 0"
param[8]="6 0 1 1 5 5 1 1"
param[9]="6 0 1 1 3 3 1 1"

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
mv src/qstest ../experiments/qs-six-sample
