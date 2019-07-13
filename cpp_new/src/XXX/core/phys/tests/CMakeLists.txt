file (GLOB SOURCES *.cpp)
foreach (SOURCE ${SOURCES})
    get_filename_component(EXE ${SOURCE} NAME_WE)
    list (APPEND EXECS ${EXE})
    add_executable(${EXE} ${SOURCE})
    target_link_libraries(${EXE} XXX-core-phys XXX-core-common)
endforeach(SOURCE)

# install generated artifacts
#install(TARGETS ${EXECS}
#        RUNTIME DESTINATION tests/core)
