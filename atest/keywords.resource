*** Settings ***
Documentation    This is a resource file, that contain variables and keywords.


*** Variables ***
${process_command}    ./build/bin/command-manager
${socket_0}           /tmp/9Lq7BNBnBycd6nxy.0.socket
${socket_1}           /tmp/9Lq7BNBnBycd6nxy.1.socket


*** Keywords ***
Test Setup
    Log                            Test Setup
    Remove Directory    ./atest-results/tmp/    recursive=True
    Create Directory    ./atest-results/tmp/

Test Teardown
    Terminate All Processes    kill=true

Get ${process} stdout file
    ${error_message}    Get File          ./atest-results/tmp/${process}stdout.txt
                        Should Not Be Empty    ${error_message}
                        RETURN                 ${error_message}