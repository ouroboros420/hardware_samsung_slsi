//===-- CPU save and restore implementation ---------------------===//
//
//      Exynos SoC  -  PWR
//----------------------------------------------------------------------------
//      Name:    pwrDrvPwrGatingCPU.s
//      Purpose: To implement save and restore SFRs & GPRs (cortex-m4f) for power-gating
//      Rev.:    V1.00
//----------------------------------------------------------------------------
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
//===----------------------------------------------------------------------===//

#if defined(SOES)
    .syntax    unified
    .text
    .align	2
    .global	pwrDrvPwrGatingSaveCPU
    .thumb
    .thumb_func
    .type	pwrDrvPwrGatingSaveCPU, %function
#endif

pwrDrvPwrGatingSaveCPU:

// Save GPRs
    push      {lr}
    stmdb    sp!, {r0-r12}

// Save special registers values into stack
    mrs       r2, primask
    push      {r2}
    mrs       r2, faultmask
    push      {r2}
    mrs       r2, basepri
    push      {r2}
    mrs       r2, control
    push      {r2}

// Do we need to save & restore psr, apsr ???

// Save stack pointers into global var
    mrs       r2, msp
    str         r2, [r0]
    mrs       r2, psp
    str         r2, [r1]

    isb
    dsb
    dmb

    nop
    nop

go_wfi:
    wfi
    b go_wfi

#if defined(SEOS)
    .size pwrDrvPwrGatingSaveCPU, .-pwrDrvPwrGatingSaveCPU

    .syntax    unified
    .text
    .align	2
    .global	pwrDrvPwrGatingRestoreCPU
    .thumb
    .thumb_func
    .type	pwrDrvPwrGatingRestoreCPU, %function
#endif
pwrDrvPwrGatingRestoreCPU:

// Restore stack pointers from global var
    msr       msp, r0
    msr       psp, r1

// Restore special registers values from stack
    pop       {r2}
    msr       control, r2
    pop       {r2}
    msr       basepri, r2
    pop       {r2}
    msr       faultmask, r2
    pop       {r2}
    msr       primask, r2

// Restore GPRs

    ldmia    sp!, {r0-r12}
    pop      {pc}

#if defined(SEOS)
    .size pwrDrvPwrGatingRestoreCPU, .-pwrDrvPwrGatingRestoreCPU
    .end
#endif
