#include "BT_Common.h"

void BIG_TITLE(const char* _x_) {
	log_fmt("\n"
		BC_AE_BGREEN "╭───────────────────────────────────────────────────────────────╮" BC_AE_RESET "\n"
		BC_AE_BGREEN "│"BC_AE_BOLD"                 %-45s " BC_AE_RESET BC_AE_BGREEN "│" BC_AE_RESET"\n"
		BC_AE_BGREEN "╰───────────────────────────────────────────────────────────────╯" BC_AE_RESET "\n",
		_x_
	);
}

void SUB_TITLE(const char* _x_) {
	log_fmt("\n"
		BC_AE_BYELLOW "╭┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄╮" BC_AE_RESET "\n"
		BC_AE_BYELLOW "┊"BC_AE_ITALIC"   %-47s " BC_AE_RESET BC_AE_BYELLOW"┊" BC_AE_RESET"\n"
		BC_AE_BYELLOW "╰┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄╯" BC_AE_RESET "\n\n",
		_x_
	);
}