obj-m :=char_driver.o

KERN_DIR=/lib/modules/$(shell uname -r)/build/
KERN_DIR_ARM=/lib/modules/5.15.84-v7l+/build/

default:
	$(MAKE) -C $(KERN_DIR) M=$(shell pwd) modules
	gcc -o write write.c 
	gcc -o read read.c 


clean:
	$(MAKE) -C $(KERN_DIR) M=$(shell pwd) clean
	rm read write 