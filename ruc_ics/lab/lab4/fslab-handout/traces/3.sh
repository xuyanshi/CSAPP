cd mnt
touch file
stat file -c %s
truncate file -s 32
stat file -c %s
truncate file -s 10000
stat file -c %s
truncate file -s 24
stat file -c %s
