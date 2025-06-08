*** Settings ***
Documentation    A test suite for parsing the arguments using when starting the command manager
...              Will be used for acceptance tests

Library          OperatingSystem
Library          Process
Resource         keywords.resource

Test Setup       Test Setup
Test Teardown    Test Teardown


*** Test Cases ***
Executable Has Help Option
    Given command-manager is available
    When user launchs it with "--help" option
    Then help message is printed
    and command-manager will return

Executable Has Version Option
    Given command-manager is available
    When user launchs it with "--version" option
    Then version message is printed
    and command-manager will return

Executable Has Usage Option
    Given command-manager is available
    When user launchs it with "--usage" option
    Then usage message is printed
    and command-manager will return

Executable Does Not Run And Shows Help
    Given command-manager is available
    When user launchs it with "/tmp/9Lq7BNBnBycd6nxy.0.socket" option
    Then error message is printed
    and command-manager will return

Executable Run And Shows Command Prompt
    Given command-manager is available
    When user launchs it with required params
    Then prompt is printed


*** Keywords ***
Command-manager is available
    File Should Exist    ${process_command}

User launchs it with required params
    Start Process    ${process_command}    /tmp/9Lq7BNBnBycd6nxy.0.socket    /tmp/9Lq7BNBnBycd6nxy.1.socket    alias=master_process    stdout=./atest-results/tmp/stdout.txt    stderr=./atest-results/tmp/stdout.txt

User launchs it with "${option}" option
    Start Process    ${process_command}    ${option}    alias=master_process    stdout=./atest-results/tmp/stdout.txt    stderr=./atest-results/tmp/stdout.txt

Command-manager will return
    Wait For Process             handle=master_process    timeout=300ms
    Process Should Be Stopped    handle=master_process

Help message is printed
    ${help_message}    Wait Until Keyword Succeeds    3x    200ms    Get stdout File
                       Should Contain    ${help_message}    Usage: command-manager [OPTION...] local-socket-path remote-socket-path
                       Should Contain    ${help_message}    command-manager send receives messages using a unix socket
                       Should Contain    ${help_message}    -?, --help Give this help list    collapse_spaces=True
                       Should Contain    ${help_message}    --usage Give a short usage message    collapse_spaces=True
                       Should Contain    ${help_message}    -V, --version Print program version    collapse_spaces=True
                       Should Contain    ${help_message}    Report bugs to <traquinedes@yahoo.es>.    collapse_spaces=True

Version message is printed
    ${version_message}    Wait Until Keyword Succeeds    3x    200ms    Get stdout File
                          Should Contain    ${version_message}    command-manager 1.0

Usage message is printed
    ${usage_message}    Wait Until Keyword Succeeds    3x    200ms    Get stdout File
                        Should Contain    ${usage_message}    Usage: command-manager [-?V] [--help] [--usage] [--version]
                        Should Contain    ${usage_message}    local-socket-path remote-socket-path

Error message is printed
    ${error_message}    Wait Until Keyword Succeeds    3x    200ms    Get stdout File
                        Should Contain    ${error_message}    Usage: command-manager [OPTION...] local-socket-path remote-socket-path
                        Should Contain    ${error_message}    Try `command-manager --help' or `command-manager --usage' for more
                        Should Contain    ${error_message}    information.

Prompt is printed
    ${prompt_message}    Wait Until Keyword Succeeds    3x    200ms    Get stdout File
                         Wait Until Keyword Succeeds    3x    200ms    Should Contain    ${prompt_message}    $

Test Setup
    Remove Directory    ./atest-results/tmp/    recursive=True
    Create Directory    ./atest-results/tmp/

Test Teardown
    Terminate All Processes       kill=true
    Run Keyword If Test Failed    Log Process stdout

Log Process stdout
    ${stdout}    Get File    ./atest-results/tmp/stdout.txt
                 Log         ${stdout}

Get stdout file
    ${error_message}    Get File          ./atest-results/tmp/stdout.txt
                        Should Not Be Empty    ${error_message}
                        RETURN                 ${error_message}
