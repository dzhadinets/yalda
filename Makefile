YALDA_BIN=${PWD}/bin/yalda

.PHONY: all
all:
	${YALDA_BIN} config
	${YALDA_BIN} sync
	${YALDA_BIN} build

.PHONY: help
help:
	@echo "The following are valid targets:"
	@echo "  all          - perform all build steps"
	@echo "  install      - modify PATH in the ~/.bashrc"
	@echo "  clean        - remove ALL"
	@echo "  help         - get this help"

.PHONY: install
install:
	@grep -q "source ${PWD}/scripts/profile.bash" ~/.bashrc \
	    || echo source ${PWD}/scripts/profile.bash >> ~/.bashrc

.PHONY: clean
clean:
	@echo rm -rf ${PWD}/.yalda

