# experiment:
# which is fastest two/three pivot algorithm? 

param[0]="2 0 0 0"
param[1]="2 0 1 1"
param[2]="2 1 1 1"
param[3]="2 1 2 2"
param[4]="2 0 2 2 "
param[5]="2 2 2 2 "
param[6]="2 1 3 3 "
param[7]="2 1 4 4"
param[8]="2 1 5 5"
param[9]="2 2 5 5"

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
mv src/qstest ../experiments/qs-two-sample
