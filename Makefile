CC	=  gcc
CFLAGS	=  -g -O2 -Wall -W
LDLIBS  = 
FILES	=	slb2unpack
COMMON	=	
DEPS	=	Makefile

OBJS	= $(COMMON) $(addsuffix .o, $(FILES))

all: $(FILES)

$(FILES): %: %.o $(COMMON) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LDLIBS) 

$(OBJS): %.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f $(OBJS) $(FILES) *.exe *~
