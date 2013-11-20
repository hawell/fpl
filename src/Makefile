NAME = fpl

MAJOR = 0
MINOR = 0
PATCH = 0

VERSION = $(MAJOR).$(MINOR).$(PATCH)

STATICTARGET = $(NAME)-$(VERSION).a
DYNAMICTARGET = lib$(NAME)-$(VERSION).so

CFLAGS = -std=c99 -ggdb3 -W -Wall -fPIC

SRCS = clahe.c config.c convolution.c fingerprint.c image.c lro.c mathfuncs.c process.c 
HDRS = $(SRCS:.c=.h)
OBJS = $(SRCS:.c=.o)

all: static dynamic

static: $(OBJS)
	ar rcf $(STATICTARGET) $(OBJS)

dynamic: $(OBJS)
	gcc -shared -Wl,--export -o $(DYNAMICTARGET) $(OBJS)

.c.o: $(HDRS)
	gcc $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -rf *.so *.a $(OBJS)
