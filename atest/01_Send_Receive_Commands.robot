*** Settings ***
Documentation    A test suite for send and receive commands between two different processes at the same time.
...              Will be used for acceptance tests

Library          OperatingSystem
Library          Process
Library          String
Resource         keywords.resource

Test Setup       Test Setup
Test Teardown    Test Teardown


*** Test Cases ***
Shutdown Command Stops Sender and Receiver
    Given receiver command-manager is running
    and sender command-manager is running
    When sender command-manager executes "D" command
    Then receiver command-manager will return
    and sender command-manager will return

Fast Command Is Received and ACK is responded
    Given receiver command-manager is running
    and sender command-manager is running
    When sender command-manager executes "F" command
    Then receiver command-manager will receive fast command
    and sender command-manager will receive ack

Slow Command Is Received and ACK is responded
    Given receiver command-manager is running
    and sender command-manager is running
    When sender command-manager executes "S" command
    Then receiver command-manager will receive slow command
    and sender command-manager will receive ack


*** Keywords ***
${process} command-manager is running
    Is Process Running    ${process}_process
    Get ${process} stdout file

Receiver command-manager will receive ${command} command
    ${stdout}    Get File          ./atest-results/tmp/receiver-stdout.txt
                 Should Contain    ${stdout}    ********************** Received ${command} Command **********************    ignore_case=True

Sender command-manager will receive ${ack}
    ${stdout}    Get File          ./atest-results/tmp/sender-stdout.txt
                 Should Contain    ${stdout}    : ${ack} received    ignore_case=True

Test Setup
    Remove tmp Files
    Create File         ./atest-results/tmp/receiver-stdin.txt
    Create File         ./atest-results/tmp/sender-stdin.txt
    Start Process       ${process_command}    /tmp/9Lq7BNBnBycd6nxy.0.socket1    /tmp/9Lq7BNBnBycd6nxy.1.socket1    alias=receiver_process    stdout=./atest-results/tmp/receiver-stdout.txt    stderr=./atest-results/tmp/receiver-stdout.txt    stdin=./atest-results/tmp/receiver-stdin.txt
    Start Process       ${process_command}    /tmp/9Lq7BNBnBycd6nxy.1.socket1    /tmp/9Lq7BNBnBycd6nxy.0.socket1   alias=sender_process    stdout=./atest-results/tmp/sender-stdout.txt    stderr=./atest-results/tmp/sender-stdout.txt    stdin=./atest-results/tmp/sender-stdin.txt

Test Teardown
    Sender command-manager executes "D" command
    Receiver command-manager executes "D" command
    Wait For Process              handle=receiver_process    timeout=500ms
    Wait For Process              handle=sender_process    timeout=500ms
    Terminate All Processes       kill=true
    Run Keyword If Test Failed    Log Process stdout

Log Process stdout
    ${stdout}    Get File    ./atest-results/tmp/receiver-stdout.txt
                 Log         ${stdout}
    ${stdout}    Get File    ./atest-results/tmp/sender-stdout.txt
                 Log         ${stdout}
    ${stdin}     Get File    ./atest-results/tmp/receiver-stdin.txt
                 Log         ${stdin}
    ${stdin}     Get File    ./atest-results/tmp/sender-stdin.txt
                 Log         ${stdin}


