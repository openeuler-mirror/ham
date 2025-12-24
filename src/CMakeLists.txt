set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/output/ham)

add_library(ham SHARED
        ham.c
        ham_comm.c
)

target_include_directories(ham PUBLIC . ${PROJECT_3RDPARTY_SRC_DIR}/libboundscheck/include)
target_link_libraries(ham
        -Wl,--start-group
        /usr/lib64/libboundscheck.so
        -Wl,--end-group)

install(FILES ham.h DESTINATION ${OUTPUT_DIR}/include PERMISSIONS ${HAM_INSTALL_INCLUDE_PERMISSIONS})
install(TARGETS ham DESTINATION ${OUTPUT_DIR}/lib PERMISSIONS ${HAM_INSTALL_LIB_PERMISSIONS})