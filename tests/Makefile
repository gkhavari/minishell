# **************************************************************************** #
#  Tests Makefile — LucasKuhn/minishell_tester + project local_tests           #
#  make test  = run full suite (Lucas builtins, pipes, redirects, extras + local)
# **************************************************************************** #

.PHONY: all test test_lucas test_local clean help

all: test

# Run LucasKuhn tester (includes our tests/local_tests as "local"); clones into tests/lucas_minishell_tester if needed
test: test_lucas
	@echo ""
	@echo "✅ Test suite finished (LucasKuhn + local_tests)."

# LucasKuhn/minishell_tester — clones into tests/lucas_minishell_tester if missing; injects tests/local_tests as "local"
test_lucas:
	@chmod +x run_lucas_tester.sh 2>/dev/null || true
	@bash run_lucas_tester.sh

# Run only our consolidated local tests (requires Lucas clone and patched tester)
test_local:
	@chmod +x run_lucas_tester.sh 2>/dev/null || true
	@bash run_lucas_tester.sh local

clean:
	@rm -f test_builtins 2>/dev/null || true
	@echo "Tests clean."

help:
	@echo "Targets:"
	@echo "  make test        - Run full suite (LucasKuhn + tests/local_tests)"
	@echo "  make test_lucas  - Same as test (Lucas runner with local injected)"
	@echo "  make test_local  - Run only tests/local_tests (Lucas 'local' file)"
	@echo "  make clean      - Remove test binaries if present"
	@echo "  make help       - This help"
