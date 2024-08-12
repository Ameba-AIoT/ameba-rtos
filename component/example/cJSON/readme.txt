##################################################################################
#                                                                                #
#                               cJSON EXAMPLE                                    #
#                                                                                #
##################################################################################

Date: 2024-7-31

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

For AmebaDPlus, AmebaLite, AmebaSmart,
	you only need to use command "make all EXAMPLE=cJSON" in the appropriate directory to compile cJSON example into the image.
		For example:
		>> cd amebadplus_gcc_project
		or
		>> cd amebalite_gcc_project
		or
		>> cd amebasmart_gcc_project

		>> make all EXAMPLE=cJSON

	Download the image(Take Dplus as an example: km4_boot_all.bin,km0_km4_app.bin) generated in the corresponding directory into the respective board.


Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    You can review the EXAMPLE file "example_cJSON.c" located in component/example/cJSON/. Try to modify the function of generate_json_data() according to specific data structure, and then parse the generated data structure in handle_json_data().

Result description
~~~~~~~~~~~~~~~~~~
    The data structure you defined display on the screen after booting.
	For example:

		{
			"Motion_Sensor":	5,
			"Light":	{
				"Red":	253,
				"Green":	123,
				"Blue":	43
			}
		}

[Supported List]
        Supported :
			AMEBADPLUS, AMEBALITE, AMEBASMART
