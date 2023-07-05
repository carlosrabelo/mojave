MAKEFLAGS += --no-print-directory

.DEFAULT_GOAL := help

.PHONY: build-tty build-sdl build-qt6 build-all check clean coverage fmt help install-tty install-sdl install-qt6 install-all lint quality run-tty run-sdl run-qt6 setup test-all test-changed test-fast uninstall-tty uninstall-sdl uninstall-qt6 uninstall-all

help: ## Show available targets
	@echo "mojave - Available targets"
	@echo ""
	@grep -hE '^[a-zA-Z0-9_-]+:.*## ' $(MAKEFILE_LIST) \
		| sort \
		| awk 'BEGIN {FS = ":.*## "} {printf "  %-15s %s\n", $$1, $$2}'

setup: ## Download dependencies
	@../.make/setup.sh

build-tty: ## Compile mojave binary
	@../.make/build-tty.sh

build-sdl: ## Compile mojave-sdl binary (requires libsdl2-dev)
	@../.make/build-sdl.sh

build-qt6: ## Compile mojave-qt6 binary (requires qt6)
	@../.make/build-qt6.sh

build-all: build-tty build-sdl build-qt6 ## Compile all binaries

install-tty: build-tty ## Install mojave
	@BINARY_NAME=mojave PREFIX="$(PREFIX)" ../.make/install.sh

install-sdl: build-sdl ## Install mojave-sdl
	@BINARY_NAME=mojave-sdl PREFIX="$(PREFIX)" ../.make/install.sh

install-qt6: build-qt6 ## Install mojave-qt6
	@BINARY_NAME=mojave-qt6 PREFIX="$(PREFIX)" ../.make/install.sh

install-all: install-tty install-sdl install-qt6 ## Install all binaries

uninstall-tty: ## Remove mojave
	@BINARY_NAME=mojave PREFIX="$(PREFIX)" ../.make/uninstall.sh

uninstall-sdl: ## Remove mojave-sdl
	@BINARY_NAME=mojave-sdl PREFIX="$(PREFIX)" ../.make/uninstall.sh

uninstall-qt6: ## Remove mojave-qt6
	@BINARY_NAME=mojave-qt6 PREFIX="$(PREFIX)" ../.make/uninstall.sh

uninstall-all: uninstall-tty uninstall-sdl uninstall-qt6 ## Remove all binaries

run-tty: build-tty ## Run mojave
	@BINARY_NAME=mojave ../.make/run.sh $(ARGS)

run-sdl: build-sdl ## Run mojave-sdl
	@BINARY_NAME=mojave-sdl ../.make/run.sh $(ARGS)

run-qt6: build-qt6 ## Run mojave-qt6
	@BINARY_NAME=mojave-qt6 ../.make/run.sh $(ARGS)

test-all: ## Compile and run all tests
	@../.make/test-all.sh

test-fast: ## Compile and run only [fast] tests
	@../.make/test-fast.sh

test-changed: ## Incremental build; run tests for changed mojave/ paths
	@../.make/test-changed.sh

check: ## Syntax-check all source files
	@../.make/check.sh

coverage: ## Build and run tests with coverage report (gcovr)
	@../.make/coverage.sh

quality: ## All quality checks
	@../.make/quality.sh

fmt: ## Format source
	@../.make/fmt.sh

lint: ## Run cppcheck
	@../.make/lint.sh

clean: ## Remove build artifacts
	@rm -rf ../build/bin ../build/obj ../build/coverage 2>/dev/null; mkdir -p ../build/bin; echo "Cleaned"
