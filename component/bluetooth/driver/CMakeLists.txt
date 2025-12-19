##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

ameba_list_append_if(CONFIG_BT_INIC public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_inic
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd
    ${c_CMPT_BLUETOOTH_DIR}/api/include
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources
    hci/hci_process/hci_process.c
	hci/hci_process/hci_download_v2.c
	hci/hci_process/hci_download_v3.c
	hci/hci_process/hci_common.c
	hci/hci_process/hci_standalone.c
	hci/hci_transport/hci_h4.c
)

ameba_list_append_if(CONFIG_AMEBADPLUS private_sources
	dtm/hci_dtm.c
)

ameba_list_append(private_sources
    platform/${c_SOC_TYPE}/hci/bt_mp_patch.c
    platform/${c_SOC_TYPE}/hci/bt_normal_patch.c
    platform/${c_SOC_TYPE}/hci/hci_platform.c
    platform/${c_SOC_TYPE}/hci/hci_uart.c
)

if(CONFIG_BT_INIC)
	ameba_list_append(private_sources hci/hci_if_inic.c)
	if(CONFIG_BT_INIC_H4 OR CONFIG_BT_INICD_H4)
		ameba_list_append(private_sources bt_inic/bt_inic_h4.c)
	elseif(CONFIG_BT_INIC_H5 OR CONFIG_BT_INICD_H5)
		ameba_list_append(private_sources bt_inic/bt_inic_h5.c)
	elseif(CONFIG_BT_INICD_SPI OR (CONFIG_WHC_INTF_SPI AND CONFIG_BT_INIC_SHARE))
		ameba_list_append(private_sources bt_inic/bt_inic_spi_dev.c)
	elseif(CONFIG_BT_INICD_SDIO OR (CONFIG_WHC_INTF_SDIO AND CONFIG_BT_INIC_SHARE))
		ameba_list_append_if(CONFIG_AMEBADPLUS private_sources bt_inic/bt_inic_sdio_dplus.c)
	endif()
elseif(CONFIG_BT_ZEPHYR)
	ameba_list_append(private_sources hci/hci_if_zephyr.c)
	ameba_list_append(private_includes
		 ${c_CMPT_BLUETOOTH_DIR}/zephyr/include
		 ${c_CMPT_BLUETOOTH_DIR}/zephyr/port
	)
elseif(CONFIG_BT_SDN_VERIFY)
	ameba_list_append(private_sources vhci/hci_if_virtual.c)
else()
	ameba_list_append(private_sources hci/hci_if_rtk.c)
endif()

ameba_list_append(private_includes
	hci
	inc
	inc/hci
	${c_CMPT_BLUETOOTH_DIR}/driver/platform/${c_SOC_TYPE}/inc
	${c_CMPT_BLUETOOTH_DIR}/rtk_coex
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(bt_hci_driver
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################