#define VERSION_MAJOR            1
#define VERSION_MINOR            0
#define VERSION_REVISION         0
#define VERSION_BUILDNUM         978
#define VERSION_GCID             0x28c7c915
#define VERSION_GCIDH            0x300dfdf0
#define VERSION_PGCID            0x092ef6c7
#define VERSION_PGCIDH           0x8ebe6121
#define LIB_NAME                 "amebadp"
#define CUSTOMER_NAME            lib
#define CN_1                     'l'
#define CN_2                     'i'
#define CN_3                     'b'
#define CN_4                     '#'
#define CN_5                     '#'
#define CN_6                     '#'
#define CN_7                     '#'
#define CN_8                     '#'
#define BUILDING_TIME            "Mon Oct 20 17:53:19 2025"
#define NAME2STR(a)              #a
#define CUSTOMER_NAME_S          #NAME2STR(CUSTOMER_NAME)
#define NUM4STR(a,b,c,d)         #a "." #b "." #c "." #d
#define VERSIONBUILDSTR(a,b,c,d) NUM4STR(a,b,c,d)
#define VERSION_BUILD_STR        VERSIONBUILDSTR(VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION,VERSION_BUILD)
#define COMMIT                   28c7c915300d
#define BUILDING_TIME_STR        Mon_2025_10_20_17_53_20
#define BUILDER                  server
#define BUILDER_STR              server
#define TO_STR(R) NAME2STR(R)
#define GENERATE_VERSION_MSG(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER) \
    GENERATE_VERSION_MSG_(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER)
#define GENERATE_VERSION_MSG_(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER) \
    MSG##_##VERSION##_##COMMIT##_##BUILDING_TIME##_##BUILDER

#define GENERATE_BIN_VERSION(MSG, VERSION) \
    typedef char GENERATE_VERSION_MSG(MSG, VERSION, COMMIT, BUILDING_TIME_STR, BUILDER_STR);
