#ifndef BCRUNTIME_TESTS_H
#define BCRUNTIME_TESTS_H

#include <BCore/Console/BC_AnsiEscape.h>

#include <BFramework/BF.h>
#include <BFramework/BF_AutoreleasePool.h>
#include <BFramework/BF_Class.h>
#include <BFramework/BF_Format.h>
#include <BFramework/Object/BO_List.h>
#include <BFramework/Object/BO_Map.h>
#include <BFramework/Object/BO_Number.h>
#include <BFramework/Object/BO_String.h>
#include <BFramework/Object/BO_StringBuilder.h>

#include <stdio.h>
#include <string.h>

#define log_fmt(...) BF_printf(__VA_ARGS__)

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

void BIG_TITLE(const char* _x_);
void SUB_TITLE(const char* _x_);

void testArray();
void testString();
void testStringBuilder();
void testNumber();
void testMap();
void testReleasePool();
void testClassRegistry();
void benchmarkAutoreleasePool();
void testBytesArray();

#endif // BCRUNTIME_TESTS_H
