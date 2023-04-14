MAKEFLAGS += --no-print-directory

.DEFAULT_GOAL := help

.PHONY: build-tty clean coverage fmt help install-tty lint quality run-tty setup test-all test-changed test-fast uninstall-tty

help: ## Show available targets
	@$(MAKE) -C mojave help

setup: ## Download dependencies (e.g. Catch2 header)
	@$(MAKE) -C mojave setup

build-tty: ## Compile mojave binary
	@$(MAKE) -C mojave build-tty

install-tty: ## Install mojave
	@$(MAKE) -C mojave install-tty

uninstall-tty: ## Remove mojave
	@$(MAKE) -C mojave uninstall-tty

run-tty: ## Run mojave
	@$(MAKE) -C mojave run-tty

test-all: ## Compile and run all tests
	@$(MAKE) -C mojave test-all

test-fast: ## Compile and run only [fast] tests
	@$(MAKE) -C mojave test-fast

test-changed: ## Incremental build; run tests for changed mojave/ paths
	@$(MAKE) -C mojave test-changed

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
