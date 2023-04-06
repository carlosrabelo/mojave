MAKEFLAGS += --no-print-directory

.DEFAULT_GOAL := help

.PHONY: build-tty clean fmt help install-tty lint run-tty setup test-all uninstall-tty

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

fmt: ## Format with clang-format
	@$(MAKE) -C mojave fmt

lint: ## Run cppcheck
	@$(MAKE) -C mojave lint

clean: ## Remove build artifacts
	@$(MAKE) -C mojave clean
