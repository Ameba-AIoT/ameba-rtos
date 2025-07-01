execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND python ${c_DIAG_FMT_SCRIPT} h2j ${fmt_head} ${fmt_json}
)
execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND python ${c_DIAG_FMT_SCRIPT} hash ${fmt_json}
    OUTPUT_VARIABLE HASH_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
configure_file(${CMAKE_CURRENT_LIST_DIR}/ameba_diagnose_hash.c.in ${fmt_src} @ONLY)