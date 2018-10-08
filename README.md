# CS220-dmfb-synthesis-skeleton
A skeleton project for implementing synthesis algorithms for digital microfluidic biochips, derived from UCR's 
open source MFSimStatic project.

This version of the project compiles assays represented by a CFG and independent DAGs for each basic block, with TRANSFER_IN instructions representing SSA phi nodes, and TRANSFER_OUT instructions representing SSI sigma nodes.

This project has been verified to run appropriately on macOS >= 10.13, Ubuntu Linux 14.04 LTS, and Windows 10 (using the Linux Subsystem).  It should work as is on any environment where cmake and GCC or Clang is available.

### Requirements:
- `git`
- `cmake` 2.8 or later
- `C++11` or later (if you use > `C++11` features, update the `CMakeLists.txt` file to require the later version to avoid warnings/errors)

## Getting started:
- Clone or fork this repo `$ git clone https://github.com/tlove004/CS220-dmfb-synthesis-skeleton.git`
- Edit the `CMakeLists.txt` file to set the path variable `YourPath` to the root of the project:
    - e.g. if you clone the directory to your home directory `~/`, you could set the variable `YourPath` to `~/CS220-dmfb-synthesis-skeleton/`

- Create a build directory and generate the Makefiles 
    ```$ mkdir build && cd build && cmake ..```
- Build the project `$ make`

You should now have an executable source file `CS220Synth` that should synthesize the default experiment (PCR) with the default synthesis algorithms.

A basic scheduler `list_scheduler`, placer `grissom_left_edge_binder`, and router `roy_maze_router` have been included 
for reference on API usage.  Reported results (in your final submission) should compare against the given synthesis algorithms.

#### Troubleshooting tips:
- Use an IDE!  This is a large project, with a lot of dependencies, and the ability to quickly visualize the structure in an IDE will make your life a lot easier.  CLion (https://www.jetbrains.com/clion/) is my recommendation, and is free for all students with a ucr.edu email account.  Also, the built in visual debugger is fantastic.
- If you are having issues with source files not showing up during your build, do not forget that you must add these sources to `CMakeLists.txt`.  Header files must be discoverable via the `include_directories` command, and source files should be set in the `set(CS220Synth_SOURCES ....)` section.
- Certain data structures, in certain situations, do not reveal their contents in `gdb`, but work fine in `lldb`, and vice-versa.  If you are having trouble viewing data structures during debugging, try a different compiler/debugger pair.

#### General nodes:
All input files (CFG and DAGs) are parsed for you. Placeholder classes have been created for you at:
- Scheduler: `Headers/Scheduler/cs220_scheduler.h` and `Source/Scheduler/cs220_scheduler.cc`
- Placer: `Headers/Placer/cs220_placer.h` and `Source/Placer/cs220_placer.cc`
- Router: `Headers/Router/cs220_router.h` and `Source/Router/cs220_router.cc`

Whichever synthesis step you are developing an algorithm for is accomplished
at the DAG level--the engine takes care of control flow constructs for you.

All output files are created for you, provided you follow the given APIs correctly.  Please refer to the provided scheduler/placer/router to view proper API usage.

#### Understanding the .cfg and .dag files

Although not necessary, as the files are parsed for you, understanding the input files may be of some help.

- a `.cfg` file specifies a control flow graph for digital microfluidic lab-on-a-chips. It is organized as follows:
  - the `NAME` property simply names the CFG, this has no bearing on execution
  - all DAGs in the program are listed using the `DAG(<dagname>)` command.
  - the `NUMCGS` property specifies the number of conditional groups (if/else blocks) present
  - for each control group there are any number of `COND` commands
    - `COND` (condition) commands have a variable number parameters: group number, number of dependent dags, comma-separated-list of dependent dags, number of branch dags, comma-separated-list of branch dags, expression ID
  - for each `COND` command there are any number of `EXP` and `TD` commands
    - `EXP` (expression) commands will match the condition's expression ID in the first parameter, followed by expression-type specific number of parameters.  The types of expressions are self-explanatory.
    - `TD` (transfer droplet) commands provide control flow for individual droplets from a DAGfrom, nodeID to a DAGto, nodeID.
    
- a `.dag` file specifies the operations performed at the level of a basic block. 
  - the `DagName` property names the dag for the `.cfg` file to find it.
  - there are number of `NODE` commands, each followed by any number of `EDGE` commands
    - a `NODE` specifies a fluidic operation to be performed.  Parameters include a node id, the type of operation, and operation-specific features such as time, volume, and/or name
    - an `EDGE` specifies a dependency between nodes. Specifically, the ege `EDGE(i, j)` requires that node i completes before node j can begin.  It also signifies that node j _consumes_ node i
    
#### Resources:

For each synthesis step, we have provided a default algorithm for you to compare against.  For more on these algorithms, refer to the following:

- Scheduling [list scheduler]
  - F. Su and K. Chakrabarty. "High-level synthesis of digital microfluidic biochips," ACM J. Emerging Tech. Comput. Syst., vol. 3, no. 4, pp. 16.1- 16.32, Jan. 2008.
  - O'Neal, Kenneth, Daniel Grissom, and Philip Brisk. "Force-directed list scheduling for digital microfluidic biochips." In VLSI and System-on-Chip (VLSI-SoC), 2012 IEEE/IFIP 20th International Conference on, pp. 6-pp. IEEE, 2012.
  
- Placement [left edge binder and the virtual topology]
  - Grissom, Daniel, and Philip Brisk. "Fast online synthesis of generally programmable digital microfluidic biochips." In Proceedings of the eighth IEEE/ACM/IFIP international conference on Hardware/software codesign and system synthesis, pp. 413-422. ACM, 2012.
  - Grissom, Daniel T., and Philip Brisk. "Fast online synthesis of digital microfluidic biochips." IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems 33, no. 3 (2014): 356-369.
  
- Routing [roy maze router]
  - Roy, Pranab, Hafizur Rahaman, and Parthasarathi Dasgupta. "A novel droplet routing algorithm for digital microfluidic biochips." In Proceedings of the 20th symposium on Great lakes symposium on VLSI, pp. 441-446. ACM, 2010.
  - Soukup, J. I. R. I. "Fast maze router." In Proceedings of the 15th Design Automation Conference, pp. 100-102. IEEE Press, 1978.