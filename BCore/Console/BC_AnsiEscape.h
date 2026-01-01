#ifndef BCORE_ANSI_ESCAPE_H
#define BCORE_ANSI_ESCAPE_H

#define BC_AE_RESET   "\x1b[0m"
#define BC_AE_END     BC_AE_RESET

#define BC_AE_BOLD       "\x1b[1m"
#define BC_AE_FAINT      "\x1b[2m"
#define BC_AE_ITALIC     "\x1b[3m"
#define BC_AE_UNDERLINE  "\x1b[4m"
#define BC_AE_BLINK      "\x1b[5m"
#define BC_AE_RAPIDBLINK "\x1b[6m"
#define BC_AE_REVERSE    "\x1b[7m"
#define BC_AE_CONCEAL    "\x1b[8m"
#define BC_AE_STRIKE     "\x1b[9m"
#define BC_AE_DOUBLEUNDER "\x1b[21m"
#define BC_AE_NORMALINT  "\x1b[22m"
#define BC_AE_OVERLINE   "\x1b[53m"

#define BC_AE_BLACK   "\x1b[30m"
#define BC_AE_RED     "\x1b[31m"
#define BC_AE_GREEN   "\x1b[32m"
#define BC_AE_YELLOW  "\x1b[33m"
#define BC_AE_BLUE    "\x1b[34m"
#define BC_AE_MAGENTA "\x1b[35m"
#define BC_AE_CYAN    "\x1b[36m"
#define BC_AE_WHITE   "\x1b[37m"

#define BC_AE_BBLACK   "\x1b[90m"
#define BC_AE_BRED     "\x1b[91m"
#define BC_AE_BGREEN   "\x1b[92m"
#define BC_AE_BYELLOW  "\x1b[93m"
#define BC_AE_BBLUE    "\x1b[94m"
#define BC_AE_BMAGENTA "\x1b[95m"
#define BC_AE_BCYAN    "\x1b[96m"
#define BC_AE_BWHITE   "\x1b[97m"

#define BC_AE_BG_BLACK   "\x1b[40m"
#define BC_AE_BG_RED     "\x1b[41m"
#define BC_AE_BG_GREEN   "\x1b[42m"
#define BC_AE_BG_YELLOW  "\x1b[43m"
#define BC_AE_BG_BLUE    "\x1b[44m"
#define BC_AE_BG_MAGENTA "\x1b[45m"
#define BC_AE_BG_CYAN    "\x1b[46m"
#define BC_AE_BG_WHITE   "\x1b[47m"

#define BC_AE_BG_BBLACK   "\x1b[100m"
#define BC_AE_BG_BRED     "\x1b[101m"
#define BC_AE_BG_BGREEN   "\x1b[102m"
#define BC_AE_BG_BYELLOW  "\x1b[103m"
#define BC_AE_BG_BBLUE    "\x1b[104m"
#define BC_AE_BG_BMAGENTA "\x1b[105m"
#define BC_AE_BG_BCYAN    "\x1b[106m"
#define BC_AE_BG_BWHITE   "\x1b[107m"

#define BC_AE_FG256(n) "\x1b[38;5;" #n "m"
#define BC_AE_BG256(n) "\x1b[48;5;" #n "m"

#define BC_AE_RGB_FG(r,g,b) "\x1b[38;2;" #r ";" #g ";" #b "m"
#define BC_AE_RGB_BG(r,g,b) "\x1b[48;2;" #r ";" #g ";" #b "m"

#endif //BCORE_ANSI_ESCAPE_H