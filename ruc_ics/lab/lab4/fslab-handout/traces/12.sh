cd mnt
touch file1
stat file1 | sed -n '1,2p;4p'
echo 23333 > file1
stat file1 | sed -n '1,2p;4p'
