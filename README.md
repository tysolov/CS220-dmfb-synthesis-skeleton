# CS220-dmfb-synthesis-skeleton
A skeleton project for implementing synthesis algorithms for digital microfluidic biochips, derived from UCR's 
open source MFSimStatic project.

This version of the project compiles assays represented by a CFG and independent DAGs for each basic block, with TRANSFER_IN instructions representing SSA phi nodes, and TRANSFER_OUT instructions representing SSI sigma nodes.



## Getting started:
- Clone or fork this repo `$ git clone https://github.com/tlove004/CS220-dmfb-synthesis-skeleton.git`
- Edit the `CMakeLists.txt` file to set the path variable `YourPath` to the root of the project:
    - e.g. if you clone the directory to your home directory `~/`, you could set the variable `YourPath` to `~/CS220-dmfb-synthesis-skeleton/`

- Create a build directory and generate the Makefiles 
    ```$ mkdir build && cd build && cmake ..```
- Build the project `$ make`

- You should now have an executable source file `CS220Synth` that should synthesize the default experiment (PCR) with the default synthesis algorithms.

A basic scheduler `list_scheduler`, placer `grissom_left_edge_binder`, and router `roy_maze_router` have been included 
for reference on API usage.  Reported results (in your final submission) should compare against the given synthesis algorithms.

#### Troubleshooting tips:
- Use an IDE!  This is a large project, with a lot of dependencies, and the ability to quickly visualize the structure in an IDE will make your life a lot easier.  CLion (https://www.jetbrains.com/clion/) is my recommendation, and is free for all students with a ucr.edu email account.
- If you are having issues with source files not showing up during your build, do not forget that you must add these sources to `CMakeLists.txt`.  Header files must be discoverable via the `include_directories` command, and source files should be set in the `set(CS220Synth_SOURCES ....)` section.
