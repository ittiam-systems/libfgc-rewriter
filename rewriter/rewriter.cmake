# src files
list(
  APPEND
  LIBFGC_REWRITER_SRCS
  
  "${FGCR_ROOT}/rewriter/avc/fgcr_bitstrm.c"
  "${FGCR_ROOT}/rewriter/avc/fgcr_parse_headers.c"
  "${FGCR_ROOT}/rewriter/avc/fgcr_parse_slice.c"
  "${FGCR_ROOT}/rewriter/avc/fgcr_parse_cavlc.c"
  "${FGCR_ROOT}/rewriter/avc/fgcr_nal.c"
  "${FGCR_ROOT}/rewriter/avc/fgcr_sei.c"
  "${FGCR_ROOT}/rewriter/avc/fgcr_api.c")

include_directories(${FGCR_ROOT}/rewriter/avc)


add_library(fgc_rewriter STATIC ${LIBFGC_REWRITER_SRCS} )