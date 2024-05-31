# 思路总览

- [Fuzz Loop的实现](#fuzz-loop的实现)
    - [Fuzz_main和`ForkServer()`的通信](#fuzz_main和forkserver的通信)
    - [Fuzz_main接收被测程序的反馈](#fuzz_main接收被测程序的反馈)
    - [被测程序输入的重定向](#被测程序输入的重定向)
- [代码覆盖度的选择与实现](#代码覆盖度的选择与实现)
    - [基本块的唯一标识、分支的维护](#基本块的唯一标识分支的维护)
    - [分支覆盖度的计算、分支的权重设计](#分支覆盖度的计算分支的权重设计)
- [种子的维护和变异](#种子的维护和变异)
    - [`Seed`、`SeedPool`和`SeedManage`](#seedseedpool和seedmanage)
    - [种子与测试样例的对应](#种子与测试样例的对应)
    - [种子的变异方法及实现](#种子的变异方法及实现)
- [并行优化](#并行优化)
    - [`ForkServer()`](#forkserver)
    - [轮内并行、轮间串行](#轮内并行轮间串行)

## Fuzz Loop的实现

### Fuzz_main和`ForkServer()`的通信

Fuzz_main和`ForkServer()`主要通过**有名管道**(named pipe)进行通信，共涉及两个管道：
- `data_pipe`：Fuzz_main通过`data_pipe`，向`ForkServer()`通过`fork()`创建的子进程传输测试样例**文件路径**。
- `inst_pipe`：Fuzz_main通过`inst_pipe`，控制`ForkServer()`的继续运行、暂停等待和终止退出（`CONTINUE_INST`、`WAIT_INST`、`EXIT_INST`）。

### Fuzz_main接收被测程序的反馈

Fuzz_main主要通过**共享内存**(shared memory)，来获取被测程序的运行时信息反馈（被测程序的程序进程号、执行时间、覆盖度等）。


### 被测程序输入的重定向

被测程序通过`fork()`创建的子进程通过`data_pipe`，得到测试样例的**文件路径**，后将该测试样例文件重定向当前子程序的标准输入（`STDIN`）。


## 代码覆盖度的选择与实现

### 基本块的唯一标识、分支的维护

在静态插桩中，为每个函数的每个基本块分配一个**16**位（定义为`FSHM_MAX_ITEM_POW2`）的随机的唯一标识符，该唯一标识符在编译时即确定。

采用哈希表的方式维护分支。在一块大小为`sizeof(unsigned int)*(1 << FSHM_MAX_ITEM_POW2)`区域（共享内存）`bitmap`内，记录被测程序输入一个测试样例后的分支信息。若存在基本块`b1`到基本块`b2`的分支，则`bitmap[ (address(b1) << 1) ^ address(b2) ] ++`。


### 分支覆盖度的计算、分支的权重设计

在Fuzz_main中维护一份`bitmap`的“总和”，即该`bitmap`位所有子程序运行完成后反馈的`bitmap`的算术总和。

Fuzz_main对每一个子程序反馈的`bitmap`采取加权求和的方式来计算分支覆盖度，即：若为**未出现的分支**，则以**新分支增益**（`NEW_BRANCH_BONUS`）作为权重计算覆盖度；若为**已出现的分支**，则使用该分支当前的**已出现次数的总和的倒数**作为权重计算覆盖度。

Fuzz_main对该总和`bitmap`采取**每一轮**更新一次的策略，因此当前方法下，分支覆盖度（包括基于它计算的种子分数）仅在**同一轮**下具有可比性（或在足够多的轮数后具有轮间的可比性）。


## 种子的维护和变异

### `Seed`、`SeedPool`和`SeedManage`

一个种子（`Seed`）由一个32位的随机哈希值（`_hash`）唯一标识，作为一个子程序**测试样例生成和反馈记录**的单元。

一个种子池（`SeedPool`）维护全体种子的哈希值（确保无冲突），用于根据（用户提供的）初始测试样例**创建新种子**（`NewSeed()`）、或根据基种子**生成变异种子**（`Mutate()`）。

一个种子管理器（`SeedManage`，或称种子队列）为一组种子的先进先出序列，根据种子的优先级（`_rank`）来为种子排序，并通过`Push()`、`Pop()`、`Trim()`等方法实现**种子入队、出队和长度控制**。


### 种子与测试样例的对应

一个种子（`Seed s`）对应一个测试样例（`s._base_test`），具体而言，**一个种子存储一个测试样例文件的路径**。该测试样例的运行时信息将记录于`s._rinfo`中，该测试样例的分数将记录于`s._score`中。

种子的`_score`将作为基于该种子变异后的新种子的`_rank`，某一种子的`_rank`来自于它的基种子的`_score`。约定由（用户提供的）初始测试样例创建的新种子（即由`NewSeed()`方法得到的种子而非`Mutate()`方法）的`_rank`为正无穷大（`DBL_MAX`），以确保**初始样例总被率先输入到被测程序执行**。


### 种子的变异方法及实现

实现了课件上提到六种变异方法：`BITFLIP`、`ARITHMETIC`、`NITEREST`、`DICTIONARY`、`HAVOC`、`SPLICE`。

变异策略为，筛选旧一轮的种子（已完成反馈和分数计算）中分数超过`SCORE_THRESHOLD`的种子，再根据其变异程度（反映在结构体`MutInfo.tot_times`中，即一个种子经过变异操作的总次数），决定是否对其进行小量数据变异或大量数据变异，以**确保种子首先要进行规模较小的首轮变异、而后再进行大规模的破坏与拼接变异**。


## 并行优化

### `ForkServer()`

静态插桩时插入`ForkServer()`函数到被测程序`main()`入口处，使得被测程序的**子程序无需再进行重复的加载、链接、初始化等操作**。


### 轮内并行、轮间串行

采用每`TEST_NUM_PER_ROUND`个子进程作为一轮（即Fuzz_main短时间内发放`TEST_NUM_PER_ROUND`个测试样例），而后由Fuzz_main集中处理当前轮下的全部子程序的反馈，完成总和`bitmap`的更新、种子分数计算和筛选、新种子的变异、种子队列维护等操作，实现**轮内并行、轮间串行**。

Fuzz_main进入轮种子处理步骤时，`ForkServer()`将进入等待状态；当Fuzz_main处理完毕后，`ForkServer()`将被唤醒、随后继续`fork`子进程。该过程通过前述提到的`inst_pipe`实现。
