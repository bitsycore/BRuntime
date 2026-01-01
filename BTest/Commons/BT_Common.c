#include "BT_Common.h"

void BT_PrintBigTitle(const char* _x_) {
	BT_Print("\n"
		BC_AE_BGREEN "╭───────────────────────────────────────────────────────────────╮" BC_AE_RESET "\n"
		BC_AE_BGREEN "│"BC_AE_BOLD"                 %-45s " BC_AE_RESET BC_AE_BGREEN "│" BC_AE_RESET"\n"
		BC_AE_BGREEN "╰───────────────────────────────────────────────────────────────╯" BC_AE_RESET "\n",
		_x_
	);
}

void BT_PrintSubTitle(const char* _x_) {
	BT_Print("\n"
		BC_AE_BYELLOW "╭┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄╮" BC_AE_RESET "\n"
		BC_AE_BYELLOW "┊"BC_AE_ITALIC"   %-47s " BC_AE_RESET BC_AE_BYELLOW"┊" BC_AE_RESET"\n"
		BC_AE_BYELLOW "╰┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄╯" BC_AE_RESET "\n\n",
		_x_
	);
}

double BT_GetTimeMicroseconds(clock_t start, clock_t end) {
	return ((double)(end - start) / CLOCKS_PER_SEC) * 1000000.0;
}