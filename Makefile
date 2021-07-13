
CFLAGS=-Wall -Wextra -Wno-unused-parameter
ifeq ($(CC),clang)
  CFLAGS += -Wassign-enum
endif

gpio: ft_gpio.o gpio.o
	echo $(CC)
	$(CC) $^ -o gpio $(CFLAGS)

clean:
	rm -f gpio *.o
