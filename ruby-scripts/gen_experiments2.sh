# experiment:
# which is fastest seven pivot quicksort?

param[0]="9 0 0 0 0 0 0 0 0 0 0"
param[1]="9 0 0 0 1 3 3 1 0 0 0"
param[2]="9 0 0 1 2 4 4 2 1 0 0"
param[3]="9 1 1 1 1 1 1 1 1 1 1"
param[4]="9 0 0 0 0 2 2 0 0 0 0"
param[5]="9 0 0 0 1 1 1 1 0 0 0"

rm ../src/algorithms/included_algorithms.h

for i in "${param[@]}";
do
    FILENAME=qs-${i}.h;
    echo Generating ${FILENAME};
    ruby gen_k_pivot_algorithm.rb $i > "../src/algorithms/$FILENAME";
    echo \#include \"$FILENAME\" >> ../src/algorithms/included_algorithms.h;
done

cd ../bin
make
mv src/qstest ../experiments/qs-nine
