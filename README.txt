Jonathan Kelaty
CSCI Operating Systems Home Project
2020-04-14

To compile my program:

    make all

Then, to run:

    ./main

The program will ask for input for RAM size and number of hard disks to
simulate. Then, user can enter the following commands:

A <#>  - Create common process of size #
AR <#> - Create real time process of size #
Q      - End time slice for currently running process
t      - Terminate currently running process
d <#>  - Send currently running process to HDD #
D <#>  - Send process being served by HDD # back to ready-queue
S r    - Snapshot of CPU and its ready-queues
S i    - Snapshot of IO devices and their IO-queues
S m    - Snapshot of RAM

Files Included:
    main.cpp
    OS.*
    CPU.h
    RAM.h
    HDD.h
    Process.h
    DataTypes.h
