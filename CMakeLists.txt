cmake_minimum_required(VERSION 2.8)
project(CS220Synth)

set(YourPath "~/CS220-dmfb-synthesis-skeleton/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_CXX_STANDARD 11)

include_directories(Headers)
include_directories(Headers/Models)
include_directories(Headers/PinMapper)
include_directories(Headers/Placer)
include_directories(Headers/Resources)
include_directories(Headers/Router)
include_directories(Headers/Scheduler)
include_directories(Headers/Util)
include_directories(Headers/WireRouter)

set(CS220Synth_SOURCES
        Source/Models/assay_node.cc
        Source/Models/cfg.cc
        Source/Models/compiled_cfg.cc
        Source/Models/compiled_dag.cc
        Source/Models/conditional_group.cc
        Source/Models/dag.cc
        Source/Models/diagonal_wire_routing_model.cc
        Source/Models/dmfb_arch.cc
        Source/Models/droplet.cc
        Source/Models/entity.cc
        Source/Models/expression.cc
        Source/Models/fixed_module.cc
        Source/Models/io_port.cc
        Source/Models/min_cost_max_flow_graph.cc
        Source/Models/op_module.cc
        Source/Models/reconfig_module.cc
        Source/Models/operation_coalescing_pool.cc
        Source/PinMapper/indiv_addr_pin_mapper.cc
        Source/PinMapper/pin_mapper.cc
        Source/Placer/cs220_placer.cc
        Source/Placer/grissom_left_edge_binder.cc
        Source/Placer/placer.cc
        Source/Router/cs220_router.cc
        Source/Router/post_subprob_compact_router.cc
        Source/Router/router.cc
        Source/Router/roy_maze_router.cc
        Source/Scheduler/list_scheduler.cc
        Source/Scheduler/cs220_scheduler.cc
        Source/Scheduler/priority.cc
        Source/Scheduler/scheduler.cc
        Source/Util/claim.cc
        Source/Util/elapsed_timer.cc
        Source/Util/command_line.cc
        Source/Util/compatibility_check.cc
        Source/Util/analyze.cc
        Source/Util/file_in.cc
        Source/Util/file_out.cc
        Source/Util/sort.cc
        Source/Util/util.cc
        Source/WireRouter/path_finder_wire_router.cc
        Source/WireRouter/paths.cc
        Source/WireRouter/wire_router.cc
        Source/WireRouter/wire_segment.cc
        Source/main.cc
        Source/synthesis.cc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${YourPath})

add_executable(CS220Synth ${CS220Synth_SOURCES})
