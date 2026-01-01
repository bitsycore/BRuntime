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

void BIG_TITLE(const char* _x_);
void SUB_TITLE(const char* _x_);

#define log_fmt(...) BF_Print(__VA_ARGS__)

#define TO_STR(_x_) \
  BO_StringCPtr((BO_StringRef)(BFAutorelease($OBJ BO_ToString($OBJ(_x_)))))

#define PRINT_ERR_IF_NOT(_cond_) \
    if (!(_cond_)) { \
        log_fmt(BC_AE_RED "Assert failed: " BC_AE_BRED #_cond_ BC_AE_RED " at: line %d\n" BC_AE_RESET, __LINE__); \
    }

#define TEST(_name_) log_fmt(BC_AE_YELLOW "  • " BC_AE_RESET "%s\n", _name_)

#define TITLE(_name_) \
    log_fmt(BC_AE_CYAN "\n▶ " BC_AE_BCYAN "%s" BC_AE_RESET "\n", _name_)

#define ASSERT(_cond_, _msg_) \
    if (!(_cond_)) { \
        log_fmt(BC_AE_RED "    ✗ " BC_AE_RESET "%s\n", _msg_); \
    } else { \
        log_fmt(BC_AE_GREEN "    ✓ " BC_AE_RESET "%s\n", _msg_); \
    }

#define ASSERT_SILENT(_cond_, _msg_) \
    if (!(_cond_)) { \
        log_fmt(BC_AE_RED "    ✗ " BC_AE_RESET "%s\n", _msg_); \
    }

#endif //BRUNTIME_BT_COMMON_H