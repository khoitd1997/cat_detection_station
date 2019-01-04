ccflags-y := -std=gnu99 -g -Wall -Wno-declaration-after-statement

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean