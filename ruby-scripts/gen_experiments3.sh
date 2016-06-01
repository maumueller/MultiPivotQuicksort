# experiment:
# which is fastest two/three pivot algorithm? 

param[0]="2 0 0 0"
param[1]="2 0 1 1"
param[2]="2 1 1 1"
param[3]="2 1 2 2"
param[4]="2 0 2 2 "
param[5]="3 0 0 0 0"
param[6]="3 0 1 1 0"
param[7]="3 1 1 1 1"
param[8]="3 0 2 2 0"
param[9]="3 1 2 2 1"
param[10]="7 0 0 0 0 0 0 0 0"
param[11]="7 1 1 1 1 1 1 1 1"
param[12]="7 2 2 2 2 2 2 2 2"
param[13]="7 0 1 2 3 3 2 1 0"
param[14]="7 0 0 1 2 2 1 0 0"
param[15]="7 0 0 0 2 2 0 0 0"
param[16]="5 0 0 0 0 0 0"
param[17]="5 0 0 1 1 0 0"
param[18]="5 1 1 1 1 1 1"
param[19]="5 0 1 2 2 1 0"

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
mv src/qstest ../experiments/qs-two
