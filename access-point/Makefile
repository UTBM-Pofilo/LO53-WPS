PREFIX=/home/lo53/staging_dir
export STAGING_DIR=/home/lo53/staging_dir
GCC=$(PREFIX)/toolchain/bin/mips-openwrt-linux-gcc
LD=$(PREFIX)/toolchain/bin/mips-openwrt-linux-ld
ALLFLAGS=-Wall -Werror -O2 -I$(PREFIX)/toolchain/include/ -I$(PREFIX)/target/usr/include/ -L$(PREFIX)/toolchain/lib/ -L$(PREFIX)/target/usr/lib -lpcap -lm -pthread
GCCFLAGS=-Wall -Werror -O2 -I$(PREFIX)/toolchain/include/ -I$(PREFIX)/target/usr/include/
LDFLAGS=-L$(PREFIX)/toolchain/lib/ -L$(PREFIX)/target/usr/lib/ -lpcap -lm -pthread 

default: app.c
	$(GCC) $(ALLFLAGS) pcap-thread.c http.c util.c app.c -o LO53-Group1

# make ID=2 IFACE=wlan0 param
param: app.c
	$(GCC) $(ALLFLAGS) -DAP_ID=${ID} -DIFACE=\"${IFACE}\" pcap-thread.c http.c util.c app.c -o LO53-Group1

%.o: %.c %.h
	$(GCC) $(GCCFLAGS) -o $@ -c $< 

clean:
	rm -f *.o
	rm -f LO53-Group1

backupclean:
	rm -f *~
