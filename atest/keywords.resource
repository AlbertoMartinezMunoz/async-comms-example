*** Settings ***
Documentation    This is a resource file, that contain variables and keywords.
Library          OperatingSystem
Library          Process
Library          String


*** Variables ***
${process_command}    ./build/bin/command-manager
${socket_0}           /tmp/9Lq7BNBnBycd6nxy.0.socket
${socket_1}           /tmp/9Lq7BNBnBycd6nxy.1.socket


*** Keywords ***
Remove tmp Files
    Remove Directory    ./atest-results/tmp/    recursive=True
    Create Directory    ./atest-results/tmp/

Test Teardown
    Terminate All Processes    kill=true

Get ${process} stdout file
    ${error_message}    Get File          ./atest-results/tmp/${process}-stdout.txt
                        Should Not Be Empty    ${error_message}
                        RETURN                 ${error_message}

Start ${process} process
    Start Process       ${process_command}    /tmp/9Lq7BNBnBycd6nxy.0.socket1    /tmp/9Lq7BNBnBycd6nxy.1.socket1    alias=${process}_process    stdout=./atest-results/tmp/${process}-stdout.txt    stderr=./atest-results/tmp/${process}-stdout.txt    stdin=./atest-results/tmp/${process}-stdin.txt

${process} command-manager executes "${command}" command
    ${process}    Convert To Lower Case    ${process}
                  Append To File           ./atest-results/tmp/${process}-stdin.txt    ${command}${\n}

${process} Command-manager will return
    Wait For Process             handle=${process}_process    timeout=300ms
    Process Should Be Stopped    handle=${process}_process
