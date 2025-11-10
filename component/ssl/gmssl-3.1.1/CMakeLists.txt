##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    include
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

ameba_list_append(private_definitions CONFIG_BUILD_NONSECURE=1)
ameba_list_append(private_definitions -Drand_bytes=TRNG_get_random_bytes)
ameba_list_append(private_compile_options
    -mfloat-abi=softfp
    -O3
    -w
)

# SM4 mode configuration options

option(ENABLE_TEST_SPEED "Enable test speed" OFF)

option(ENABLE_SM4_ECB "Enable SM4 ECB mode" ON)
option(ENABLE_SM4_OFB "Enable SM4 OFB mode" ON)
option(ENABLE_SM4_CFB "Enable SM4 CFB mode" ON)
option(ENABLE_SM4_CCM "Enable SM4 CCM mode" ON)
option(ENABLE_SM4_XTS "Enable SM4 XTS mode" ON)
option(ENABLE_SM4_CBC_MAC "Enable SM4-CBC-MAC" ON)

option(ENABLE_SM2_EXTS "Enable SM2 Extensions" OFF)
option(ENABLE_SM3_XMSS "Enable SM3-XMSS signature" ON)

option(ENABLE_SHA1 "Enable SHA1" ON)
option(ENABLE_SHA2 "Enable SHA2" ON)
option(ENABLE_AES "Enable AES" ON)
option(ENABLE_CHACHA20 "Enable Chacha20" ON)

option(ENABLE_SKF "Enable SKF module" OFF)
option(ENABLE_SDF "Enable SDF module" ON)

option(ENABLE_ASM_UNDERSCORE_PREFIX "Add prefix `_` to assembly symbols" ON)
option(ENABLE_TLS_DEBUG "Enable TLS and TLCP print debug message" OFF)
option (ENABLE_SM2_ENC_PRE_COMPUTE "Enable SM2 encryption precomputing" ON)

# =============================================================================
# 源文件列表定义
# =============================================================================

# 定义核心库源文件列表 (src 变量)
# 包含了 GmSSL 库的所有核心算法实现文件
set(src
	src/version.c      # 版本信息
	src/debug.c        # 调试支持
	src/sm4.c          # SM4 对称加密算法核心实现

	# SM4 算法工作模式实现
	src/sm4_cbc.c      # CBC 模式
	src/sm4_ctr.c      # CTR 模式
	src/sm4_gcm.c      # GCM 认证加密模式

	# SM3 哈希算法及其应用
	src/sm3.c          # SM3 哈希算法核心实现
	src/sm3_hmac.c     # SM3-HMAC 消息认证码
	src/sm3_kdf.c      # SM3 密钥派生函数
	src/sm3_pbkdf2.c   # SM3-PBKDF2 密码哈希
	src/sm3_digest.c   # SM3 摘要工具接口

	# SM2 椭圆曲线公钥密码算法
	src/sm2_z256.c     # SM2 256位素数域运算
	src/sm2_z256_table.c # SM2 预计算表
	src/sm2_key.c      # SM2 密钥生成和管理
	src/sm2_sign.c     # SM2 数字签名
	src/sm2_enc.c      # SM2 公钥加密
	src/sm2_exch.c     # SM2 密钥交换

	# SM9 标识密码算法
	src/sm9_z256.c     # SM9 256位素数域运算
	src/sm9_z256_table.c # SM9 预计算表
	src/sm9_key.c      # SM9 密钥生成和管理
	src/sm9_sign.c     # SM9 标识签名
	src/sm9_enc.c      # SM9 标识加密
	src/sm9_exch.c     # SM9 密钥交换

	# ZUC 序列密码算法
	src/zuc.c          # ZUC 核心算法
	src/zuc_modes.c    # ZUC 工作模式

	# 通用密码算法接口层
	src/block_cipher.c # 分组密码统一接口
	src/digest.c       # 哈希算法统一接口
	src/hmac.c         # HMAC 统一接口
	src/hkdf.c         # HKDF 密钥派生

	# 数学运算和底层支持
	src/gf128.c        # GF(2^128) 域运算
	src/ghash.c        # GCM 模式的 GHASH 运算

	# 国密算法组合应用
	src/sm4_cbc_sm3_hmac.c  # SM4-CBC + SM3-HMAC 组合
	src/sm4_ctr_sm3_hmac.c  # SM4-CTR + SM3-HMAC 组合

	# 国际标准算法支持
	src/pkcs8.c        # PKCS#8 私钥格式
	src/ec.c           # 椭圆曲线算法支持
	src/rsa.c          # RSA 算法支持

	# 数据编码和格式支持
	src/asn1.c         # ASN.1 编码解码
	src/hex.c          # 十六进制编码
	src/base64.c       # Base64 编码
	src/pem.c          # PEM 格式支持

	# X.509 公钥基础设施
	src/x509_alg.c     # X.509 算法标识符
	src/x509_cer.c     # X.509 证书解析
	src/x509_ext.c     # X.509 扩展项
	src/x509_req.c     # X.509 证书请求
	src/x509_crl.c     # X.509 证书吊销列表
	src/x509_new.c     # X.509 证书生成

	# CMS (Cryptographic Message Syntax)
	src/cms.c          # CMS 消息语法

	# 网络通信和协议支持
	# src/socket.c       # Socket 网络通信

	# TLS/TLCP 安全协议栈
	# src/tls.c          # TLS 核心协议
	# src/tls_ext.c      # TLS 扩展支持
	# src/tls_trace.c    # TLS 协议跟踪调试
	# src/tlcp.c         # TLCP 中国传输层密码协议
	# src/tls12.c        # TLS 1.2 协议实现
	# src/tls13.c        # TLS 1.3 协议实现

	# 文件操作工具
	src/file.c         # 文件操作工具
	src/file.c         # 文件操作工具（重复项，应清理）
)

# 定义命令行工具源文件列表 (tools 变量)
# 包含了所有 GmSSL 命令行工具的实现文件
set(tools
	# 主程序和版本信息
	# tools/gmssl.c          # gmssl 主程序入口
	# tools/version.c        # 版本信息工具

	# SM4 对称加密工具
	tools/sm4.c            # SM4 基础加密工具
	tools/sm4_cbc.c        # SM4-CBC 模式工具
	tools/sm4_ctr.c        # SM4-CTR 模式工具
	tools/sm4_gcm.c        # SM4-GCM 模式工具
	tools/sm4_cbc_sm3_hmac.c  # SM4-CBC + SM3-HMAC 组合工具
	tools/sm4_ctr_sm3_hmac.c  # SM4-CTR + SM3-HMAC 组合工具

	# SM3 哈希算法工具
	tools/sm3.c            # SM3 哈希工具
	tools/sm3hmac.c        # SM3-HMAC 工具
	tools/sm3_pbkdf2.c     # SM3-PBKDF2 密码哈希工具
	tools/sm3xmss_keygen.c # SM3-XMSS 密钥生成工具

	# SM2 公钥密码工具
	tools/sm2keygen.c      # SM2 密钥生成工具
	tools/sm2sign.c        # SM2 签名工具
	tools/sm2verify.c      # SM2 验证工具
	tools/sm2encrypt.c     # SM2 加密工具
	tools/sm2decrypt.c     # SM2 解密工具

	# SM9 标识密码工具
	tools/sm9setup.c       # SM9 系统参数设置工具
	tools/sm9keygen.c      # SM9 密钥生成工具
	tools/sm9sign.c        # SM9 签名工具
	tools/sm9verify.c      # SM9 验证工具
	tools/sm9encrypt.c     # SM9 加密工具
	tools/sm9decrypt.c     # SM9 解密工具

	# ZUC 序列密码工具
	tools/zuc.c            # ZUC 算法工具

	# 其他密码工具
	tools/rand.c           # 随机数生成工具
	tools/ghash.c          # GHASH 运算工具

	# X.509 证书管理工具
	tools/certgen.c       # 证书生成工具
	tools/certparse.c     # 证书解析工具
	tools/certverify.c    # 证书验证工具
	tools/certrevoke.c    # 证书吊销工具

	# X.509 证书请求工具
	tools/reqgen.c        # 证书请求生成工具
	tools/reqparse.c      # 证书请求解析工具
	tools/reqsign.c       # 证书请求签名工具

	# X.509 证书吊销列表工具
	tools/crlgen.c        # CRL 生成工具
	tools/crlget.c        # CRL 获取工具
	tools/crlparse.c      # CRL 解析工具
	tools/crlverify.c     # CRL 验证工具

	# CMS (Cryptographic Message Syntax) 工具
	tools/cmssign.c       # CMS 签名工具
	tools/cmsverify.c     # CMS 验证工具
	tools/cmsencrypt.c    # CMS 加密工具
	tools/cmsdecrypt.c    # CMS 解密工具
	tools/cmsparse.c      # CMS 解析工具

	# TLS/TLCP 协议测试工具
	# tools/tlcp_client.c   # TLCP 客户端测试工具
	# tools/tlcp_server.c   # TLCP 服务器测试工具
	# tools/tls12_client.c  # TLS 1.2 客户端测试工具
	# tools/tls12_server.c  # TLS 1.2 服务器测试工具
	# tools/tls13_client.c  # TLS 1.3 客户端测试工具
	# tools/tls13_server.c  # TLS 1.3 服务器测试工具
)

# 定义测试程序列表 (tests 变量)
# 包含了所有算法和功能的单元测试程序
# 每个测试程序对应 tests/ 目录下的一个 test.c 文件
set(tests
	# SM4 对称加密算法测试
	sm4              # SM4 基础算法测试
	sm4_cbc          # SM4-CBC 模式测试
	sm4_ctr          # SM4-CTR 模式测试
	sm4_gcm          # SM4-GCM 模式测试

	# SM3 哈希算法测试
	sm3              # SM3 哈希算法测试
	sm4_sm3_hmac     # SM4 + SM3-HMAC 组合测试

	# SM2 公钥密码算法测试
	sm2_z256         # SM2 256位域运算测试
	sm2_key          # SM2 密钥操作测试
	sm2_sign         # SM2 数字签名测试
	sm2_enc          # SM2 公钥加密测试

	# SM9 标识密码算法测试
	sm9              # SM9 算法完整测试

	# ZUC 序列密码测试
	zuc              # ZUC 算法测试

	# 通用密码算法接口测试
	block_cipher     # 分组密码接口测试
	digest           # 哈希算法接口测试
	hmac             # HMAC 接口测试
	hkdf             # HKDF 密钥派生测试

	# 数学运算测试
	gf128            # GF(2^128) 域运算测试
	ghash            # GHASH 运算测试

	# 密钥格式和编码测试
	pkcs8            # PKCS#8 私钥格式测试
	ec               # 椭圆曲线算法测试
	asn1             # ASN.1 编码测试
	hex              # 十六进制编码测试
	base64           # Base64 编码测试
	pem              # PEM 格式测试

	# X.509 公钥基础设施测试
	x509             # X.509 证书基础测试
	x509_oid         # X.509 对象标识符测试
	x509_alg         # X.509 算法标识符测试
	x509_str         # X.509 字符串处理测试
	x509_ext         # X.509 扩展项测试
	x509_req         # X.509 证书请求测试
	x509_crl         # X.509 证书吊销列表测试

	# CMS 和协议测试
	cms              # CMS 消息语法测试
	tls              # TLS 协议测试
	tls13            # TLS 1.3 协议测试
)

# =============================================================================
# 条件编译逻辑处理
# =============================================================================

# 包含 CMake 的符号检查模块
include(CheckSymbolExists)

# 小型化编译选项
# 用于嵌入式或资源受限环境，减小代码体积
option(ENABLE_SMALL_FOOTPRINT "Enable small code size" OFF)
if (ENABLE_SMALL_FOOTPRINT)
	message(STATUS "ENABLE_SMALL_FOOTPRINT is ON")
	ameba_list_append(private_definitions -DENABLE_SMALL_FOOTPRINT)
endif()

# 性能测试选项
# 启用后会编译进性能测试代码
if (ENABLE_TEST_SPEED)
	message(STATUS "ENABLE_TEST_SPEED is ON")
	ameba_list_append(private_definitions -DENABLE_TEST_SPEED)
endif()

# SM2 算法标识符编码选项
# 控制某些特殊情况下的 ASN.1 编码方式
option(ENABLE_SM2_ALGOR_ID_ENCODE_NULL "Enable AlgorithmIdenifier with algorithm sm2sign_with_sm3 encode a NULL object as parameters" OFF)
if (ENABLE_SM2_ALGOR_ID_ENCODE_NULL)
	message(STATUS "ENABLE_SM2_ALGOR_ID_ENCODE_NULL is ON")
	ameba_list_append(private_definitions -DENABLE_SM2_ALGOR_ID_ENCODE_NULL)
endif()

# 汇编符号前缀选项
# 某些平台需要给汇编符号添加下划线前缀
if (ENABLE_ASM_UNDERSCORE_PREFIX)
	message(STATUS "ENABLE_ASM_UNDERSCORE_PREFIX is ON")
	ameba_list_append(private_definitions -DENABLE_ASM_UNDERSCORE_PREFIX)
endif()

if (ENABLE_SM2_NEON)
	message(STATUS "ENABLE_SM2_NEON is ON")
	ameba_list_append(private_definitions -DENABLE_SM2_NEON)
endif()

if (ENABLE_TLS_DEBUG)
	message(STATUS "ENABLE_TLS_DEBUG is ON")
	ameba_list_append(private_definitions -DENABLE_TLS_DEBUG)
endif()

if (ENABLE_SM4_ECB)
	message(STATUS "ENABLE_SM4_ECB is ON")
	ameba_list_append(private_definitions -DENABLE_SM4_ECB)
	list(APPEND src src/sm4_ecb.c)
	list(APPEND tools tools/sm4_ecb.c)
	list(APPEND tests sm4_ecb)
endif()

if (ENABLE_SM4_OFB)
	message(STATUS "ENABLE_SM4_OFB is ON")
	ameba_list_append(private_definitions -DENABLE_SM4_OFB)
	list(APPEND src src/sm4_ofb.c)
	list(APPEND tools tools/sm4_ofb.c)
	list(APPEND tests sm4_ofb)
endif()

if (ENABLE_SM4_CFB)
	message(STATUS "ENABLE_SM4_CFB is ON")
	ameba_list_append(private_definitions -DENABLE_SM4_CFB)
	list(APPEND src src/sm4_cfb.c)
	list(APPEND tools tools/sm4_cfb.c)
	list(APPEND tests sm4_cfb)
endif()

if (ENABLE_SM4_CCM)
	message(STATUS "ENABLE_SM4_CCM is ON")
	set(ENABLE_SM4_CBC_MAC ON)
	ameba_list_append(private_definitions -DENABLE_SM4_CCM)
	list(APPEND src src/sm4_ccm.c)
	list(APPEND tools tools/sm4_ccm.c)
	list(APPEND tests sm4_ccm)
endif()

if (ENABLE_SM4_XTS)
	message(STATUS "ENABLE_SM4_XTS is ON")
	ameba_list_append(private_definitions -DENABLE_SM4_XTS)
	list(APPEND src src/sm4_xts.c)
	list(APPEND tools tools/sm4_xts.c)
	list(APPEND tests sm4_xts)
endif()

if (ENABLE_SHA1)
	message(STATUS "ENABLE_SHA1 is ON")
	ameba_list_append(private_definitions -DENABLE_SHA1)
	list(APPEND src src/sha1.c)
	list(APPEND tests sha1)
endif()

if (ENABLE_SHA2)
	message(STATUS "ENABLE_SHA2 is ON")
	ameba_list_append(private_definitions -DENABLE_SHA2)
	list(APPEND src src/sha256.c src/sha512.c)
	list(APPEND tests sha224 sha256 sha384 sha512)
endif()

if (ENABLE_AES)
	message(STATUS "ENABLE_AES is ON")
	list(APPEND src src/aes.c src/aes_modes.c)
	list(APPEND tests aes)
endif()


if (ENABLE_CHACHA20)
	message(STATUS "ENABLE_CHACHA20 is ON")
	list(APPEND src src/chacha20.c)
	list(APPEND tests chacha20)
endif()


ameba_list_append(private_sources ${src})
ameba_list_append(private_sources ${tools})

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(gmssl
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