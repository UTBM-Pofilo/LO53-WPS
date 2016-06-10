PREFIX=/home/lo53/staging_dir
export STAGING_DIR=/home/lo53/staging_dir
GCC=$(PREFIX)/toolchain/bin/mips-openwrt-linux-gcc
LD=$(PREFIX)/toolchain/bin/mips-openwrt-linux-ld
ALLFLAGS=-Wall -Werror -O2 -I$(PREFIX)/toolchain/include/ -I$(PREFIX)/target/usr/include/ -L$(PREFIX)/toolchain/lib/ -L$(PREFIX)/target/usr/lib -lpcap -lm -pthread
GCCFLAGS=-Wall -Werror -O2 -I$(PREFIX)/toolchain/include/ -I$(PREFIX)/target/usr/include/
LDFLAGS=-L$(PREFIX)/toolchain/lib/ -L$(PREFIX)/target/usr/lib/ -lpcap -lm -pthread 

rssi: rssi_list.c
	$(GCC) $(ALLFLAGS) rssi_list.c http-test.c -o tests

%.o: %.c %.h
	$(GCC) $(GCCFLAGS) -o $@ -c $< 

clean:
	rm -f *.o
	rm -f rssi_list

backupclean:
	rm -f *~
