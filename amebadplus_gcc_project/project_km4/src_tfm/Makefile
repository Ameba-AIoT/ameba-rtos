
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(shell pwd)
DIRSUITES = $(DIR)/lib/ext/tfm_test_repo-src/test/secure_fw/suites
tfm_tests_dir	= $(ROOTDIR)/build_tfm/tf-m-tests/

MODULE_IFLAGS += -I$(DIR)/interface
MODULE_IFLAGS += -I$(DIR)/interface/include
MODULE_IFLAGS += -I$(DIR)/interface/include/crypto_keys
MODULE_IFLAGS += -I$(FREERTOSDIR)/include
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaDplus_KM4/non_secure
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaDplus_KM4/secure

GLOBAL_CFLAGS += -Wno-cast-function-type
GLOBAL_CFLAGS += -DPLATFORM_DEFAULT_CRYPTO_KEYS
GLOBAL_CFLAGS += -DPLATFORM_NS_NV_COUNTERS=0
GLOBAL_CFLAGS += -DPROJECT_CONFIG_HEADER_FILE=\"$(DIR)/interface/config_profile_medium.h\" 
GLOBAL_CFLAGS += -DPS_CRYPTO_AEAD_ALG_CCM
GLOBAL_CFLAGS += -DPS_ENCRYPTION
GLOBAL_CFLAGS += -DQCBOR_DISABLE_FLOAT_HW_USE
GLOBAL_CFLAGS += -DQCBOR_DISABLE_PREFERRED_FLOAT
GLOBAL_CFLAGS += -DTFM_PARTITION_CRYPTO
GLOBAL_CFLAGS += -DTFM_PARTITION_IDLE
GLOBAL_CFLAGS += -DTFM_PARTITION_INITIAL_ATTESTATION
GLOBAL_CFLAGS += -DTFM_PARTITION_INTERNAL_TRUSTED_STORAGE
GLOBAL_CFLAGS += -DTFM_PARTITION_LOG_LEVEL=TFM_PARTITION_LOG_LEVEL_SILENCE
GLOBAL_CFLAGS += -DTFM_PARTITION_NS_AGENT_TZ
GLOBAL_CFLAGS += -DTFM_PARTITION_PLATFORM
GLOBAL_CFLAGS += -DTFM_PARTITION_PROTECTED_STORAGE
GLOBAL_CFLAGS += -DTFM_PARTITION_TEST_CORE_IPC
GLOBAL_CFLAGS += -DTFM_PARTITION_TEST_PS
GLOBAL_CFLAGS += -DTFM_PARTITION_TEST_SECURE_SERVICES

ifeq ($(CONFIG_TFM_TEST_ENABLE), y)
GLOBAL_CFLAGS += -DDOMAIN_NS=1
GLOBAL_CFLAGS += -DTEST_FRAMEWORK_NS
# GLOBAL_CFLAGS += -DTEST_NS_FLIH_IRQ
# GLOBAL_CFLAGS += -DTFM_FIH_PROFILE_OFF
endif

vpath %.c $(DIR) $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
SRC_PATH += $(DIR)/interface/src
CSRC :=	$(foreach dir, $(SRC_PATH), $(wildcard $(dir)/*.c))

ifeq ($(CONFIG_TFM_TEST_ENABLE), y)

ifeq ($(findstring -DTEST_NS_ATTESTATION=OFF,$(CMAKE_ARGU)),)
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/attestation/libtfm_test_suite_attestation_ns.a
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/t_cose/libtfm_t_cose_ns.a
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/qcbor/libtfm_qcbor_ns.a
endif
ifeq ($(findstring -DTEST_NS_T_COSE=OFF,$(CMAKE_ARGU)),)
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/t_cose/libtfm_test_suite_t_cose_ns.a
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/t_cose/libtfm_t_cose_test.a
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/t_cose/libtfm_t_cose_ns.a
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/qcbor/libtfm_qcbor_ns.a
endif
ifeq ($(findstring -DTEST_NS_CRYPTO=OFF,$(CMAKE_ARGU)),)
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/crypto/libtfm_test_suite_crypto_ns.a
endif
ifeq ($(findstring -DTEST_NS_ITS=OFF,$(CMAKE_ARGU)),)
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/its/libtfm_test_suite_its_ns.a
endif
ifeq ($(findstring -DTEST_NS_PS=OFF,$(CMAKE_ARGU)),)
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/ps/libtfm_test_suite_ps_ns.a
endif
ifeq ($(findstring -DTEST_NS_PLATFORM=OFF,$(CMAKE_ARGU)),)
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/platform/libtfm_test_suite_platform_ns.a
endif
ifeq ($(findstring -DTEST_NS_IPC=OFF,$(CMAKE_ARGU)),)
LINK_TFM_LIB += $(tfm_tests_dir)/test/secure_fw/suites/spm/ipc/libtfm_test_suite_ipc_ns.a
endif

ifeq ($(CONFIG_TFM_TEST_PSA_API), y)
LINK_TFM_LIB += $(tfm_tests_dir)/app/psa_api_tests/val/val_nspe.a
LINK_TFM_LIB += $(tfm_tests_dir)/app/psa_api_tests/platform/pal_nspe.a

ifeq ($(CONFIG_TEST_PSA_API_STORAGE), y)
LINK_TFM_LIB += $(tfm_tests_dir)/app/psa_api_tests/dev_apis/storage/test_combine.a
endif
ifeq ($(CONFIG_TEST_PSA_API_CRYPTO), y)
LINK_TFM_LIB += $(tfm_tests_dir)/app/psa_api_tests/dev_apis/crypto/test_combine.a
endif
ifeq ($(CONFIG_TEST_PSA_API_INITIAL_ATTESTATION), y)
LINK_TFM_LIB += $(tfm_tests_dir)/app/psa_api_tests/dev_apis/initial_attestation/test_combine.a
endif
endif

LINK_TFM_LIB += $(tfm_tests_dir)/app/libtfm_test_app.a
LINK_TFM_LIB += $(tfm_tests_dir)/app/libtfm_api_ns.a

CSRC += $(DIR)/ameba_tfm_test_cmd.c
endif

CSRC += $(DIR)/interface/src/os_wrapper/tfm_ns_interface_rtos.c
CSRC += $(DIR)/ameba_freertos_wrapper.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	@for libname in $(LINK_TFM_LIB); do $(AR) x $$libname; done
	@$(AR) rvs lib_tfm_ns.a *.o
	@$(MOVE) -f lib_tfm_ns.a $(IMAGE_TARGET_FOLDER)/lib_tfm_ns.a
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)	
