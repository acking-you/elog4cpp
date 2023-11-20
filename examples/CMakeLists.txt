file(GLOB example_src ${PROJECT_SOURCE_DIR}/examples/*.cc)

add_definitions(-DLOG_DIR="${PROJECT_SOURCE_DIR}/examples/log/")

foreach (src_path ${example_src})
    get_filename_component(target ${src_path} NAME_WLE)
    add_executable(${target} ${src_path})
    target_link_libraries(${target} elog)
endforeach ()