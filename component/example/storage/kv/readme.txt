
This example is for key-value module which wraps littlefs.

[Setup Guide]
~~~~~~~~~~~
	    1. In project/realtek_amebaLite_va0_example/GCC-RELEASE/project_ap/asdk/make/file_system/Makefile ADD:
		COMPONENT += littlefs
		COMPONENT += kv
		COMPONENT_CLEAN += littlefs
		COMPONENT_CLEAN += kv
	    2. GCC: use CMD "make all EXAMPLE=kv" to compile kv example. 

[Supported List]
~~~~~~~~~~
		AMEBASMART, AMEBALITE, AMEBADPLUS

Result description
~~~~~~~~~~~~~~~~~~
	   Example Steps:
			1. Set key-value pair (key = TEST_FILE, value = KV module works normally !!!)
			2. Get key-value pair
			3. Delete key-value pair

	   Expected Results:
			Step 1: Set key-value pair success with value length = 28
			Step 2: Get key-value pair success with value length = 28
			Step 3: Delete Success