########## ENABLE CCACHE ##################
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message("ccache found")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)