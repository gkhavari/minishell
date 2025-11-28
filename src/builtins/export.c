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

static int find_env_key_index(char **envp, const char *key)
{
    int index = 0;
    size_t key_len = strlen(key);

    while (envp && envp[index])
    {
        if (strncmp(envp[index], key, key_len) == 0 && envp[index][key_len] == '=')
            return index;
        index++;
    }
    return -1;
}

static char *create_env_entry(const char *key, const char *value)
{
    size_t key_len = strlen(key);
    size_t value_len = strlen(value);
    char *entry = malloc(key_len + value_len + 2);
    if (!entry) return NULL;
    memcpy(entry, key, key_len);
    entry[key_len] = '=';
    memcpy(entry + key_len + 1, value, value_len);
    entry[key_len + 1 + value_len] = '\0';
    return entry;
}

static void append_env_entry(t_shell *shell, char *entry)
{
    int env_count = 0;
    while (shell->envp && shell->envp[env_count]) env_count++;
    char **new_envp = malloc(sizeof(char *) * (env_count + 2));
    if (!new_envp) return;
    for (int copy_index = 0; copy_index < env_count; copy_index++)
        new_envp[copy_index] = strdup(shell->envp[copy_index]);
    new_envp[env_count] = entry;
    new_envp[env_count + 1] = NULL;
    shell->envp = new_envp;
}

static void replace_env_entry(t_shell *shell, int target_index, char *entry)
{
    free(shell->envp[target_index]);
    shell->envp[target_index] = entry;
}

int builtin_export(char **args, t_shell *shell)
{
    if (!args || !shell)
        return (1);
    if (!args[1])
    {
        /* Print env */
        int env_index = 0;
        while (shell->envp && shell->envp[env_index])
        {
            printf("%s\n", shell->envp[env_index]);
            env_index++;
        }
        return (0);
    }
    for (int arg_index = 1; args[arg_index]; arg_index++)
    {
        char *equals_sign = strchr(args[arg_index], '=');
        if (!equals_sign)
            continue; /* skip names without = for now */
        size_t key_len = equals_sign - args[arg_index];
        char *key = strndup(args[arg_index], key_len);
        char *value = strdup(equals_sign + 1);
        char *entry = create_env_entry(key, value);
        free(key);
        free(value);
        if (!entry) continue;
        int existing_index = find_env_key_index(shell->envp, entry);
        if (existing_index >= 0)
            replace_env_entry(shell, existing_index, entry);
        else
            append_env_entry(shell, entry);
    }
    return (0);
}
