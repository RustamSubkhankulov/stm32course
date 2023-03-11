# Two slightly different syntaxes are support for ARM and THUMB instructions. 
# The default, divided, uses the old style where ARM and THUMB instructions had 
# their own, separate syntaxes. The new, unified syntax, which can be selected 
# via the .syntax directive, and has the following main features:
# - Immediate operands do not require a # prefix.
# - The IT instruction may appear, and if it does 
#   it is validated against subsequent conditional affixes. 
# - All instructions set the flags if and only if they have an s affix.
# - The .N and .W suffixes are recognized and honored.

.section .text

# -----------------------------------------------
# void timing_perfect_delay(uint32_t decamicros);
# Make a delay for 10 * 'decamicros' microsecs
# -----------------------------------------------

.syntax unified
.thumb_func
.global timing_perfect_delay
.type timing_perfect_delay, %function

timing_perfect_delay:

    # saving prev fp onto stack
    mov r1, r11
    push {r1}

    # setting up new fp
    mov r11, sp

.loop:  
    cmp r0, 0       
    beq .exit      

    ldr r2, =mcs_n_cycles
    ldr r1, [r2]

    .cycle:
        subs r1, r1, 1
        cmp r1, 0
        bne .cycle

    subs r0, r0, #1
    b .loop

.exit:

    # readjust rsp
    mov sp, r11

    # restore prev fp from stack
    pop {r1}
    mov r11, r1  

    bx lr     

# CPU_FREQ == 48000000
# due to various CPU optimizations mcs_n_cycles = (CPU_FREQ) / (1000000 * 5)
#                                                                         ^ CPU cycles per inner loop cycle
# mls_n_cycles:       .word 9600
mcs_n_cycles:       .word 96
