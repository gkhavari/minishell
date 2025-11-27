/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 20:29:23 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/27 20:29:24 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Minimal `export` builtin: support KEY=VALUE assignments and printing when no args.
 * This is a simple implementation for scaffolding and test purposes.
 */
#include "minishell.h"
#include <string.h>

static int find_key_index(char **envp, const char *key)
{
    int i = 0;
    size_t len = strlen(key);

    while (envp && envp[i])
    {
        if (strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
            return i;
        i++;
    }
    return -1;
}

static char *make_kv(const char *key, const char *val)
{
    size_t k = strlen(key);
    size_t v = strlen(val);
    char *s = malloc(k + v + 2);
    if (!s) return NULL;
    memcpy(s, key, k);
    s[k] = '=';
    memcpy(s + k + 1, val, v);
    s[k + 1 + v] = '\0';
    return s;
}

static void append_env(t_shell *shell, char *kv)
{
    int n = 0;
    while (shell->envp && shell->envp[n]) n++;
    char **ne = malloc(sizeof(char *) * (n + 2));
    if (!ne) return;
    for (int i = 0; i < n; i++)
        ne[i] = strdup(shell->envp[i]);
    ne[n] = kv;
    ne[n + 1] = NULL;
    shell->envp = ne;
}

static void replace_env(t_shell *shell, int idx, char *kv)
{
    free(shell->envp[idx]);
    shell->envp[idx] = kv;
}

int builtin_export(char **args, t_shell *shell)
{
    if (!args || !shell)
        return (1);
    if (!args[1])
    {
        /* Print env */
        int i = 0;
        while (shell->envp && shell->envp[i])
        {
            printf("%s\n", shell->envp[i]);
            i++;
        }
        return (0);
    }
    for (int i = 1; args[i]; i++)
    {
        char *eq = strchr(args[i], '=');
        if (!eq)
            continue; /* skip names without = for now */
        size_t keylen = eq - args[i];
        char *key = strndup(args[i], keylen);
        char *val = strdup(eq + 1);
        char *kv = make_kv(key, val);
        free(key);
        free(val);
        if (!kv) continue;
        int idx = find_key_index(shell->envp, kv);
        if (idx >= 0)
            replace_env(shell, idx, kv);
        else
            append_env(shell, kv);
    }
    return (0);
}
