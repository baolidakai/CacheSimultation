CC=g++ -std=c++11

SRCS=cache.cc driver.cc

OBJS=$(SRCS:.cc=.o)

EXEC=driver

start:$(OBJS)
	$(CC) -o $(EXEC) $(OBJS)
.cc.o:
	$(CC) -o $@ -c $<
clean:
	rm -rf $(OBJS)
