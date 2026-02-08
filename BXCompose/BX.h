#ifndef BCOMPOSE_H
#define BCOMPOSE_H

#include "BX_Settings.h"
#include "BX_Types.h"

#include "Core/BX_Context.h"
#include "Core/BX_Node.h"
#include "Core/BX_Scope.h"

#include "State/BX_Remember.h"
#include "State/BX_MutableState.h"

#include "Render/BX_Window.h"
#include "Render/BX_Canvas.h"

#include "Composables/BX_Layout.h"
#include "Composables/BX_Text.h"

// =========================================================
// MARK: Runtime Initialization
// =========================================================

void BX_Initialize(void);
void BX_Deinitialize(void);

#endif //BCOMPOSE_H
