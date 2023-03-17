list(APPEND LIBFGC_REWRITER_APP_SRCS "${FGCR_ROOT}/test/main.c")

add_executable(fgc_rewriter_app ${LIBFGC_REWRITER_APP_SRCS})

target_link_libraries(fgc_rewriter_app fgc_rewriter)
