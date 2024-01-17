APP_CC = $(CROSS_COMPILE)gcc
APP_LD = $(CROSS_COMPILE)ld

ifeq ($(BUILD_DIR),)
BUILD_DIR := $(PWD)
else
BUILD_DIR := $(shell realpath $(BUILD_DIR))
$(info Custom build dir $(BUILD_DIR))
endif

APP_WARNFLAGS=-Wall -Wno-unused-function -Wno-unused-variable
APP_CFLAGS+= $(call cc-option,-O3,-O1) ${APP_WARNFLAGS} -g -DDEBUG

### !!! VERY IMPORTANT !!!
### YALDA does not support dynamic linkage. Always use this flag
APP_CFLAGS+= -static

APP_TARGET=yalda_app
APP_SRCS=test_app.c fn.c
APP_OBJS=$(APP_SRCS:.c=.o)

.c.o:
	@$(APP_CC) $(APP_CFLAGS) -c $< -o $(BUILD_DIR)/$@

all: $(APP_OBJS)
	@cd $(BUILD_DIR); $(APP_CC) $(APP_CFLAGS) -o $(APP_TARGET) $(APP_OBJS)

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: install
install:
	@cd $(BUILD_DIR); cp "$(APP_TARGET)" "$(INSTALL_PATH)/usr/bin/"
