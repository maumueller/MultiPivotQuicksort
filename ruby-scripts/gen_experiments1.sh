# experiment:
# can five pivot quicksort beat three pivot quicksort?

param[7]="2 0 0 0"
param[8]="2 0 1 1"
param[9]="2 0 2 2"
param[0]="3 0 0 0 0"
param[6]="3 0 1 1 0"
param[1]="3 1 2 2 1"
param[2]="5 0 0 0 0 0 0"
param[3]="5 0 1 2 2 1 0"
param[4]="5 0 0 1 1 0 0"
param[5]="5 0 2 3 3 2 0"
param[10]="7 0 0 0 0 0 0 0 0"
param[11]="7 1 1 1 1 1 1 1 1"
param[12]="7 2 2 2 2 2 2 2 2"
param[13]="7 0 1 2 3 3 2 1 0"
param[14]="7 0 0 1 2 2 1 0 0"
param[15]="7 0 0 0 2 2 0 0 0"
param[16]="9 0 0 0 0 0 0 0 0 0 0"
param[17]="9 0 0 0 1 3 3 1 0 0 0"
param[18]="9 0 0 1 2 4 4 2 1 0 0"
param[19]="9 1 1 1 1 1 1 1 1 1 1"
param[20]="9 0 0 0 0 2 2 0 0 0 0"
param[21]="9 0 0 0 1 1 1 1 0 0 0"

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
mv src/qstest ../experiments/qs-three-five
