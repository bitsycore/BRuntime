#include "BCRuntime.h"

extern void _BCNumberInitialize();

void __internal_BCInitialize(void) {
	_BCNumberInitialize();
}

void __internal_BCUninitialize(void) {

}