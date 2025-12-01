/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/12/01 18:21:17 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Unit tests for Phase 1 builtins
** Compile: make run (from tests directory)
** Run: ./test_builtins
*/

#include "minishell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Test counters */
static int g_pass = 0;
static int g_fail = 0;

/* Colors */
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[1;33m"
#define NC "\033[0m"

/* Helper to create test shell */
static void	init_test_shell(t_shell *shell, char **envp)
{
	ft_bzero(shell, sizeof(t_shell));
	shell->envp = ft_arrdup(envp);
	shell->cwd = getcwd(NULL, 0);
	shell->user = ft_strdup("testuser");
	shell->last_exit = 0;
}

/* Helper to free test shell */
static void	free_test_shell(t_shell *shell)
{
	int	i;

	if (shell->envp)
	{
		i = 0;
		while (shell->envp[i])
			free(shell->envp[i++]);
		free(shell->envp);
	}
	free(shell->cwd);
	free(shell->user);
}

/* Test assertion */
static void	assert_eq(const char *name, int expected, int actual)
{
	if (expected == actual)
	{
		printf(GREEN "[PASS]" NC " %s\n", name);
		g_pass++;
	}
	else
	{
		printf(RED "[FAIL]" NC " %s (expected %d, got %d)\n",
			name, expected, actual);
		g_fail++;
	}
}

/* ========================================================================== */
/*                              ECHO TESTS                                     */
/* ========================================================================== */

static void	test_echo(t_shell *shell)
{
	char	*args1[] = {"echo", "hello", NULL};
	char	*args2[] = {"echo", "-n", "hello", NULL};
	char	*args3[] = {"echo", NULL};
	char	*args4[] = {"echo", "-nnnnn", "test", NULL};
	char	*args5[] = {"echo", "-n", "-n", "-n", "multi", NULL};
	char	*args6[] = {"echo", "-n", "-a", "invalid", NULL};
	char	*args7[] = {"echo", "", NULL};
	char	*args8[] = {"echo", "hello", "world", "foo", NULL};
	int		ret;

	printf(YELLOW "\n--- Echo Tests ---\n" NC);

	/* Test: echo hello */
	printf("Output: ");
	ret = builtin_echo(args1, shell);
	assert_eq("echo hello returns 0", 0, ret);

	/* Test: echo -n hello */
	printf("Output: ");
	ret = builtin_echo(args2, shell);
	printf("\n");
	assert_eq("echo -n hello returns 0", 0, ret);

	/* Test: echo (no args) */
	printf("Output: ");
	ret = builtin_echo(args3, shell);
	assert_eq("echo (no args) returns 0", 0, ret);

	/* Test: echo -nnnnn test */
	printf("Output: ");
	ret = builtin_echo(args4, shell);
	printf("\n");
	assert_eq("echo -nnnnn returns 0", 0, ret);

	/* Test: echo -n -n -n multi */
	printf("Output: ");
	ret = builtin_echo(args5, shell);
	printf("\n");
	assert_eq("echo -n -n -n returns 0", 0, ret);

	/* Test: echo -n -a invalid (stops at -a) */
	printf("Output: ");
	ret = builtin_echo(args6, shell);
	assert_eq("echo -n -a prints -a", 0, ret);

	/* Test: echo "" (empty string) */
	printf("Output: ");
	ret = builtin_echo(args7, shell);
	assert_eq("echo empty string returns 0", 0, ret);

	/* Test: echo hello world foo (multiple args) */
	printf("Output: ");
	ret = builtin_echo(args8, shell);
	assert_eq("echo multiple args returns 0", 0, ret);
}

/* ========================================================================== */
/*                              PWD TESTS                                      */
/* ========================================================================== */

static void	test_pwd(t_shell *shell)
{
	char	*args[] = {"pwd", NULL};
	int		ret;

	printf(YELLOW "\n--- PWD Tests ---\n" NC);

	ret = builtin_pwd(args, shell);
	assert_eq("pwd returns 0", 0, ret);
}

/* ========================================================================== */
/*                              CD TESTS                                       */
/* ========================================================================== */

static void	test_cd(t_shell *shell)
{
	char	*args_tmp[] = {"cd", "/tmp", NULL};
	char	*args_home[] = {"cd", NULL};
	char	*args_invalid[] = {"cd", "/nonexistent_12345", NULL};
	char	*original_cwd;
	char	*pwd_val;
	char	*oldpwd_val;
	int		ret;

	printf(YELLOW "\n--- CD Tests ---\n" NC);

	original_cwd = getcwd(NULL, 0);

	/* Test: cd /tmp */
	ret = builtin_cd(args_tmp, shell);
	assert_eq("cd /tmp returns 0", 0, ret);

	/* Check PWD was updated */
	pwd_val = get_env(shell->envp, "PWD");
	if (pwd_val && ft_strncmp(pwd_val, "/tmp", 4) == 0)
		printf(GREEN "[PASS]" NC " cd updates PWD env var\n");
	else
		printf(RED "[FAIL]" NC " cd should update PWD\n");

	/* Check OLDPWD was updated */
	oldpwd_val = get_env(shell->envp, "OLDPWD");
	if (oldpwd_val)
		printf(GREEN "[PASS]" NC " cd updates OLDPWD env var\n");
	else
		printf(RED "[FAIL]" NC " cd should update OLDPWD\n");

	/* Test: cd (home) */
	ret = builtin_cd(args_home, shell);
	assert_eq("cd (home) returns 0", 0, ret);

	/* Test: cd invalid */
	ret = builtin_cd(args_invalid, shell);
	assert_eq("cd invalid returns 1", 1, ret);

	/* Restore */
	chdir(original_cwd);
	free(original_cwd);
}

/* ========================================================================== */
/*                              ENV TESTS                                     */
/* ========================================================================== */

static void	test_env(t_shell *shell)
{
	char	*args[] = {"env", NULL};
	int		ret;

	printf(YELLOW "\n--- ENV Tests ---\n" NC);

	ret = builtin_env(args, shell);
	assert_eq("env returns 0", 0, ret);
}

/* ========================================================================== */
/*                              EXPORT TESTS                                  */
/* ========================================================================== */

static void	test_export(t_shell *shell)
{
	char	*args_set[] = {"export", "TEST_VAR=hello", NULL};
	char	*args_invalid[] = {"export", "1INVALID=x", NULL};
	char	*args_equals[] = {"export", "VAR=hello=world", NULL};
	char	*args_empty[] = {"export", "EMPTY_VAR=", NULL};
	char	*args_update[] = {"export", "TEST_VAR=updated", NULL};
	char	*val;
	int		ret;

	printf(YELLOW "\n--- Export Tests ---\n" NC);

	/* Test: export TEST_VAR=hello */
	ret = builtin_export(args_set, shell);
	assert_eq("export set var returns 0", 0, ret);

	/* Verify TEST_VAR was set */
	val = get_env(shell->envp, "TEST_VAR");
	if (val && strcmp(val, "hello") == 0)
	{
		printf(GREEN "[PASS]" NC " export correctly sets TEST_VAR=hello\n");
		g_pass++;
	}
	else
	{
		printf(RED "[FAIL]" NC " export should set TEST_VAR=hello\n");
		g_fail++;
	}

	/* Test: export 1INVALID=x */
	ret = builtin_export(args_invalid, shell);
	assert_eq("export invalid returns 1", 1, ret);

	/* Test: export VAR=hello=world (value contains =) */
	ret = builtin_export(args_equals, shell);
	assert_eq("export VAR=hello=world returns 0", 0, ret);
	val = get_env(shell->envp, "VAR");
	if (val && strcmp(val, "hello=world") == 0)
	{
		printf(GREEN "[PASS]" NC " export handles = in value\n");
		g_pass++;
	}
	else
	{
		printf(RED "[FAIL]" NC " export should handle = in value\n");
		g_fail++;
	}

	/* Test: export EMPTY_VAR= */
	ret = builtin_export(args_empty, shell);
	assert_eq("export EMPTY_VAR= returns 0", 0, ret);

	/* Test: export TEST_VAR=updated (update existing) */
	ret = builtin_export(args_update, shell);
	assert_eq("export update returns 0", 0, ret);
	val = get_env(shell->envp, "TEST_VAR");
	if (val && strcmp(val, "updated") == 0)
	{
		printf(GREEN "[PASS]" NC " export updates existing variable\n");
		g_pass++;
	}
	else
	{
		printf(RED "[FAIL]" NC " export should update existing variable\n");
		g_fail++;
	}
}

/* ========================================================================== */
/*                              UNSET TESTS                                    */
/* ========================================================================== */

static void	test_unset(t_shell *shell)
{
	char	*args_valid[] = {"unset", "TEST_VAR", NULL};
	char	*args_invalid[] = {"unset", "1INVALID", NULL};
	char	*args_nonexist[] = {"unset", "NONEXISTENT_VAR_12345", NULL};
	int		ret;

	printf(YELLOW "\n--- Unset Tests ---\n" NC);

	/* Test: unset valid */
	ret = builtin_unset(args_valid, shell);
	assert_eq("unset valid var returns 0", 0, ret);

	/* Test: unset invalid name */
	ret = builtin_unset(args_invalid, shell);
	assert_eq("unset invalid returns 1", 1, ret);

	/* Test: unset nonexistent (should still return 0) */
	ret = builtin_unset(args_nonexist, shell);
	assert_eq("unset nonexistent returns 0", 0, ret);
}

/* ========================================================================== */
/*                              EXIT TESTS                                     */
/* ========================================================================== */

#include <sys/wait.h>

/* Helper to test exit in forked process and capture exit code */
static int	test_exit_fork(t_shell *shell, char *code)
{
	pid_t	pid;
	int		status;
	char	*args[3];

	args[0] = "exit";
	args[1] = code;
	args[2] = NULL;
	pid = fork();
	if (pid == 0)
	{
		builtin_exit(args, shell);
		exit(99);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (-1);
}

static void	test_exit(t_shell *shell)
{
	int	exit_code;

	printf(YELLOW "\n--- Exit Tests ---\n" NC);

	/* Normal exit codes */
	exit_code = test_exit_fork(shell, "42");
	assert_eq("exit 42 returns 42", 42, exit_code);

	exit_code = test_exit_fork(shell, "0");
	assert_eq("exit 0 returns 0", 0, exit_code);

	exit_code = test_exit_fork(shell, "255");
	assert_eq("exit 255 returns 255", 255, exit_code);

	/* Wraparound (256 -> 0) */
	exit_code = test_exit_fork(shell, "256");
	assert_eq("exit 256 wraps to 0", 0, exit_code);

	/* Negative */
	exit_code = test_exit_fork(shell, "-1");
	assert_eq("exit -1 returns 255", 255, exit_code);

	/* Overflow detection - should return 2 (numeric argument required) */
	exit_code = test_exit_fork(shell, "9999999999999999999");
	assert_eq("exit overflow returns 2", 2, exit_code);

	exit_code = test_exit_fork(shell, "-9999999999999999999");
	assert_eq("exit negative overflow returns 2", 2, exit_code);

	/* LONG_MAX should be valid */
	exit_code = test_exit_fork(shell, "9223372036854775807");
	assert_eq("exit LONG_MAX wraps to 255", 255, exit_code);

	/* LONG_MAX + 1 should overflow */
	exit_code = test_exit_fork(shell, "9223372036854775808");
	assert_eq("exit LONG_MAX+1 returns 2 (overflow)", 2, exit_code);

	/* Non-numeric */
	exit_code = test_exit_fork(shell, "abc");
	assert_eq("exit abc returns 2 (non-numeric)", 2, exit_code);
}

/* ========================================================================== */
/*                              MAIN                                           */
/* ========================================================================== */

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;

	printf("\n");
	printf("==========================================\n");
	printf("   PHASE 1: BUILTIN UNIT TESTS\n");
	printf("==========================================\n");

	init_test_shell(&shell, envp);

	test_echo(&shell);
	test_pwd(&shell);
	test_cd(&shell);
	test_env(&shell);
	test_export(&shell);
	test_unset(&shell);
	test_exit(&shell);

	free_test_shell(&shell);

	printf("\n");
	printf("==========================================\n");
	printf("              SUMMARY\n");
	printf("==========================================\n");
	printf("Passed: %d\n", g_pass);
	printf("Failed: %d\n", g_fail);
	printf("Total:  %d\n", g_pass + g_fail);
	printf("\n");

	if (g_fail == 0)
		printf(GREEN "✅ All tests passed!\n" NC);
	else
		printf(YELLOW "⚠️  Some tests failed.\n" NC);

	return (g_fail > 0);
}
