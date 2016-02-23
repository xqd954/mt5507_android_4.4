    .text
    .align
    .global         pow
    .type           pow, %function
pow:
    .fnstart

.LFilterRange:
    movw            ip, #0x0000
    movt            ip, #0x4010
    vmov            d1, r2, r3
    cmp             r1, ip
    cmple           r3, ip
    movt            ip, #0x3f80
    bgt             .LGotoKernel
    cmp             r1, ip
    cmpge           r3, ip
    blt             .LGotoKernel

.LFilterSpecial:
    cmp             r2, #0
    beq             .LSpecialY

.LNormalize:
    ubfx            r2, r1, #0, #20
    movw            r3, #0xB67A
    movt            r3, #0x000B
    cmp             r2, r3
    bge             .LNormalizeX2
    movw            r3, #0x988E
    movt            r3, #0x0003
    cmp             r2, r3
    movt            ip, #0x3ff0
    mov             r3, r1, asr #20
    orr             r1, r2, ip
    fconstdle       d3, #0x70
    fconstdgt       d3, #0x78
    vmov            d0, r0, r1
    vsub.f64        d31, d0,  d3
    vadd.f64        d30, d0,  d3
    vdiv.f64        d17, d31, d30
    sub             r3, r3, ip, asr #20
    vldrle          d4, .LDLogDP0
    fmsr            s4, r3
    vldrgt          d4, .LDLogDP
    fsitod          d2, s4

.LLog:
    vldr            d5,  .LDLogCP
    vldr            d22, .LDLogL5
    vldr            d24, .LDLogL4
    vldr            d26, .LDLogL3
    vldr            d28, .LDLogL2
    vldr            d30, .LDLogL1
    fconstd         d23, #0x08
    vmul.f64        d20, d17, d17
    vmul.f64        d29, d1,  d5
    vmla.f64        d24, d20, d22
    vmul.f64        d18, d20, d20
    vmul.f64        d27, d29, d17
    vmul.f64        d31, d1,  d2
    vmla.f64        d26, d20, d24
    vadd.f64        d25, d23, d20
    vmla.f64        d31, d1,  d4
    vmla.f64        d28, d20, d26
    vmul.f64        d16, d27, d18
    vmla.f64        d31, d25, d27
    vmla.f64        d30, d20, d28
    vldr            d17, .LDln2
    vmla.f64        d31, d16, d30
    vmov            d2, d31
    vmov            d16, d31
.LSplitZ:
    vcvt.s32.f64    d2,  d2, #1
    vshl.u32        d3,  d2, #31
    vmvn.u32        d4,  d2
    vand.u32        d3,  d3, d4
    vbic.s32        d2,  d2, #1
    vshr.u32        d3,  d3, #30
    vadd.s32        d2,  d2, d3
    vcvt.f64.s32    d2,  d2, #1
    vsub.f64        d16, d16, d2
.LPowerOfTwo:
    vldr            d31, .LDExpE12
    vldr            d29, .LDExpE11
    vldr            d27, .LDExpE10
    vldr            d25, .LDExpE9
    vldr            d30, .LDExpE8
    vldr            d28, .LDExpE7 
    vldr            d26, .LDExpE5
    vmul.f64        d1,  d16, d17
    fconstd         d4,  #0x70
    vldr            d24, .LDExpE4
    vldr            d23, .LDExpE6
    vmla.f64        d29, d1,  d31
    vadd.f64        d16, d1,  d4
    vmul.f64        d3,  d1,  d1
    vldr.f64        d22, .LDExpE2
    vmla.f64        d27, d1,  d29
    vmla.f64        d28, d1,  d30
    vmul.f64        d7,  d3,  d3
    vmul.f64        d5,  d3,  d1
    vmla.f64        d25, d1,  d27
    vmla.f64        d26, d3,  d28
    vldr.f64        d21, .LDExpE3
    vmla.f64        d16, d7,  d24
    vmla.f64        d23, d5,  d25
    vmla.f64        d22, d5,  d26
    vmla.f64        d21, d5,  d23
    vmla.f64        d16, d3,  d22
    vmla.f64        d16, d5,  d21

.LScaleBack:
    ftosid          s4, d2
    fmrs            r2, s4
    vmov            r0, r1, d16
    add             r1, r1, r2, asl #20
    bx              lr

.LNormalizeX2:
    movt            ip, #0x3ff0
    mov             r3, r1, asr #20
    orr             r1, r2, ip
    sub             r3, r3, ip, asr #20
    movt            ip, #0x0010
    sub             r1, r1, ip
    fconstd         d3, #0x70
    vmov            d0, r0, r1
    fmsr            s4, r3
    vsub.f64        d31, d0, d3
    vadd.f64        d30, d0, d3
    fsitod          d2, s4
    vdiv.f64        d17, d31, d30
    vsub.f64        d4, d3, d3
    vadd.f64        d2, d2, d3
    b               .LLog

.LSpecialY:
    vmov            d31, r0, r1
    movt            ip, #0x3FF0
    cmp             r3, #0
    vsqrt.f64       d30, d31
    vmul.f64        d29, d31, d31
    fconstd         d28, #0x70
    beq             .LSpecialY_Y0
    cmp             r3, ip
    movt            ip, #0x4000
    beq             .LSpecialY_Y1
    cmp             r3, ip
    movt            ip, #0x3FD0
    beq             .LSpecialY_Y2
    cmp             r3, ip
    movt            ip, #0x3FE0
    beq             .LSpecialY_Y1to4
    cmp             r3, ip
    bne             .LNormalize
.LSpecialY_Y1to2:
    vmov            r0, r1, d30
    bx              lr
.LSpecialY_Y1to4:
    vsqrt.f64       d31, d30
    vmov            r0, r1, d31
    bx              lr
.LSpecialY_Y0:
    vmov            r0, r1, d28
    bx              lr
.LSpecialY_Y1:
    bx              lr
.LSpecialY_Y2:
    vmov            r0, r1, d29
    bx              lr
.LGotoKernel:
    vmov            r2, r3, d1
    b               pow_kernel(PLT)

    .align  6
.LDLogCP:
    .word   0xDC3A03FD
    .word   0x3FEEC709
.LDLogL5:
    .word   0x93C9DB65
    .word   0x3FCD864A
.LDLogL4:
    .word   0xA91D4101
    .word   0x3FD17460
.LDLogL3:
    .word   0x518F264D
    .word   0x3FD55555
.LDLogL2:
    .word   0xDB6FABFF
    .word   0x3FDB6DB6
.LDLogL1:
    .word   0x33333303
    .word   0x3FE33333
.LDLogDP:
    .word   0x473F7AD1
    .word   0x3FE2B803
.LDLogDP0:
    .word   0x00000000
    .word   0x00000000
.LDln2:
    .word   0xFEFA39EF
    .word   0x3FE62E42
.LDExpE12:
    .word   0xeff8d898
    .word   0x3e21eed8
.LDExpE11:
    .word   0x67f544e4
    .word   0x3e5ae645
.LDExpE10:
    .word   0xb7789f5c
    .word   0x3e927e4f
.LDExpE9:
    .word   0xa556c734
    .word   0x3ec71de3
.LDExpE8:
    .word   0x1a01a01a
    .word   0x3efa01a0
.LDExpE7:
    .word   0x1a01a01a
    .word   0x3f2a01a0
.LDExpE6:
    .word   0x16c16c17
    .word   0x3f56c16c
.LDExpE5:
    .word   0x11111111
    .word   0x3f811111
.LDExpE4:
    .word   0x55555555
    .word   0x3fa55555
.LDExpE3:
    .word   0x55555555
    .word   0x3fc55555
.LDExpE2:
    .word   0x00000000
    .word   0x3fe00000

    .fnend
    .size   pow, .-pow
