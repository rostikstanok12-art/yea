#pragma once
#include "memory.h"
void OxideESP_BlueStacks_Init(Memory& mem);
void ApplyNoRecoil_BlueStacks(Memory& mem, uintptr_t weaponConfig);
void ApplyNoSpread_BlueStacks(Memory& mem, uintptr_t weaponConfig);
bool GetViewMatrix_BlueStacks(Memory& mem, float* outMatrix);
