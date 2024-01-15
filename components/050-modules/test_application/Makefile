APP_CC		= $(CROSS_COMPILE)gcc
APP_LD		= $(CROSS_COMPILE)ld

APP_WARNFLAGS=-Wall -Wno-unused-function -Wno-unused-variable
APP_CFLAGS+= $(call cc-option,-O3,-O1) ${APP_WARNFLAGS} -g -DDEBUG

### !!! VERY IMPORTANT !!!
### YALDA does not support dynamic linkage. Always use this flag
APP_CFLAGS+= -static

APP_TARGET=yalda_app
APP_SRCS=test_app.c
APP_OBJS=$(APP_SRCS:.c=.o)

test_make: $(APP_OBJS)
	$(APP_CC) $(APP_CFLAGS) -o $(APP_TARGET) $(APP_OBJS)

.c.o:
	$(APP_CC) $(APP_CFLAGS) -c $< -o $@

test_clean:
	rm -f $(APP_TARGET) $(APP_OBJS)

.PHONY: all clean install uninstall

all: test_make

clean: test_clean

install:
	cp "$(APP_TARGET)" "$(INSTALL_MOD_PATH)/usr/bin/"

uninstall:
	rm -f "$(INSTALL_MOD_PATH)/usr/bin/$(APP_TARGET)"
