cd mnt
for ((i=0;i<1024;++i)); do
	mkdir "dir$i"
	touch "file$i"
	done
ls
