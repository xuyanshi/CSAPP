cd mnt
touch file1 -t 201212210000.36
stat file1 -c %x%y
touch file2 -r file1
stat file2 -c %x%y
