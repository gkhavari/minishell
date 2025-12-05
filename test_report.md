## Minishell Tester Results

**Branch:** copilot/sub-pr-15-again
**Commit:** f433d6f5f74776f7e139d7599e5b93a287c30a9c
**Date:** Fri Dec  5 15:50:35 UTC 2025

```
[0;31m🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥
                                                                                             
 ██    ██ █ ██    █ █ █████ █   █ █████ █     █       ██████ █████ █████ ██████ █████ █████  
 ███  ███ █ ███   █ █ █     █   █ █     █     █         ██   █     █       ██   █     █   ██ 
 █ ████ █ █ █ ██  █ █ █████ █████ ████  █     █         ██   ████  █████   ██   ████  █████  
 █  ██  █ █ █  ██ █ █     █ █   █ █     █     █         ██   █         █   ██   █     █   ██ 
 █      █ █ █   ███ █ █████ █   █ █████ █████ █████     ██   █████ █████   ██   █████ █   ██ 
                                                                                             
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥
                                                                                              
[0;31m———————————— builtins
[0;33mTest   1: ❌[38;5;244m echo hello world 
[0mmini output = (0 echo 0 hello 0 world)
bash output = (hello world)
mini exit code =
bash exit code = 0
[0;33mTest   2: ❌[38;5;244m echo "hello world" 
[0mmini output = (0 echo 0 hello world)
bash output = (hello world)
mini exit code =
bash exit code = 0
[0;33mTest   3: ❌[38;5;244m echo 'hello world' 
[0mmini output = (0 echo 0 hello world)
bash output = (hello world)
mini exit code =
bash exit code = 0
[0;33mTest   4: ❌[38;5;244m echo hello'world' 
[0mmini output = (0 echo 0 helloworld)
bash output = (helloworld)
mini exit code =
bash exit code = 0
[0;33mTest   5: ❌[38;5;244m echo hello""world 
[0mmini output = (0 echo 0 helloworld)
bash output = (helloworld)
mini exit code =
bash exit code = 0
[0;33mTest   6: ❌[38;5;244m echo '' 
[0mmini output = (0 echo)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest   7: ❌[38;5;244m echo "$PWD" 
[0mmini output = (0 echo 0 /home/runner/work/minishell/minishell/minishell_tester)
bash output = (/home/runner/work/minishell/minishell/minishell_tester)
mini exit code =
bash exit code = 0
[0;33mTest   8: ❌[38;5;244m echo '$PWD' 
[0mmini output = (0 echo 0 $PWD)
bash output = ($PWD)
mini exit code =
bash exit code = 0
[0;33mTest   9: ❌[38;5;244m echo "aspas ->'" 
[0mmini output = (0 echo 0 aspas ->')
bash output = (aspas ->')
mini exit code =
bash exit code = 0
[0;33mTest  10: ❌[38;5;244m echo "aspas -> ' " 
[0mmini output = (0 echo 0 aspas -> ' )
bash output = (aspas -> ' )
mini exit code =
bash exit code = 0
[0;33mTest  11: ❌[38;5;244m echo 'aspas ->"' 
[0mmini output = (0 echo 0 aspas ->")
bash output = (aspas ->")
mini exit code =
bash exit code = 0
[0;33mTest  12: ❌[38;5;244m echo 'aspas -> " ' 
[0mmini output = (0 echo 0 aspas -> " )
bash output = (aspas -> " )
mini exit code =
bash exit code = 0
[0;33mTest  13: ❌[38;5;244m echo "> >> < * ? [ ] | ; [ ] || && ( ) & # $  <<" 
[0mmini output = (0 echo 0 > >> < README.md bash.supp bash_outfiles bonus bonus_bonus builtins extras local.supp loop.out manual_tests mini_outfiles os_specific outfiles pipes redirects syntax test_files test_output.txt test_report.md tester wildcards ? [ ] | ; [ ] || && ( ) & # $ <<)
bash output = (> >> < README.md bash.supp bash_outfiles bonus bonus_bonus builtins extras local.supp loop.out manual_tests mini_outfiles os_specific outfiles pipes redirects syntax test_files test_output.txt test_report.md tester wildcards ? [ ] | ; [ ] || && ( ) & # $ <<)
mini exit code =
bash exit code = 0
[0;33mTest  14: ❌[38;5;244m echo '> >> < * ? [ ] | ; [ ] || && ( ) & # $  <<' 
[0mmini output = (0 echo 0 > >> < README.md bash.supp bash_outfiles bonus bonus_bonus builtins extras local.supp loop.out manual_tests mini_outfiles os_specific outfiles pipes redirects syntax test_files test_output.txt test_report.md tester wildcards ? [ ] | ; [ ] || && ( ) & # $ <<)
bash output = (> >> < README.md bash.supp bash_outfiles bonus bonus_bonus builtins extras local.supp loop.out manual_tests mini_outfiles os_specific outfiles pipes redirects syntax test_files test_output.txt test_report.md tester wildcards ? [ ] | ; [ ] || && ( ) & # $ <<)
mini exit code =
bash exit code = 0
[0;33mTest  15: ❌[38;5;244m echo "exit_code ->$? user ->$USER home -> $HOME" 
[0mmini output = (0 echo 0 exit_code ->0 user ->runner home -> /home/runner)
bash output = (exit_code ->0 user ->runner home -> /home/runner)
mini exit code =
bash exit code = 0
[0;33mTest  16: ❌[38;5;244m echo 'exit_code ->$? user ->$USER home -> $HOME' 
[0mmini output = (0 echo 0 exit_code ->$? user ->$USER home -> $HOME)
bash output = (exit_code ->$? user ->$USER home -> $HOME)
mini exit code =
bash exit code = 0
[0;33mTest  17: ❌[38;5;244m echo "$" 
[0mmini output = (0 echo 0 $)
bash output = ($)
mini exit code =
bash exit code = 0
[0;33mTest  18: ❌[38;5;244m echo '$' 
[0mmini output = (0 echo 0 $)
bash output = ($)
mini exit code =
bash exit code = 0
[0;33mTest  19: ❌[38;5;244m echo $ 
[0mmini output = (0 echo 0 $)
bash output = ($)
mini exit code =
bash exit code = 0
[0;33mTest  20: ❌[38;5;244m echo $? 
[0mmini output = (0 echo 0 0)
bash output = (0)
mini exit code =
bash exit code = 0
[0;33mTest  21: ❌[38;5;244m echo $?HELLO 
[0mmini output = (0 echo 0 0HELLO)
bash output = (0HELLO)
mini exit code =
bash exit code = 0
[0;33mTest  22: ❌[38;5;244m pwd 
[0mmini output = (0 pwd)
bash output = (/home/runner/work/minishell/minishell/minishell_tester)
mini exit code =
bash exit code = 0
[0;33mTest  23: ❌[38;5;244m pwd oi 
[0mmini output = (0 pwd 0 oi)
bash output = (/home/runner/work/minishell/minishell/minishell_tester)
mini exit code =
bash exit code = 0
[0;33mTest  24: ❌[38;5;244m export hello 
[0mmini output = (0 export 0 hello)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  25: ❌[38;5;244m export HELLO=123 
[0mmini output = (0 export 0 HELLO=123)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  26: ❌[38;5;244m export A- 
[0mmini output = (0 export 0 A-)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( not a valid identifier)
[0;33mTest  27: ❌[38;5;244m export HELLO=123 A 
[0mmini output = (0 export 0 HELLO=123 0 A)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  28: ❌[38;5;244m export HELLO="123 A-" 
[0mmini output = (0 export 0 HELLO=123 A-)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  29: ❌[38;5;244m export hello world 
[0mmini output = (0 export 0 hello 0 world)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  30: ❌[38;5;244m export HELLO-=123 
[0mmini output = (0 export 0 HELLO-=123)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( not a valid identifier)
[0;33mTest  31: ❌[38;5;244m export = 
[0mmini output = (0 export 0 =)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( not a valid identifier)
[0;33mTest  32: ❌[38;5;244m export 123 
[0mmini output = (0 export 0 123)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( not a valid identifier)
[0;33mTest  33: ❌[38;5;244m unset 
[0mmini output = (0 unset)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  34: ❌[38;5;244m unset HELLO 
[0mmini output = (0 unset 0 HELLO)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  35: ❌[38;5;244m unset HELLO1 HELLO2 
[0mmini output = (0 unset 0 HELLO1 0 HELLO2)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  36: ❌[38;5;244m unset HOME 
[0mmini output = (0 unset 0 HOME)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  37: ❌[38;5;244m unset PATH 
[0mmini output = (0 unset 0 PATH)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  38: ❌[38;5;244m unset SHELL 
[0mmini output = (0 unset 0 SHELL)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  39: ❌[38;5;244m cd $PWD 
[0mmini output = (0 cd 0 /home/runner/work/minishell/minishell/minishell_tester)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  40: ❌[38;5;244m cd $PWD hi 
[0mmini output = (0 cd 0 /home/runner/work/minishell/minishell/minishell_tester 0 hi)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( too many arguments)
[0;33mTest  41: ❌[38;5;244m cd 123123 
[0mmini output = (0 cd 0 123123)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  42: ❌[38;5;244m exit 123 
[0mmini output = (0 0 123)
bash output = ()
mini exit code =
bash exit code = 123
[0;33mTest  43: ❌[38;5;244m exit 298 
[0mmini output = (0 0 298)
bash output = ()
mini exit code =
bash exit code = 42
[0;33mTest  44: ❌[38;5;244m exit +100 
[0mmini output = (0 0 +100)
bash output = ()
mini exit code =
bash exit code = 100
[0;33mTest  45: ❌[38;5;244m exit "+100" 
[0mmini output = (0 0 +100)
bash output = ()
mini exit code =
bash exit code = 100
[0;33mTest  46: ❌[38;5;244m exit +"100" 
[0mmini output = (0 0 +100)
bash output = ()
mini exit code =
bash exit code = 100
[0;33mTest  47: ❌[38;5;244m exit -100 
[0mmini output = (0 0 -100)
bash output = ()
mini exit code =
bash exit code = 156
[0;33mTest  48: ❌[38;5;244m exit "-100" 
[0mmini output = (0 0 -100)
bash output = ()
mini exit code =
bash exit code = 156
[0;33mTest  49: ❌[38;5;244m exit -"100" 
[0mmini output = (0 0 -100)
bash output = ()
mini exit code =
bash exit code = 156
[0;33mTest  50: ❌[38;5;244m exit hello 
[0mmini output = (0 0 hello)
bash output = ()
mini exit code =
bash exit code = 2
mini error = ()
bash error = ( numeric argument required)
[0;33mTest  51: ❌[38;5;244m exit 42 world 
[0mmini output = (0 0 42 0 world)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( too many arguments)
[0;33mTest  52: ❌[38;5;244m  
[0mmini exit code =
bash exit code = 0
[0;31m———————————— pipes
[0;33mTest  53: ❌[38;5;244m env | sort | grep -v SHLVL | grep -v ^_ 
[0mmini output = (SHELL=/bin/bash SELENIUM_JAR_PATH=/usr/share/java/selenium-server.jar CONDA=/usr/share/miniconda GITHUB_WORKSPACE=/home/runner/work/minishell/minishell JAVA_HOME_11_X64=/usr/lib/jvm/temurin-11-jdk-amd64 JAVA_HOME_25_X64=/usr/lib/jvm/temurin-25-jdk-amd64 GITHUB_PATH=/home/runner/work/_temp/_runner_file_commands/add_path_34b4b202-8424-4088-b325-e56895b7c5cc GITHUB_ACTION=run_tests JAVA_HOME=/usr/lib/jvm/temurin-17-jdk-amd64 GITHUB_RUN_NUMBER=25 RUNNER_NAME=GitHub Actions 1000000044 GRADLE_HOME=/usr/share/gradle-9.2.1 GITHUB_REPOSITORY_OWNER_ID=201211038 ACTIONS_RUNNER_ACTION_ARCHIVE_CACHE=/opt/actionarchivecache XDG_CONFIG_HOME=/home/runner/.config MEMORY_PRESSURE_WRITE=c29tZSAyMDAwMDAgMjAwMDAwMAA= DOTNET_SKIP_FIRST_TIME_EXPERIENCE=1 ANT_HOME=/usr/share/ant JAVA_HOME_8_X64=/usr/lib/jvm/temurin-8-jdk-amd64 GITHUB_TRIGGERING_ACTOR=gkhavari GITHUB_REF_TYPE=branch HOMEBREW_CLEANUP_PERIODIC_FULL_DAYS=3650 ANDROID_NDK=/usr/local/lib/android/sdk/ndk/27.3.13750724 BOOTSTRAP_HASKELL_NONINTERACTIVE=1 PIPX_BIN_DIR=/opt/pipx_bin PWD=/home/runner/work/minishell/minishell/minishell_tester LOGNAME=runner GITHUB_REPOSITORY_ID=1103994604 GITHUB_ACTIONS=true ANDROID_NDK_LATEST_HOME=/usr/local/lib/android/sdk/ndk/29.0.14206865 SYSTEMD_EXEC_PID=1929 GITHUB_SHA=f433d6f5f74776f7e139d7599e5b93a287c30a9c GITHUB_WORKFLOW_REF=gkhavari/minishell/.github/workflows/test.yml@refs/heads/copilot/sub-pr-15-again POWERSHELL_DISTRIBUTION_CHANNEL=GitHub-Actions-ubuntu24 RUNNER_ENVIRONMENT=github-hosted DOTNET_MULTILEVEL_LOOKUP=0 GITHUB_REF=refs/heads/copilot/sub-pr-15-again RUNNER_OS=Linux GITHUB_REF_PROTECTED=false HOME=/home/runner GITHUB_API_URL=https://api.github.com LANG=C.UTF-8 GOROOT_1_25_X64=/opt/hostedtoolcache/go/1.25.4/x64 RUNNER_TRACKING_ID=github_57c04d68-19d3-44eb-84a0-631e0cb3684b RUNNER_ARCH=X64 MEMORY_PRESSURE_WATCH=/sys/fs/cgroup/system.slice/hosted-compute-agent.service/memory.pressure RUNNER_TEMP=/home/runner/work/_temp GITHUB_STATE=/home/runner/work/_temp/_runner_file_commands/save_state_34b4b202-8424-4088-b325-e56895b7c5cc EDGEWEBDRIVER=/usr/local/share/edge_driver JAVA_HOME_21_X64=/usr/lib/jvm/temurin-21-jdk-amd64 GITHUB_ENV=/home/runner/work/_temp/_runner_file_commands/set_env_34b4b202-8424-4088-b325-e56895b7c5cc INVOCATION_ID=8700e4f9b7014e5dba936b43074a311a GITHUB_EVENT_PATH=/home/runner/work/_temp/_github_workflow/event.json GITHUB_EVENT_NAME=push GITHUB_RUN_ID=19968243447 JAVA_HOME_17_X64=/usr/lib/jvm/temurin-17-jdk-amd64 ANDROID_NDK_HOME=/usr/local/lib/android/sdk/ndk/27.3.13750724 GITHUB_STEP_SUMMARY=/home/runner/work/_temp/_runner_file_commands/step_summary_34b4b202-8424-4088-b325-e56895b7c5cc HOMEBREW_NO_AUTO_UPDATE=1 GITHUB_ACTOR=gkhavari NVM_DIR=/home/runner/.nvm SGX_AESM_ADDR=1 GITHUB_RUN_ATTEMPT=1 ANDROID_HOME=/usr/local/lib/android/sdk GITHUB_GRAPHQL_URL=https://api.github.com/graphql ACCEPT_EULA=Y USER=runner GITHUB_SERVER_URL=https://github.com PIPX_HOME=/opt/pipx GECKOWEBDRIVER=/usr/local/share/gecko_driver CHROMEWEBDRIVER=/usr/local/share/chromedriver-linux64 SHLVL=2 ANDROID_SDK_ROOT=/usr/local/lib/android/sdk VCPKG_INSTALLATION_ROOT=/usr/local/share/vcpkg GITHUB_ACTOR_ID=201211038 RUNNER_TOOL_CACHE=/opt/hostedtoolcache ImageVersion=20251126.144.1 DOTNET_NOLOGO=1 GOROOT_1_23_X64=/opt/hostedtoolcache/go/1.23.12/x64 GITHUB_WORKFLOW_SHA=f433d6f5f74776f7e139d7599e5b93a287c30a9c GOROOT_1_24_X64=/opt/hostedtoolcache/go/1.24.10/x64 GITHUB_REF_NAME=copilot/sub-pr-15-again GITHUB_JOB=test XDG_RUNTIME_DIR=/run/user/1001 AZURE_EXTENSION_DIR=/opt/az/azcliextensions GITHUB_REPOSITORY=gkhavari/minishell ANDROID_NDK_ROOT=/usr/local/lib/android/sdk/ndk/27.3.13750724 CHROME_BIN=/usr/bin/google-chrome GOROOT_1_22_X64=/opt/hostedtoolcache/go/1.22.12/x64 GITHUB_RETENTION_DAYS=90 JOURNAL_STREAM=9:13272 RUNNER_WORKSPACE=/home/runner/work/minishell GITHUB_ACTION_REPOSITORY= PATH=/snap/bin:/home/runner/.local/bin:/opt/pipx_bin:/home/runner/.cargo/bin:/home/runner/.config/composer/vendor/bin:/usr/local/.ghcup/bin:/home/runner/.dotnet/tools:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin GITHUB_BASE_REF= GHCUP_INSTALL_BASE_PREFIX=/usr/local CI=true SWIFT_PATH=/usr/share/swift/usr/bin ImageOS=ubuntu24 GITHUB_REPOSITORY_OWNER=gkhavari GITHUB_HEAD_REF= GITHUB_ACTION_REF= ENABLE_RUNNER_TRACING=true GITHUB_WORKFLOW=CI - Test with minishell_tester DEBIAN_FRONTEND=noninteractive GITHUB_OUTPUT=/home/runner/work/_temp/_runner_file_commands/set_output_34b4b202-8424-4088-b325-e56895b7c5cc AGENT_TOOLSDIRECTORY=/opt/hostedtoolcache OLDPWD=/home/runner/work/minishell/minishell _=../minishell)
bash output = (ACCEPT_EULA=Y ACTIONS_RUNNER_ACTION_ARCHIVE_CACHE=/opt/actionarchivecache AGENT_TOOLSDIRECTORY=/opt/hostedtoolcache ANDROID_HOME=/usr/local/lib/android/sdk ANDROID_NDK=/usr/local/lib/android/sdk/ndk/27.3.13750724 ANDROID_NDK_HOME=/usr/local/lib/android/sdk/ndk/27.3.13750724 ANDROID_NDK_LATEST_HOME=/usr/local/lib/android/sdk/ndk/29.0.14206865 ANDROID_NDK_ROOT=/usr/local/lib/android/sdk/ndk/27.3.13750724 ANDROID_SDK_ROOT=/usr/local/lib/android/sdk ANT_HOME=/usr/share/ant AZURE_EXTENSION_DIR=/opt/az/azcliextensions BOOTSTRAP_HASKELL_NONINTERACTIVE=1 CHROMEWEBDRIVER=/usr/local/share/chromedriver-linux64 CHROME_BIN=/usr/bin/google-chrome CI=true CONDA=/usr/share/miniconda DEBIAN_FRONTEND=noninteractive DOTNET_MULTILEVEL_LOOKUP=0 DOTNET_NOLOGO=1 DOTNET_SKIP_FIRST_TIME_EXPERIENCE=1 EDGEWEBDRIVER=/usr/local/share/edge_driver ENABLE_RUNNER_TRACING=true GECKOWEBDRIVER=/usr/local/share/gecko_driver GHCUP_INSTALL_BASE_PREFIX=/usr/local GITHUB_ACTION=run_tests GITHUB_ACTIONS=true GITHUB_ACTION_REF= GITHUB_ACTION_REPOSITORY= GITHUB_ACTOR=gkhavari GITHUB_ACTOR_ID=201211038 GITHUB_API_URL=https://api.github.com GITHUB_BASE_REF= GITHUB_ENV=/home/runner/work/_temp/_runner_file_commands/set_env_34b4b202-8424-4088-b325-e56895b7c5cc GITHUB_EVENT_NAME=push GITHUB_EVENT_PATH=/home/runner/work/_temp/_github_workflow/event.json GITHUB_GRAPHQL_URL=https://api.github.com/graphql GITHUB_HEAD_REF= GITHUB_JOB=test GITHUB_OUTPUT=/home/runner/work/_temp/_runner_file_commands/set_output_34b4b202-8424-4088-b325-e56895b7c5cc GITHUB_PATH=/home/runner/work/_temp/_runner_file_commands/add_path_34b4b202-8424-4088-b325-e56895b7c5cc GITHUB_REF=refs/heads/copilot/sub-pr-15-again GITHUB_REF_NAME=copilot/sub-pr-15-again GITHUB_REF_PROTECTED=false GITHUB_REF_TYPE=branch GITHUB_REPOSITORY=gkhavari/minishell GITHUB_REPOSITORY_ID=1103994604 GITHUB_REPOSITORY_OWNER=gkhavari GITHUB_REPOSITORY_OWNER_ID=201211038 GITHUB_RETENTION_DAYS=90 GITHUB_RUN_ATTEMPT=1 GITHUB_RUN_ID=19968243447 GITHUB_RUN_NUMBER=25 GITHUB_SERVER_URL=https://github.com GITHUB_SHA=f433d6f5f74776f7e139d7599e5b93a287c30a9c GITHUB_STATE=/home/runner/work/_temp/_runner_file_commands/save_state_34b4b202-8424-4088-b325-e56895b7c5cc GITHUB_STEP_SUMMARY=/home/runner/work/_temp/_runner_file_commands/step_summary_34b4b202-8424-4088-b325-e56895b7c5cc GITHUB_TRIGGERING_ACTOR=gkhavari GITHUB_WORKFLOW=CI - Test with minishell_tester GITHUB_WORKFLOW_REF=gkhavari/minishell/.github/workflows/test.yml@refs/heads/copilot/sub-pr-15-again GITHUB_WORKFLOW_SHA=f433d6f5f74776f7e139d7599e5b93a287c30a9c GITHUB_WORKSPACE=/home/runner/work/minishell/minishell GOROOT_1_22_X64=/opt/hostedtoolcache/go/1.22.12/x64 GOROOT_1_23_X64=/opt/hostedtoolcache/go/1.23.12/x64 GOROOT_1_24_X64=/opt/hostedtoolcache/go/1.24.10/x64 GOROOT_1_25_X64=/opt/hostedtoolcache/go/1.25.4/x64 GRADLE_HOME=/usr/share/gradle-9.2.1 HOME=/home/runner HOMEBREW_CLEANUP_PERIODIC_FULL_DAYS=3650 HOMEBREW_NO_AUTO_UPDATE=1 INVOCATION_ID=8700e4f9b7014e5dba936b43074a311a ImageOS=ubuntu24 ImageVersion=20251126.144.1 JAVA_HOME=/usr/lib/jvm/temurin-17-jdk-amd64 JAVA_HOME_11_X64=/usr/lib/jvm/temurin-11-jdk-amd64 JAVA_HOME_17_X64=/usr/lib/jvm/temurin-17-jdk-amd64 JAVA_HOME_21_X64=/usr/lib/jvm/temurin-21-jdk-amd64 JAVA_HOME_25_X64=/usr/lib/jvm/temurin-25-jdk-amd64 JAVA_HOME_8_X64=/usr/lib/jvm/temurin-8-jdk-amd64 JOURNAL_STREAM=9:13272 LANG=C.UTF-8 LOGNAME=runner MEMORY_PRESSURE_WATCH=/sys/fs/cgroup/system.slice/hosted-compute-agent.service/memory.pressure MEMORY_PRESSURE_WRITE=c29tZSAyMDAwMDAgMjAwMDAwMAA= NVM_DIR=/home/runner/.nvm OLDPWD=/home/runner/work/minishell/minishell PATH=/snap/bin:/home/runner/.local/bin:/opt/pipx_bin:/home/runner/.cargo/bin:/home/runner/.config/composer/vendor/bin:/usr/local/.ghcup/bin:/home/runner/.dotnet/tools:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin PIPX_BIN_DIR=/opt/pipx_bin PIPX_HOME=/opt/pipx POWERSHELL_DISTRIBUTION_CHANNEL=GitHub-Actions-ubuntu24 PWD=/home/runner/work/minishell/minishell/minishell_tester RUNNER_ARCH=X64 RUNNER_ENVIRONMENT=github-hosted RUNNER_NAME=GitHub Actions 1000000044 RUNNER_OS=Linux RUNNER_TEMP=/home/runner/work/_temp RUNNER_TOOL_CACHE=/opt/hostedtoolcache RUNNER_TRACKING_ID=github_57c04d68-19d3-44eb-84a0-631e0cb3684b RUNNER_WORKSPACE=/home/runner/work/minishell SELENIUM_JAR_PATH=/usr/share/java/selenium-server.jar SGX_AESM_ADDR=1 SHELL=/bin/bash SWIFT_PATH=/usr/share/swift/usr/bin SYSTEMD_EXEC_PID=1929 USER=runner VCPKG_INSTALLATION_ROOT=/usr/local/share/vcpkg XDG_CONFIG_HOME=/home/runner/.config XDG_RUNTIME_DIR=/run/user/1001)
mini exit code = $?
bash exit code = 0
[0;33mTest  54: ❌[38;5;244m cat ./test_files/infile_big | grep oi 
[0mmini output = (0 cat 0 ./test_files/infile_big 1 | 0 grep 0 oi)
bash output = (was going to happen next. First, she tried to look down and make out disappointment it was empty: she did not like to drop the jar for fear not going to do _that_ in a hurry. “No, I’ll look first,” she said, “and see whether it’s marked ‘_poison_’ or not”; for she had read bottle marked “poison,” it is almost certain to disagree with you, However, this bottle was _not_ marked “poison,” so Alice ventured to brightened up at the thought that she was now the right size for going waited for a few minutes to see if she was going to shrink any further: said Alice to herself, “in my going out altogether, like a candle. I After a while, finding that nothing more happened, she decided on going Rabbit came near her, she began, in a low, timid voice, “If you please, to repeat it, but her voice sounded hoarse and strange, and the words now about two feet high, and was going on shrinking rapidly: she soon dropped it hastily, just in time to avoid shrinking away altogether. “Not like cats!” cried the Mouse, in a shrill, passionate voice. “Would trembling voice, “Let us get to the shore, and then I’ll tell you my “What I was going to say,” said the Dodo in an offended tone, “was, “But who is to give the prizes?” quite a chorus of voices asked. “Why, _she_, of course,” said the Dodo, pointing to Alice with one The next thing was to eat the comfits: this caused some noise and the others all joined in chorus, “Yes, please do!” but the Mouse only doesn’t suit my throat!” and a Canary called out in a trembling voice doing out here? Run home this moment, and fetch me a pair of gloves and at once in the direction it pointed to, without trying to explain the “How queer it seems,” Alice said to herself, “to be going messages for gloves, and was just going to leave the room, when her eye fell upon a she heard a voice outside, and stopped to listen. “Mary Ann! Mary Ann!” said the voice. “Fetch me my gloves this moment!” Next came an angry voice—the Rabbit’s—“Pat! Pat! Where are you?” And then a voice she had never heard before, “Sure then I’m here! Digging rumbling of little cartwheels, and the sound of a good many voices all then the Rabbit’s voice along—“Catch him, you by the hedge!” then silence, and then another confusion of voices—“Hold up his head—Brandy Last came a little feeble, squeaking voice, (“That’s Bill,” thought “We must burn the house down!” said the Rabbit’s voice; and Alice addressed her in a languid, sleepy voice. came different!” Alice replied in a very melancholy voice. By the use of this ointment—one shilling the box— going to dive in among the leaves, which she found to be nothing but Pigeon, raising its voice to a shriek, “and just as I was thinking I going to be, from one minute to another! However, I’ve got back to my are; secondly, because they’re making such a noise inside, no one could noise going on within—a constant howling and sneezing, and every now “Oh, _please_ mind what you’re doing!” cried Alice, jumping up and down (In which the cook and the baby joined): ear and left foot, so as to prevent its undoing itself,) she carried it No, there were no tears. “If you’re going to turn into a pig, my dear,” eyes, “Of course, of course; just what I was going to remark myself.” quarrelled last March—just before _he_ went mad, you know—” (pointing hoarse, feeble voice: “I heard every word you fellows were saying.” The Dormouse had closed its eyes by this time, and was going off into a neither of the others took the least notice of her going, though she voice, “Why the fact is, you see, Miss, this here ought to have been a you see, Miss, we’re doing our best, afore she comes, to—” At this “And who are _these?_” said the Queen, pointing to the three gardeners “Get up!” said the Queen, in a shrill, loud voice, and the three turning to the rose-tree, she went on, “What _have_ you been doing “May it please your Majesty,” said Two, in a very humble tone, going “Come on, then!” roared the Queen, and Alice joined the procession, “It’s—it’s a very fine day!” said a timid voice at her side. She was “Get to your places!” shouted the Queen in a voice of thunder, and had got its neck nicely straightened out, and was going to give the going to begin again, it was very provoking to find that the hedgehog “How do you like the Queen?” said the Cat in a low voice. “Who _are_ you talking to?” said the King, going up to Alice, and Alice thought she might as well go back, and see how the game was going on, as she heard the Queen’s voice in the distance, screaming with a large crowd collected round it: there was a dispute going on between thing before, and he wasn’t going to begin at _his_ time of life. startled when she heard her voice close to her ear. “You’re thinking “The game’s going on rather better now,” she said, by way of keeping up But here, to Alice’s great surprise, the Duchess’s voice died away, “A fine day, your Majesty!” the Duchess began in a low, weak voice. about half no time! Take your choice!” The Duchess took her choice, and was gone in a moment. As they walked off together, Alice heard the King say in a low voice, sea. The master was an old Turtle—we used to call him Tortoise—” “Why did you call him Tortoise, if he wasn’t one?” Alice asked. “We called him Tortoise because he taught us,” said the Mock Turtle Fainting in Coils.” two sobs choked his voice. “Same as if he had a bone in his throat,” the back. At last the Mock Turtle recovered his voice, and, with tears “Change lobsters again!” yelled the Gryphon at the top of its voice. Turtle, suddenly dropping his voice; and the two creatures, who had “There’s a porpoise close behind us, and he’s treading on my tail. They are waiting on the shingle—will you come and join the dance? Will you, won’t you, will you, won’t you, will you join the dance? Will you, won’t you, will you, won’t you, won’t you join the dance? Said he thanked the whiting kindly, but he would not join the dance. Would not, could not, would not, could not, would not join the dance. Would not, could not, would not, could not, could not join the dance. Then turn not pale, beloved snail, but come and join the dance. Will you, won’t you, will you, won’t you, will you join the dance? Will you, won’t you, will you, won’t you, won’t you join the dance?” “Boots and shoes under the sea,” the Gryphon went on in a deep voice, running on the song, “I’d have said to the porpoise, ‘Keep back, wise fish would go anywhere without a porpoise.” and told me he was going a journey, I should say ‘With what porpoise?’” Alice a little timidly: “but it’s no use going back to yesterday, “Stand up and repeat ‘’_Tis the voice of the sluggard_,’” said the “’Tis the voice of the Lobster; I heard him declare, His voice has a timid and tremulous sound.] wrong, and she went on in a trembling voice:— The Mock Turtle sighed deeply, and began, in a voice sometimes choked they doing?” Alice whispered to the Gryphon. “They can’t have anything “Stupid things!” Alice began in a loud, indignant voice, but she “I’m a poor man, your Majesty,” the Hatter began, in a trembling voice, The King looked anxiously at the White Rabbit, who said in a low voice, nearly out of sight, he said in a deep voice, “What are tarts made of?” “Treacle,” said a sleepy voice behind her. little voice, the name “Alice!” “The trial cannot proceed,” said the King in a very grave voice, “until verdict,” he said to the jury, in a low, trembling voice. “Why, there they are!” said the King triumphantly, pointing to the “Off with her head!” the Queen shouted at the top of her voice. Nobody looking up into hers—she could hear the very tones of her voice, and shared their never-ending meal, and the shrill voice of the Queen cries to the voice of the shepherd boy—and the sneeze of the baby, the shriek of the Gryphon, and all the other queer noises, would change unenforceability of any provision of this agreement shall not void the)
mini exit code =
bash exit code = 0
[0;33mTest  55: ❌[38;5;244m cat minishell.h | grep ");"$ 
[0mmini output = (0 cat 0 minishell.h 1 | 0 grep 0 );$)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  56: ❌[38;5;244m export GHOST=123 | env | grep GHOST 
[0mmini output = (0 export 0 GHOST=123 1 | 0 env 1 | 0 grep 0 GHOST)
bash output = ()
mini exit code =
bash exit code = 1
[0;31m———————————— redirects
[0;33mTest  57: ❌[38;5;244m grep hi <./test_files/infile 
[0mmini output = (0 grep 0 hi 2 < 0 ./test_files/infile)
bash output = (hi)
mini exit code =
bash exit code = 0
[0;33mTest  58: ❌[38;5;244m grep hi "<infile" <         ./test_files/infile 
[0mmini output = (0 grep 0 hi 0 <infile 2 < 0 ./test_files/infile)
bash output = ()
mini exit code =
bash exit code = 2
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  59: ❌[38;5;244m echo hi < ./test_files/infile bye bye 
[0mmini output = (0 echo 0 hi 2 < 0 ./test_files/infile 0 bye 0 bye)
bash output = (hi bye bye)
mini exit code =
bash exit code = 0
[0;33mTest  60: ❌[38;5;244m grep hi <./test_files/infile_big <./test_files/infile 
[0mmini output = (0 grep 0 hi 2 < 0 ./test_files/infile_big 2 < 0 ./test_files/infile)
bash output = (hi)
mini exit code =
bash exit code = 0
[0;33mTest  61: ❌[38;5;244m echo <"./test_files/infile" "bonjour       42" 
[0mmini output = (0 echo 2 < 0 ./test_files/infile 0 bonjour 42)
bash output = (bonjour 42)
mini exit code =
bash exit code = 0
[0;33mTest  62: ❌[38;5;244m cat <"./test_files/file name with spaces" 
[0mmini output = (0 cat 2 < 0 ./test_files/file name with spaces)
bash output = (😈 😈 😈 This will break your minishell 😈 😈 😈)
mini exit code =
bash exit code = 0
[0;33mTest  63: ❌[38;5;244m cat <./test_files/infile_big ./test_files/infile 
[0mmini output = (0 cat 2 < 0 ./test_files/infile_big 0 ./test_files/infile)
bash output = (hi hello world 42)
mini exit code =
bash exit code = 0
[0;33mTest  64: ❌[38;5;244m cat <"1""2""3""4""5" 
[0mmini output = (0 cat 2 < 0 12345)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  65: ❌[38;5;244m echo <"./test_files/infile" <missing <"./test_files/infile" 
[0mmini output = (0 echo 2 < 0 ./test_files/infile 2 < 0 missing 2 < 0 ./test_files/infile)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  66: ❌[38;5;244m echo <missing <"./test_files/infile" <missing 
[0mmini output = (0 echo 2 < 0 missing 2 < 0 ./test_files/infile 2 < 0 missing)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  67: ❌[38;5;244m cat <"./test_files/infile" 
[0mmini output = (0 cat 2 < 0 ./test_files/infile)
bash output = (hi hello world 42)
mini exit code =
bash exit code = 0
[0;33mTest  68: ❌[38;5;244m echo <"./test_files/infile_big" | cat <"./test_files/infile" 
[0mmini output = (0 echo 2 < 0 ./test_files/infile_big 1 | 0 cat 2 < 0 ./test_files/infile)
bash output = (hi hello world 42)
mini exit code =
bash exit code = 0
[0;33mTest  69: ❌[38;5;244m echo <"./test_files/infile_big" | cat "./test_files/infile" 
[0mmini output = (0 echo 2 < 0 ./test_files/infile_big 1 | 0 cat 0 ./test_files/infile)
bash output = (hi hello world 42)
mini exit code =
bash exit code = 0
[0;33mTest  70: ❌[38;5;244m echo <"./test_files/infile_big" | echo <"./test_files/infile" 
[0mmini output = (0 echo 2 < 0 ./test_files/infile_big 1 | 0 echo 2 < 0 ./test_files/infile)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  71: ❌[38;5;244m echo hi | cat <"./test_files/infile" 
[0mmini output = (0 echo 0 hi 1 | 0 cat 2 < 0 ./test_files/infile)
bash output = (hi hello world 42)
mini exit code =
bash exit code = 0
[0;33mTest  72: ❌[38;5;244m echo hi | cat "./test_files/infile" 
[0mmini output = (0 echo 0 hi 1 | 0 cat 0 ./test_files/infile)
bash output = (hi hello world 42)
mini exit code =
bash exit code = 0
[0;33mTest  73: ❌[38;5;244m cat <"./test_files/infile" | echo hi 
[0mmini output = (0 cat 2 < 0 ./test_files/infile 1 | 0 echo 0 hi)
bash output = (hi)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( Broken pipe)
[0;33mTest  74: ❌[38;5;244m cat <"./test_files/infile" | grep hello 
[0mmini output = (0 cat 2 < 0 ./test_files/infile 1 | 0 grep 0 hello)
bash output = (hello)
mini exit code =
bash exit code = 0
[0;33mTest  75: ❌[38;5;244m cat <"./test_files/infile_big" | echo hi 
[0mmini output = (0 cat 2 < 0 ./test_files/infile_big 1 | 0 echo 0 hi)
bash output = (hi)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( Broken pipe)
[0;33mTest  76: ❌[38;5;244m cat <missing 
[0mmini output = (0 cat 2 < 0 missing)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  77: ❌[38;5;244m cat <missing | cat 
[0mmini output = (0 cat 2 < 0 missing 1 | 0 cat)
bash output = ()
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  78: ❌[38;5;244m cat <missing | echo oi 
[0mmini output = (0 cat 2 < 0 missing 1 | 0 echo 0 oi)
bash output = (oi)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  79: ❌[38;5;244m cat <missing | cat <"./test_files/infile" 
[0mmini output = (0 cat 2 < 0 missing 1 | 0 cat 2 < 0 ./test_files/infile)
bash output = (hi hello world 42)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  80: ❌[38;5;244m echo <123 <456 hi | echo 42 
[0mmini output = (0 echo 2 < 0 123 2 < 0 456 0 hi 1 | 0 echo 0 42)
bash output = (42)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( No such file or directory)
[0;33mTest  81: ❌[38;5;244m ls >./outfiles/outfile01 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 3 > 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  82: ❌[38;5;244m ls >         ./outfiles/outfile01 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 3 > 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  83: ❌[38;5;244m echo hi >         ./outfiles/outfile01 bye 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
hi bye
mini output = (0 echo 0 hi 3 > 0 ./outfiles/outfile01 0 bye)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  84: ❌[38;5;244m ls >./outfiles/outfile01 >./outfiles/outfile02 
[0mOnly in ./bash_outfiles: outfile01
Only in ./bash_outfiles: outfile02
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 3 > 0 ./outfiles/outfile01 3 > 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  85: ❌[38;5;244m ls >./outfiles/outfile01 >./test_files/invalid_permission 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
mini output = (0 ls 3 > 0 ./outfiles/outfile01 3 > 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest  86: ❌[38;5;244m ls >"./outfiles/outfile with spaces" 
[0mOnly in ./bash_outfiles: outfile with spaces
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 3 > 0 ./outfiles/outfile with spaces)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  87: ❌[38;5;244m ls >"./outfiles/outfile""1""2""3""4""5" 
[0mOnly in ./bash_outfiles: outfile12345
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 3 > 0 ./outfiles/outfile12345)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  88: ❌[38;5;244m ls >"./outfiles/outfile01" >./test_files/invalid_permission >"./outfiles/outfile02" 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
mini output = (0 ls 3 > 0 ./outfiles/outfile01 3 > 0 ./test_files/invalid_permission 3 > 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest  89: ❌[38;5;244m ls >./test_files/invalid_permission >"./outfiles/outfile01" >./test_files/invalid_permission 
[0mmini output = (0 ls 3 > 0 ./test_files/invalid_permission 3 > 0 ./outfiles/outfile01 3 > 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest  90: ❌[38;5;244m cat <"./test_files/infile" >"./outfiles/outfile01" 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
hi
hello
world
42
mini output = (0 cat 2 < 0 ./test_files/infile 3 > 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  91: ❌[38;5;244m echo hi >./outfiles/outfile01 | echo bye 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
hi
mini output = (0 echo 0 hi 3 > 0 ./outfiles/outfile01 1 | 0 echo 0 bye)
bash output = (bye)
mini exit code =
bash exit code = 0
[0;33mTest  92: ❌[38;5;244m echo hi >./outfiles/outfile01 >./outfiles/outfile02 | echo bye 
[0mOnly in ./bash_outfiles: outfile01
Only in ./bash_outfiles: outfile02
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
hi
mini output = (0 echo 0 hi 3 > 0 ./outfiles/outfile01 3 > 0 ./outfiles/outfile02 1 | 0 echo 0 bye)
bash output = (bye)
mini exit code =
bash exit code = 0
[0;33mTest  93: ❌[38;5;244m echo hi | echo >./outfiles/outfile01 bye 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
bye
mini output = (0 echo 0 hi 1 | 0 echo 3 > 0 ./outfiles/outfile01 0 bye)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  94: ❌[38;5;244m echo hi | echo bye >./outfiles/outfile01 >./outfiles/outfile02 
[0mOnly in ./bash_outfiles: outfile01
Only in ./bash_outfiles: outfile02
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
bye
mini output = (0 echo 0 hi 1 | 0 echo 0 bye 3 > 0 ./outfiles/outfile01 3 > 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  95: ❌[38;5;244m echo hi >./outfiles/outfile01 | echo bye >./outfiles/outfile02 
[0mOnly in ./bash_outfiles: outfile01
Only in ./bash_outfiles: outfile02
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
hi
bye
mini output = (0 echo 0 hi 3 > 0 ./outfiles/outfile01 1 | 0 echo 0 bye 3 > 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest  96: ❌[38;5;244m echo hi >./outfiles/outfile01 >./test_files/invalid_permission | echo bye 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
mini output = (0 echo 0 hi 3 > 0 ./outfiles/outfile01 3 > 0 ./test_files/invalid_permission 1 | 0 echo 0 bye)
bash output = (bye)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( Permission denied)
[0;33mTest  97: ❌[38;5;244m echo hi >./test_files/invalid_permission | echo bye 
[0mmini output = (0 echo 0 hi 3 > 0 ./test_files/invalid_permission 1 | 0 echo 0 bye)
bash output = (bye)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( Permission denied)
[0;33mTest  98: ❌[38;5;244m echo hi >./test_files/invalid_permission >./outfiles/outfile01 | echo bye 
[0mmini output = (0 echo 0 hi 3 > 0 ./test_files/invalid_permission 3 > 0 ./outfiles/outfile01 1 | 0 echo 0 bye)
bash output = (bye)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( Permission denied)
[0;33mTest  99: ❌[38;5;244m echo hi | echo bye >./test_files/invalid_permission 
[0mmini output = (0 echo 0 hi 1 | 0 echo 0 bye 3 > 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 100: ❌[38;5;244m echo hi | >./outfiles/outfile01 echo bye >./test_files/invalid_permission 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
mini output = (0 echo 0 hi 1 | 3 > 0 ./outfiles/outfile01 0 echo 0 bye 3 > 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 101: ❌[38;5;244m echo hi | echo bye >./test_files/invalid_permission >./outfiles/outfile01 
[0mmini output = (0 echo 0 hi 1 | 0 echo 0 bye 3 > 0 ./test_files/invalid_permission 3 > 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 102: ❌[38;5;244m cat <"./test_files/infile" >./test_files/invalid_permission 
[0mmini output = (0 cat 2 < 0 ./test_files/infile 3 > 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 103: ❌[38;5;244m cat >./test_files/invalid_permission <"./test_files/infile" 
[0mmini output = (0 cat 3 > 0 ./test_files/invalid_permission 2 < 0 ./test_files/infile)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 104: ❌[38;5;244m cat <missing >./outfiles/outfile01 
[0mmini output = (0 cat 2 < 0 missing 3 > 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest 105: ❌[38;5;244m cat >./outfiles/outfile01 <missing 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
mini output = (0 cat 3 > 0 ./outfiles/outfile01 2 < 0 missing)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest 106: ❌[38;5;244m cat <missing >./test_files/invalid_permission 
[0mmini output = (0 cat 2 < 0 missing 3 > 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest 107: ❌[38;5;244m cat >./test_files/invalid_permission <missing 
[0mmini output = (0 cat 3 > 0 ./test_files/invalid_permission 2 < 0 missing)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 108: ❌[38;5;244m cat >./outfiles/outfile01 <missing >./test_files/invalid_permission 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
mini output = (0 cat 3 > 0 ./outfiles/outfile01 2 < 0 missing 3 > 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest 109: ❌[38;5;244m ls >>./outfiles/outfile01 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 4 >> 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest 110: ❌[38;5;244m ls >>      ./outfiles/outfile01 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 4 >> 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest 111: ❌[38;5;244m ls >>./outfiles/outfile01 >./outfiles/outfile01 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 4 >> 0 ./outfiles/outfile01 3 > 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest 112: ❌[38;5;244m ls >./outfiles/outfile01 >>./outfiles/outfile01 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 3 > 0 ./outfiles/outfile01 4 >> 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest 113: ❌[38;5;244m ls >./outfiles/outfile01 >>./outfiles/outfile01 >./outfiles/outfile02 
[0mOnly in ./bash_outfiles: outfile01
Only in ./bash_outfiles: outfile02
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 3 > 0 ./outfiles/outfile01 4 >> 0 ./outfiles/outfile01 3 > 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest 114: ❌[38;5;244m ls >>./outfiles/outfile01 >>./outfiles/outfile02 
[0mOnly in ./bash_outfiles: outfile01
Only in ./bash_outfiles: outfile02
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
README.md
bash.supp
bash_outfiles
bonus
bonus_bonus
builtins
extras
local.supp
loop.out
manual_tests
mini_outfiles
os_specific
outfiles
pipes
redirects
syntax
test_files
test_output.txt
test_report.md
tester
wildcards
mini output = (0 ls 4 >> 0 ./outfiles/outfile01 4 >> 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest 115: ❌[38;5;244m ls >>./test_files/invalid_permission 
[0mmini output = (0 ls 4 >> 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 116: ❌[38;5;244m ls >>./test_files/invalid_permission >>./outfiles/outfile01 
[0mmini output = (0 ls 4 >> 0 ./test_files/invalid_permission 4 >> 0 ./outfiles/outfile01)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 117: ❌[38;5;244m ls >>./outfiles/outfile01 >>./test_files/invalid_permission 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
mini output = (0 ls 4 >> 0 ./outfiles/outfile01 4 >> 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 118: ❌[38;5;244m ls >./outfiles/outfile01 >>./test_files/invalid_permission >>./outfiles/outfile02 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
mini output = (0 ls 3 > 0 ./outfiles/outfile01 4 >> 0 ./test_files/invalid_permission 4 >> 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 119: ❌[38;5;244m ls <missing >>./test_files/invalid_permission >>./outfiles/outfile02 
[0mmini output = (0 ls 2 < 0 missing 4 >> 0 ./test_files/invalid_permission 4 >> 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest 120: ❌[38;5;244m ls >>./test_files/invalid_permission >>./outfiles/outfile02 <missing 
[0mmini output = (0 ls 4 >> 0 ./test_files/invalid_permission 4 >> 0 ./outfiles/outfile02 2 < 0 missing)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 121: ❌[38;5;244m echo hi >>./outfiles/outfile01 | echo bye 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
hi
mini output = (0 echo 0 hi 4 >> 0 ./outfiles/outfile01 1 | 0 echo 0 bye)
bash output = (bye)
mini exit code =
bash exit code = 0
[0;33mTest 122: ❌[38;5;244m echo hi >>./outfiles/outfile01 >>./outfiles/outfile02 | echo bye 
[0mOnly in ./bash_outfiles: outfile01
Only in ./bash_outfiles: outfile02
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
hi
mini output = (0 echo 0 hi 4 >> 0 ./outfiles/outfile01 4 >> 0 ./outfiles/outfile02 1 | 0 echo 0 bye)
bash output = (bye)
mini exit code =
bash exit code = 0
[0;33mTest 123: ❌[38;5;244m echo hi | echo >>./outfiles/outfile01 bye 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
bye
mini output = (0 echo 0 hi 1 | 0 echo 4 >> 0 ./outfiles/outfile01 0 bye)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest 124: ❌[38;5;244m echo hi | echo bye >>./outfiles/outfile01 >>./outfiles/outfile02 
[0mOnly in ./bash_outfiles: outfile01
Only in ./bash_outfiles: outfile02
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
bye
mini output = (0 echo 0 hi 1 | 0 echo 0 bye 4 >> 0 ./outfiles/outfile01 4 >> 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest 125: ❌[38;5;244m echo hi >>./outfiles/outfile01 | echo bye >>./outfiles/outfile02 
[0mOnly in ./bash_outfiles: outfile01
Only in ./bash_outfiles: outfile02
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
hi
bye
mini output = (0 echo 0 hi 4 >> 0 ./outfiles/outfile01 1 | 0 echo 0 bye 4 >> 0 ./outfiles/outfile02)
bash output = ()
mini exit code =
bash exit code = 0
[0;33mTest 126: ❌[38;5;244m echo hi >>./test_files/invalid_permission | echo bye 
[0mmini output = (0 echo 0 hi 4 >> 0 ./test_files/invalid_permission 1 | 0 echo 0 bye)
bash output = (bye)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( Permission denied)
[0;33mTest 127: ❌[38;5;244m echo hi >>./test_files/invalid_permission >./outfiles/outfile01 | echo bye 
[0mmini output = (0 echo 0 hi 4 >> 0 ./test_files/invalid_permission 3 > 0 ./outfiles/outfile01 1 | 0 echo 0 bye)
bash output = (bye)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( Permission denied)
[0;33mTest 128: ❌[38;5;244m echo hi | echo bye >>./test_files/invalid_permission 
[0mmini output = (0 echo 0 hi 1 | 0 echo 0 bye 4 >> 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 129: ❌[38;5;244m echo hi | echo >>./outfiles/outfile01 bye >./test_files/invalid_permission 
[0mOnly in ./bash_outfiles: outfile01
mini outfiles:
cat: './mini_outfiles/*': No such file or directory
bash outfiles:
mini output = (0 echo 0 hi 1 | 0 echo 4 >> 0 ./outfiles/outfile01 0 bye 3 > 0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( Permission denied)
[0;33mTest 130: ❌[38;5;244m cat <minishell.h>./outfiles/outfile 
[0mmini output = (0 cat 2 < 0 minishell.h 3 > 0 ./outfiles/outfile)
bash output = ()
mini exit code =
bash exit code = 1
mini error = ()
bash error = ( No such file or directory)
[0;33mTest 131: ❌[38;5;244m cat <minishell.h|ls 
[0mmini output = (0 cat 2 < 0 minishell.h 1 | 0 ls)
bash output = (README.md bash.supp bash_outfiles bonus bonus_bonus builtins extras local.supp loop.out manual_tests mini_outfiles os_specific outfiles pipes redirects syntax test_files test_output.txt test_report.md tester wildcards)
mini exit code =
bash exit code = 0
mini error = ()
bash error = ( No such file or directory)
[0;31m———————————— extras
[0;33mTest 132: ❌[38;5;244m  
[0mmini exit code =
bash exit code = 0
[0;33mTest 133: ❌[38;5;244m $PWD 
[0mmini output = (0 /home/runner/work/minishell/minishell/minishell_tester)
bash output = ()
mini exit code =
bash exit code = 126
mini error = ()
bash error = ( Is a directory)
[0;33mTest 134: ❌[38;5;244m $EMPTY 
[0mmini exit code =
bash exit code = 0
[0;33mTest 135: ❌[38;5;244m $EMPTY echo hi 
[0mmini output = (0 echo 0 hi)
bash output = (hi)
mini exit code =
bash exit code = 0
[0;33mTest 136: ❌[38;5;244m ./test_files/invalid_permission 
[0mmini output = (0 ./test_files/invalid_permission)
bash output = ()
mini exit code =
bash exit code = 126
mini error = ()
bash error = ( Permission denied)
[0;33mTest 137: ❌[38;5;244m ./missing.out 
[0mmini output = (0 ./missing.out)
bash output = ()
mini exit code =
bash exit code = 127
mini error = ()
bash error = ( No such file or directory)
[0;33mTest 138: ❌[38;5;244m missing.out 
[0mmini output = (0 missing.out)
bash output = ()
mini exit code =
bash exit code = 127
mini error = ()
bash error = ( command not found)
[0;33mTest 139: ❌[38;5;244m "aaa" 
[0mmini output = (0 aaa)
bash output = ()
mini exit code =
bash exit code = 127
mini error = ()
bash error = ( command not found)
[0;33mTest 140: ❌[38;5;244m test_files 
[0mmini output = (0 test_files)
bash output = ()
mini exit code =
bash exit code = 127
mini error = ()
bash error = ( command not found)
[0;33mTest 141: ❌[38;5;244m ./test_files 
[0mmini output = (0 ./test_files)
bash output = ()
mini exit code =
bash exit code = 126
mini error = ()
bash error = ( Is a directory)
[0;33mTest 142: ❌[38;5;244m /test_files 
[0mmini output = (0 /test_files)
bash output = ()
mini exit code =
bash exit code = 127
mini error = ()
bash error = ( No such file or directory)
[0;33mTest 143: ❌[38;5;244m minishell.h 
[0mmini output = (0 minishell.h)
bash output = ()
mini exit code =
bash exit code = 127
mini error = ()
bash error = ( command not found)
[0;33mTest 144: ❌[38;5;244m $ 
[0mmini output = (0 $)
bash output = ()
mini exit code =
bash exit code = 127
mini error = ()
bash error = ( command not found)
[0;33mTest 145: ❌[38;5;244m $? 
[0mmini output = (0 0)
bash output = ()
mini exit code =
bash exit code = 127
mini error = ()
bash error = ( command not found)
[0;33mTest 146: ❌[38;5;244m README.md 
[0mmini output = (0 README.md)
bash output = ()
mini exit code =
bash exit code = 127
mini error = ()
bash error = ( command not found)
[0;35m[1m/146
[0m😭 😭 😭
```

### Summary
- ✅ Passed: 0
- ❌ Failed: 146
- 📊 Total: 146
