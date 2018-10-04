# CS220-dmfb-synthesis-skeleton
A skeleton project for implementing synthesis algorithms for digital microfluidic biochips, derived from UCR's 
open source MFSimStatic project.

This version of the project compiles assays represented by a CFG and independent DAGs for each basic block, with TRANSFER_IN instructions representing SSA phi nodes, and TRANSFER_OUT instructions representing SSI sigma nodes.

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