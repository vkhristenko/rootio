file (GLOB SOURCES *.cpp)
foreach (SOURCE ${SOURCES})
    get_filename_component(EXE ${SOURCE} NAME_WE)
    list (APPEND EXECS ${EXE})
    add_executable(${EXE} ${SOURCE})
    target_link_libraries(${EXE} rootio-corecpp rootio-core ${ROOT_LIBRARIES})
endforeach(SOURCE)

# install generated artifacts
install(TARGETS ${EXECS}
        RUNTIME DESTINATION root-tests/corecpp)
