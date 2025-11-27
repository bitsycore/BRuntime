#ifndef BCRUNTIME_TESTS_H
#define BCRUNTIME_TESTS_H

#include <BCRuntime/BCRuntime.h>
#include <BCRuntime/Core/BCAutoreleasePool.h>
#include <BCRuntime/Core/BCClass.h>
#include <BCRuntime/Object/BCList.h>
#include <BCRuntime/Object/BCMap.h>
#include <BCRuntime/Object/BCNumber.h>
#include <BCRuntime/Object/BCString.h>
#include <BCRuntime/Object/BCStringBuilder.h>
#include <BCRuntime/Utilities/BC_AnsiEscape.h>

#include <stdio.h>
#include <string.h>

#define log_fmt(...) printf(__VA_ARGS__)

#define TO_STR(_x_) \
  BCStringCPtr((BCStringRef)(BCAutorelease($OBJ BCToString($OBJ(_x_)))))

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

#endif // BCRUNTIME_TESTS_H
