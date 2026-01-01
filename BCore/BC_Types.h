#ifndef BCORE_TYPES_H
#define BCORE_TYPES_H

// =========================================================
// MARK: Primitive Types
// =========================================================

typedef _Bool BC_bool;
#define BC_true ((BC_bool)(1))
#define BC_false ((BC_bool)(0))

// =========================================================
// MARK: Ref Types
// =========================================================

typedef struct BC_Allocator* BC_AllocatorRef;
typedef struct BC_Arena* BC_ArenaRef;

#endif //BCORE_TYPES_H