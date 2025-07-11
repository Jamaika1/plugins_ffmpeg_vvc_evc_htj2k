#define LCEVC_MAJOR_VERSION 4
#define LCEVC_MINOR_VERSION 0
#define LCEVC_PATCH_VERSION 0

# define __STR(x)  #x
# define STR(x)    __STR(x)

const char* const CoreVersionFull = "LCEVC" STR(LCEVC_MAJOR_VERSION) "." STR(LCEVC_MINOR_VERSION) "." STR(LCEVC_PATCH_VERSION) "-49183ed";
