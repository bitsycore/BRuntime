#include "BC.h"

extern void _BCNumberInitialize();

__attribute__((constructor))
void BCInitialize(void) {
	_BCNumberInitialize();
}

__attribute__((destructor))
void BCUninitilize(void) {

}