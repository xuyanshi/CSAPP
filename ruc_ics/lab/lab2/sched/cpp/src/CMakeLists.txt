add_executable(trace_gen
    trace_gen.cc
    task.cc)
target_link_libraries(trace_gen
    PRIVATE nlohmann_json::nlohmann_json)

add_executable(sim
    sim.cc
    task.cc
    event.cc
    policy_wrapper.cc
    policy.cc)
target_link_libraries(sim
    PRIVATE nlohmann_json::nlohmann_json)
target_compile_definitions(sim
    PRIVATE $<$<CONFIG:Debug>:SCHED_DEBUG>
)

add_executable(sim_cpp
    sim.cc
    task.cc
    event.cc
    policy_wrapper.cc
    policy.cc)
target_link_libraries(sim_cpp
    PRIVATE nlohmann_json::nlohmann_json)
target_compile_definitions(sim_cpp
    PRIVATE CPP_SOLUTION
    PRIVATE $<$<CONFIG:Debug>:SCHED_DEBUG>
)


