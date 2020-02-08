all:
	gcc-9 --static-pie -ggdb -T ./static-pie.ld -o test test.c
	python strip_headers.py ./test 
	echo "_Alignas(char) const " >> ./include/embedded.h
	xxd -i test >> ./include/embedded.h
	gcc-9 -Os -T static-pie.ld -static-pie static-pie-loader.c -o loader\
                -Wl,--gc-sections -Wl,-z,max-page-size=1 -Wl,-z,common-page-size=1\
                -mmanual-endbr -nostartfiles  -nodefaultlibs  -nolibc  -nostdlib\
                -fdata-sections -ffunction-sections
clean:
	rm test loader 
