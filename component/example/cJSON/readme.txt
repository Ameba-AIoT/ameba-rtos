##################################################################################
#                                                                                #
#                               cJSON EXAMPLE                                    #
#                                                                                #
##################################################################################

Date: 2019-10-24

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Parameter Setting and Configuration
 - Result description
 - Supported List

Description
~~~~~~~~~~~
    This example shows how to use cJSON lib to generate JSON data and how to parse JSON data.


Setup Guide
~~~~~~~~~~~

For Ameba-z
        1. Add cJSON example to SDK
        
        /component/common/utilities
        .
        |-- cJSON.c
        |-- cJSON.h
        
        /component/common/example/cJSON
        .
        |-- example_cJSON.c
        |-- example_cJSON.h
        `-- readme.txt
        
        2. Enable CONFIG_EXAMPLE_CJSON in [platform_opts.h]
        /* For cJSON example */
        #define CONFIG_EXAMPLE_CJSON     1

        3. Add example_cJSON() to [example_entry.c]
        #if defined(CONFIG_EXAMPLE_CJSON) && CONFIG_EXAMPLE_CJSON
        #include <cJSON/example_cJSON.h>
        #endif
        void example_entry(void)
        {
        #if defined(CONFIG_EXAMPLE_CJSON) && CONFIG_EXAMPLE_CJSON
            example_cJSON();
        #endif
        }
        
        4. Add cJSON lib to project
        (a) For IAR project, add cJson source code to group <utilities> 
            $PROJ_DIR$\..\..\..\component\common\utilities\cJSON.c
        (b) For GCC project, add cJSON source file to utilities Makefile
            CSRC += $(DIR)/cJSON.c
        
        5. Add cJSON example source files to project
        (a) For IAR project, add cJSON example to group <example> 
            $PROJ_DIR$\..\..\..\component\common\example\cJSON\example_cJSON.c
        (b) For GCC project, add cJSON example to example Makefile
            CSRC += $(DIR)/cJSON/example_cJSON.c

For Ameba-1, Ameba-pro, Ameba-z2, AmebaD
	Enable CONFIG_EXAMPLE_CJSON in [platform_opts.h]
        /* For cJSON example */
        #define CONFIG_EXAMPLE_CJSON     1
	
For AmebaLite
	Build: make all EXAMPLE=cJSON
Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Modify generate_json_data() to specific data structure and parse it in handle_json_data().

Result description
~~~~~~~~~~~~~~~~~~
    The data structure you defined display on the screen after booting.

[Supported List]
        Supported :
            AMEBA-1, AMEBA-PRO, AMEBA-Z2, AMEBAD, AMEBASMART, AMEBALITE
        Source code not in project:
            AMEBA-Z