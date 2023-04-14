MAKEFLAGS += --no-print-directory

.DEFAULT_GOAL := help

.PHONY: build-tty check clean coverage fmt help install-tty lint quality run-tty setup test-all test-changed test-fast uninstall-tty

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

install-tty: build-tty ## Install mojave
	@BINARY_NAME=mojave PREFIX="$(PREFIX)" ../.make/install.sh

uninstall-tty: ## Remove mojave
	@BINARY_NAME=mojave PREFIX="$(PREFIX)" ../.make/uninstall.sh

run-tty: build-tty ## Run mojave
	@BINARY_NAME=mojave ../.make/run.sh $(ARGS)

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
