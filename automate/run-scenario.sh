#!/bin/bash
cd ../

# Small muffer
seed=2
target=$(($seed+5))
while [ $seed -le $target ]
do
  ./waf --run "scratch/test-mmw --ccProt=TcpBbr --seed=$seed --lteBuff=2000000" 
  cd scratch
  python processdata.py
  cd ..
  mv traces/test traces/2e6_tcp_bbr_$seed

  ./waf --run "scratch/test-mmw --ccProt=TcpSiad --seed=$seed --lteBuff=2000000" 
  cd scratch
  python processdata.py
  cd ..
  mv traces/test traces/2e6_tcp_siad_$seed
  echo "seed=$seed run finished"
  seed=$(($seed+1))
done

# Medium buffer
seed=2
target=$(($seed+5))
while [ $seed -le $target ]
do
  ./waf --run "scratch/test-mmw --ccProt=TcpBbr --seed=$seed --lteBuff=7000000" 
  cd scratch
  python processdata.py
  cd ..
  mv traces/test traces/7e6_tcp_bbr_$seed

  ./waf --run "scratch/test-mmw --ccProt=TcpSiad --seed=$seed --lteBuff=7000000" 
  cd scratch
  python processdata.py
  cd ..
  mv traces/test traces/7e6_tcp_siad_$seed
  echo "seed=$seed run finished"
  seed=$(($seed+1))
done

# Large buffer
seed=2
target=$(($seed+5))
while [ $seed -le $target ]
do
  ./waf --run "scratch/test-mmw --ccProt=TcpBbr --seed=$seed --lteBuff=20000000" 
  cd scratch
  python processdata.py
  cd ..
  mv traces/test traces/20e6_tcp_bbr_$seed

  ./waf --run "scratch/test-mmw --ccProt=TcpSiad --seed=$seed --lteBuff=20000000" 
  cd scratch
  python processdata.py
  cd ..
  mv traces/test traces/20e6_tcp_siad_$seed
  echo "seed=$seed run finished"
  seed=$(($seed+1))
done
