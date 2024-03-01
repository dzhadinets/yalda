YALDA_BIN=${PWD}/bin/yalda

.PHONY: help
help:
	@echo "The following are valid targets:"
	@echo "  all          - perform all build steps"
	@echo "  install      - modify PATH in the ~/.bashrc"
	@echo "  config       - configure YALDA"
	@echo "  sync         - obtain sources"
	@echo "  build        - build whole dev environment"
	@echo "  clean        - remove ALL"
	@echo "  help         - get this help"

all: .yalda/config sync build

.PHONY: config
config:
	${YALDA_BIN} config

.yalda/config:
	${YALDA_BIN} config

.PHONY: sync
sync:
	${YALDA_BIN} sync

.PHONY: build
build:
	${YALDA_BIN} build

.PHONY: install
install:
	@grep -q "source ${PWD}/scripts/profile.bash" ~/.bashrc \
	    || echo source ${PWD}/scripts/profile.bash >> ~/.bashrc

.PHONY: clean
clean:
	@echo rm -rf ${PWD}/.yalda
