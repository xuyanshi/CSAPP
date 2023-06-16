cd mnt
cp ../fs.c fs.c
cp fs.c ../fs-test.c
cd ..
diff fs.c fs-test.c
