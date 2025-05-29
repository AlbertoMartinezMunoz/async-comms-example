*** Settings ***
Documentation     A test suite for send and receive commands between two different processes at the same time.
...               Will be used for acceptance tests


*** Test Cases ***
Simple Test
    Hello World


*** Keywords ***
Hello World
    Log 	Hello, world! 			# Normal INFO message.