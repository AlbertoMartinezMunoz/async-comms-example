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
    and master command-manager will return

Executable Has Version Option
    Given command-manager is available
    When user launchs it with "--version" option
    Then version message is printed
    and master command-manager will return

Executable Has Usage Option
    Given command-manager is available
    When user launchs it with "--usage" option
    Then usage message is printed
    and master command-manager will return

Executable Does Not Run And Shows Help
    Given command-manager is available
    When user launchs it with "/tmp/9Lq7BNBnBycd6nxy.0.socket" option
    Then error message is printed
    and master command-manager will return

Executable Run And Shows Command Prompt
    Given command-manager is available
    When user launchs it with required params
    Then prompt is printed


*** Keywords ***
Command-manager is available
    File Should Exist    ${process_command}

User launchs it with required params
    Start master process

User launchs it with "${option}" option
    Start Process    ${process_command}    ${option}    alias=master_process    stdout=./atest-results/tmp/master-stdout.txt    stderr=./atest-results/tmp/master-stdout.txt    stdin=./atest-results/tmp/master-stdin.txt

Help message is printed
    ${help_message}    Wait Until Keyword Succeeds    3x    200ms    Get master stdout File
                       Should Contain    ${help_message}    Usage: command-manager [OPTION...] local-socket-path remote-socket-path
                       Should Contain    ${help_message}    command-manager send receives messages using a unix socket
                       Should Contain    ${help_message}    -?, --help Give this help list    collapse_spaces=True
                       Should Contain    ${help_message}    --usage Give a short usage message    collapse_spaces=True
                       Should Contain    ${help_message}    -V, --version Print program version    collapse_spaces=True
                       Should Contain    ${help_message}    Report bugs to <traquinedes@yahoo.es>.    collapse_spaces=True

Version message is printed
    ${version_message}    Wait Until Keyword Succeeds    3x    200ms    Get master stdout File
                          Should Contain    ${version_message}    command-manager 1.0

Usage message is printed
    ${usage_message}    Wait Until Keyword Succeeds    3x    200ms    Get master stdout File
                        Should Contain    ${usage_message}    Usage: command-manager [-?V] [--help] [--usage] [--version]
                        Should Contain    ${usage_message}    local-socket-path remote-socket-path

Error message is printed
    ${error_message}    Wait Until Keyword Succeeds    3x    200ms    Get master stdout File
                        Should Contain    ${error_message}    Usage: command-manager [OPTION...] local-socket-path remote-socket-path
                        Should Contain    ${error_message}    Try `command-manager --help' or `command-manager --usage' for more
                        Should Contain    ${error_message}    information.

Prompt is printed
    ${prompt_message}    Wait Until Keyword Succeeds    3x    200ms    Get master stdout File
                         Wait Until Keyword Succeeds    3x    200ms    Should Contain    ${prompt_message}    $

Test Setup
    Remove tmp Files
    Create File         ./atest-results/tmp/master-stdin.txt

Test Teardown
    master command-manager executes "D" command
    Wait For Process           handle=master_process    timeout=500ms
    Terminate All Processes    kill=true
    Log Process stdout

Log Process stdout
    ${stdout}    Get File    ./atest-results/tmp/master-stdout.txt
                 Log         ${stdout}
