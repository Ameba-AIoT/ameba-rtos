#define VERSION_MAJOR            0
#define VERSION_MINOR            0
#define VERSION_REVISION         0
#define VERSION_BUILDNUM         924
#define VERSION_GCID             0x54f220e6
#define VERSION_GCIDH            0x5f476740
#define VERSION_PGCID            0x8b6a57d5
#define VERSION_PGCIDH           0x71c633c7
#define LIB_NAME                 "green2-km4"
#define CUSTOMER_NAME            lib
#define CN_1                     'l'
#define CN_2                     'i'
#define CN_3                     'b'
#define CN_4                     '#'
#define CN_5                     '#'
#define CN_6                     '#'
#define CN_7                     '#'
#define CN_8                     '#'
#define BUILDING_TIME            "Wed Mar 19 13:49:04 2025"
#define NAME2STR(a)              #a
#define CUSTOMER_NAME_S          #NAME2STR(CUSTOMER_NAME)
#define NUM4STR(a,b,c,d)         #a "." #b "." #c "." #d
#define VERSIONBUILDSTR(a,b,c,d) NUM4STR(a,b,c,d)
#define VERSION_BUILD_STR        VERSIONBUILDSTR(VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION,VERSION_BUILD)
#define COMMIT                   54f220e65f47
#define BUILDING_TIME_STR        Wed_2025_03_19_13_49_04
#define BUILDER                  carrie_zhang
#define BUILDER_STR              carrie_zhang
#define TO_STR(R) NAME2STR(R)
#define GENERATE_VERSION_MSG(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER) \
    GENERATE_VERSION_MSG_(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER)
#define GENERATE_VERSION_MSG_(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER) \
    MSG##_##VERSION##_##COMMIT##_##BUILDING_TIME##_##BUILDER

#define GENERATE_BIN_VERSION(MSG, VERSION) \
    typedef char GENERATE_VERSION_MSG(MSG, VERSION, COMMIT, BUILDING_TIME_STR, BUILDER_STR);
