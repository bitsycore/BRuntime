#ifndef BRUNTIME_BT_COMMON_H
#define BRUNTIME_BT_COMMON_H

#include <BCore/Console/BC_AnsiEscape.h>

#include <BFramework/BF.h>
#include <BFramework/BF_AutoreleasePool.h>
#include <BFramework/BF_Class.h>
#include <BFramework/BF_Format.h>
#include <BFramework/BObject/BO_List.h>
#include <BFramework/BObject/BO_Map.h>
#include <BFramework/BObject/BO_Number.h>
#include <BFramework/BObject/BO_String.h>
#include <BFramework/BObject/BO_StringBuilder.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

void BT_PrintBigTitle(const char* _x_);
void BT_PrintSubTitle(const char* _x_);

double BT_GetTimeMicroseconds(clock_t start, clock_t end);

#define BT_Print(...) BF_Print(__VA_ARGS__)

#define BT_ToStr(_x_) \
  BO_StringCPtr((BO_StringRef)(BF_Autorelease($OBJ BO_ToString($OBJ(_x_)))))

#define BT_PrintErrIfNot(_cond_) \
    if (!(_cond_)) { \
        BT_Print(BC_AE_RED "Assert failed: " BC_AE_BRED #_cond_ BC_AE_RED " at: line %d\n" BC_AE_RESET, __LINE__); \
    }

#define BT_Test(_name_) BT_Print(BC_AE_YELLOW "  • " BC_AE_RESET "%s\n", _name_)

#define BT_Title(_name_) \
    BT_Print(BC_AE_CYAN "\n▶ " BC_AE_BCYAN "%s" BC_AE_RESET "\n", _name_)

#define BT_Assert(_cond_, _msg_) \
    if (!(_cond_)) { \
        BT_Print(BC_AE_RED "    ✗ " BC_AE_RESET "%s\n", _msg_); \
    } else { \
        BT_Print(BC_AE_GREEN "    ✓ " BC_AE_RESET "%s\n", _msg_); \
    }

#define BT_AssertSilent(_cond_, _msg_) \
    if (!(_cond_)) { \
        BT_Print(BC_AE_RED "    ✗ " BC_AE_RESET "%s\n", _msg_); \
    }

#endif //BRUNTIME_BT_COMMON_H