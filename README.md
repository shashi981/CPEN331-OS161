# 🖥️ OS161 Operating System Implementation  

This repository contains an implementation of an operating system developed as part of the [CPEN 331 Operating Systems course](https://sites.google.com/site/cpen331/) at the **University of British Columbia**.

---

## 🔎 Overview  

This project serves as a hands-on exploration of operating system fundamentals. Through the implementation of core OS concepts, the project reinforces learning in areas such as concurrency, synchronization, virtual memory management, system calls, file systems, and process scheduling.  

Key topics covered include:  
- Process synchronization and concurrency  
- Memory management and virtual memory  
- System call handling  
- Filesystem interactions  
- Device drivers and networking basics  

The goal was to implement foundational components to enhance understanding of operating system design and structure.

---

## 🚀 Implemented Features  

### 1. **Concurrency Mechanisms**  

#### 🔒 Mutex Locks  
Mutex locks are synchronization primitives designed to allow exclusive access to a shared resource by a single thread at any given moment. They utilize spinlocks and a waiting channel to manage thread access. When a thread attempts to acquire a mutex lock, it will spin until the lock becomes available or sleep on the waiting channel if it's already in use.

- **Code Reference**: [`/kern/thread/synch.c`](#)  

---

#### 🛌 Condition Variables  
Condition variables are essential for thread synchronization, allowing threads to wait until a specific condition is met before proceeding. They support safe concurrency by ensuring that the act of sleeping and waking up is atomic.

- **Code Reference**: [`/kern/thread/synch.c`](#)

---

### 2. **Filesystem System Calls**  

The following system calls have been implemented to interact with the filesystem:  

- `open()` - Opens a file  
- `read()` - Reads from a file  
- `write()` - Writes data to a file  
- `lseek()` - Changes the file offset  
- `close()` - Closes an open file descriptor  
- `dup2()` - Duplicates file descriptors  
- `chdir()` - Changes the current working directory  
- `__getcwd()` - Retrieves the current working directory path  

- **Code Reference**: [`/kern/syscall/fsyscall.c`](#)

---

### 3. **Process System Calls**  

The following system calls were implemented for process management:  

- `getpid()` - Retrieves the process ID of the current process  
- `fork()` - Creates a child process  
- `execv()` - Replaces the current process image with a new one  
- `_exit()` - Terminates the current process  
- `kill_curthread()` - Terminates the currently running thread (to be expanded)  
- `sbrk()` - Adjusts program data segment size (to be expanded)  

- **Code Reference**: [`/kern/syscall/psyscall.c`](#)

---

### 4. **Virtual Memory**  

OS161 integrates virtual memory support by leveraging mechanisms like Translation Lookaside Buffers (TLB), page tables, and Memory Management Units (MMU). Additional features such as swapping have also been explored.

---

## ⚙️ Configuration Instructions  

### Installation Requirements  

To run and configure OS161, the following libraries are required:  
- [bmake](http://crufty.net/help/sjg/bmake.html)  
- [libmpc](http://www.multiprecision.org/index.php?prog=mpc)  
- [gmp](https://gmplib.org/)  
- [wget](https://www.gnu.org/software/wget/)

#### On Linux:  
```bash
sudo apt-get -y install bmake ncurses-dev libmpc-dev


## ⚙️ Setup & Compilation  

Assuming **OS161** is installed at `~/os161`, follow the steps below to configure and compile the kernel:

---

### 🛠️ **Step 1: Configure the Build**  

Run the following commands to configure the build:

```bash
cd ~/os161/src
./configure --ostree=$HOME/os161/root

### 🛠️ **Step 2: Build Userland**

Use **bmake** to build userland components:

```bash
bmake
bmake install

### 🛠️ **Step 3: Configure the Kernel**

```bash
cd kern/conf
./config DUMBVM

### 🛠️ **Step 4: Compile and Install the Kernel**

```bash
cd ~/os161/src/kern/compile/DUMBVM
bmake depend
bmake
bmake install


# 📊 Features Summary  

---

## ✅ Concurrency Mechanisms  
Implemented **Mutex Locks** and **Condition Variables** for managing thread-safe operations and process synchronization.  

---

## 🖥️ Filesystem System Calls  
System calls implemented for common filesystem operations:  
- `open()`  
- `read()`  
- `write()`  
- `close()`  
- `chdir()`  
- `lseek()`  

---

## 🔄 Process Management  
System calls to support process lifecycle management:  
- `fork()`  
- `execv()`  
- `getpid()`  
- `_exit()`  

---

## 🗂️ Virtual Memory Support  
Integrated virtual memory strategies with TLB handling, page table support, and swap management.  


