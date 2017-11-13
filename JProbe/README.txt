COMPILE:
make

INSERT MODULE:
sudo insmod jprobe.ko process_id=<pid>

GET BACK THE ADDRESSES OF PROCESS BY USER PROGRAM:
gcc user.c -o user
sudo ./user

REMOVE MODULE:
sudo rmmod jprobe

CLEAN THE EXECUTABLES AND OBJECT FILES:
make clean
