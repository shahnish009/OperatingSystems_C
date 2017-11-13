# OperatingSystems_C

# By using this code, you agree to appropriate licences

# This code is a part of academic project and is for illustration purposes only

## Creating and Destroying Process Hierarchy using Linux System Calls - Linux / C (Google Cloud Platform)
- Program to recursively create a process hierarchy tree with positive height where each child process on each height have C children each
- Systematically terminate all the processes in the tree. 
- This project is implemented using Linux system calls on Google Cloud Platform (GCP).

## Character Device Kernel Module - Linux / C (Google Cloud Platform)
- Implementation of a kernel module that creates character device that supports the read() operation
- When the read() system call is invoked on the character device from a user space process, the kernel module returns the information of all currently running processes (process ID, parent process ID, the CPU on which the process is running, its current state)

## Named Pipe Kernel Module - Linux / C (Google Cloud Platform)
- Kernel-level pipe (Character Device) for Producer-Consumer problem using Semaphores and Mutexes
- Producers write and Consumers read lines from this pipe
- Producers block when the pipe is full and Consumers block when pipe is empty

## JProbe - probe the process - Linux / C (Google Cloud Platform)
- Probe the process using JProbe
- Implementation/usage of JProbe
