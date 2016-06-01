# experiment:
# which is fastest two/three pivot algorithm? 

param[0]="1 0 0"
param[1]="1 1 1"
param[2]="1 0 1"
param[3]="1 2 2"
param[4]="1 1 2 "
param[5]="1 3 3 "

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
mv src/qstest ../experiments/qs-one-sample
