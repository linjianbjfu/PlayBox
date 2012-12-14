#define INI_SECTION_BUFFER_LENGTH        10240
#define MAX_SECTION_LENGTH               65535

#define STR_ADVERT_SECTION        "Advert"
#define STR_FLASH_UPDATE_SECTION  "FlashUpdate"
#define STR_UPDATE_SKIN_SECTION     "UpdateSkin"
#define STR_POP_MESSAGE_SECTION		"MultiPopMessage"
#define STR_SKIN_TIPS			"SkinTips"
#define STR_LRC_LOW_AD_SECTION	"LrcLowAd"

void UpdateSectionConfig(const char* szSectionName, const char* szConfigPath, const char* szDesFilePath);
void UpdateConfigFile(const char* szDesFilePath);
void UpdateDateSectionConfig(const char* szSectionName, const char* szConfigPath, const char* szDesFilePath);
