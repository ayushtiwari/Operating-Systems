# Virtual Scheduler
Round robin thread scheduler.
<br/>
The assigment requirements seemed very loose. Some assumptions have been made.
<br/>
Tested OK on Darwin 14.5.0 (Mac OS Yosemite) with gcc 7.0.2 AND linux 3.10.0 with gcc 4.8.5

### Getting started
I have tried to build it to simulate a standalone scheduler as much as possible. The scheduler is interrupt driven. CPU time keeping is done by threads themselves. I had planned to use timer driven interrupts as well but there is no time. Since this is a virtual scheduler which runs at the top of Operating System scheduler, sometimes the program will be at at the mercy of the OS. Approximations and occassional usleeps have been used to handle such cases. Therefore, the code might fail to work. In such a case tuning the approximations might help.

### Demo
<p align="center">
<img src="https://github.com/ayushtiwari/Operating-Systems-IITKgp/blob/master/Virtual%20Scheduler/demo.gif" />
</p>

### Usage
Compilation
```
$ gcc scheduler.c -lpthread
```
Normal Execution
```
$ ./a.out
```
Verbose Execution
```
$ ./a.out -v
```
