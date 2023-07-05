MAKEFLAGS += --no-print-directory

.DEFAULT_GOAL := help

.PHONY: build-tty build-sdl build-qt6 build-all check clean coverage fmt help install-tty install-sdl install-qt6 install-all lint quality run-tty run-sdl run-qt6 setup test-all test-changed test-fast uninstall-tty uninstall-sdl uninstall-qt6 uninstall-all

help: ## Show available targets
	@$(MAKE) -C mojave help

setup: ## Download dependencies (e.g. Catch2 header)
	@$(MAKE) -C mojave setup

build-tty: ## Compile mojave binary
	@$(MAKE) -C mojave build-tty

build-sdl: ## Compile mojave-sdl binary (requires libsdl2-dev)
	@$(MAKE) -C mojave build-sdl

build-qt6: ## Compile mojave-qt6 binary (requires qt6)
	@$(MAKE) -C mojave build-qt6

build-all: ## Compile all binaries
	@$(MAKE) -C mojave build-all

install-tty: ## Install mojave
	@$(MAKE) -C mojave install-tty

install-sdl: ## Install mojave-sdl
	@$(MAKE) -C mojave install-sdl

install-qt6: ## Install mojave-qt6
	@$(MAKE) -C mojave install-qt6

install-all: ## Install all binaries
	@$(MAKE) -C mojave install-all

uninstall-tty: ## Remove mojave
	@$(MAKE) -C mojave uninstall-tty

uninstall-sdl: ## Remove mojave-sdl
	@$(MAKE) -C mojave uninstall-sdl

uninstall-qt6: ## Remove mojave-qt6
	@$(MAKE) -C mojave uninstall-qt6

uninstall-all: ## Remove all binaries
	@$(MAKE) -C mojave uninstall-all

run-tty: ## Run mojave
	@$(MAKE) -C mojave run-tty

run-sdl: ## Run mojave-sdl
	@$(MAKE) -C mojave run-sdl

run-qt6: ## Run mojave-qt6
	@$(MAKE) -C mojave run-qt6

test-all: ## Compile and run all tests
	@$(MAKE) -C mojave test-all

test-fast: ## Compile and run only [fast] tests
	@$(MAKE) -C mojave test-fast

test-changed: ## Incremental build; run tests for changed mojave/ paths
	@$(MAKE) -C mojave test-changed

check: ## Syntax-check sources
	@$(MAKE) -C mojave check

coverage: ## Build and run tests with coverage report (gcovr)
	@$(MAKE) -C mojave coverage

quality: ## fmt + lint + test
	@$(MAKE) -C mojave quality

fmt: ## Format with clang-format
	@$(MAKE) -C mojave fmt

lint: ## Run cppcheck
	@$(MAKE) -C mojave lint

clean: ## Remove build artifacts
	@$(MAKE) -C mojave clean
