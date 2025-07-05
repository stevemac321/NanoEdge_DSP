/*
 * ram_test.c
 *
 *  Created on: Jul 2, 2025
 *      Author: steve
 */
#include <dsp_test.h>
#include <stdio.h>
#include <stdint.h>

extern uint32_t _estack; // Defined in linker script
extern uint32_t _estack;
extern uint32_t _stack_start;

extern float buffer0_128[128];

#define STACK_START  ((uint32_t)&_stack_start)
#define STACK_END    ((uint32_t)&_estack)
#define STACK_SIZE_BYTES   (STACK_END - STACK_START)

//#define STACK_SIZE_BYTES  0x1000  // Set according to your actual stack size

#define STACK_PATTERN     0xA5A5A5A5

void fill_stack_pattern(void)
{
    uint32_t* sp = (uint32_t*)((uint8_t*)&_estack - STACK_SIZE_BYTES);
    while (sp < (uint32_t*)&_estack) {
        *sp++ = STACK_PATTERN;
    }
}

// Returns how many bytes of stack have been used
size_t get_stack_usage_bytes(void)
{
    uint32_t* sp = (uint32_t*)((uint8_t*)&_estack - STACK_SIZE_BYTES);
    while (sp < (uint32_t*)&_estack) {
        if (*sp != STACK_PATTERN) break;
        sp++;
    }

    return (uint8_t*)&_estack - (uint8_t*)sp;
}

void test_ram_usage(void)
{
    static float filtered_stack_usage = 0.0f;
    size_t stack_used = 0;

#ifdef SELF_DIAG_MODE
    struct ru_vec vstack;
    ru_vec_init(&vstack, buffer0_128, 128, 128);
    for(int i=0; i < 128; i++) {
#endif
    stack_used = get_stack_usage_bytes();
    filtered_stack_usage = 0.9f * filtered_stack_usage + 0.1f * (float)stack_used;
#ifdef SELF_DIAG_MODE
    float normalized = fminf(fmaxf(filtered_stack_usage / (float)STACK_SIZE_BYTES, 0.0f), 1.0f);
       vstack.pbuf[i] = normalized;
    }
    one_inference(vstack.pbuf, "RAM from ram_test.c");
#endif

    printf("RAM Usage Test:\n");
    printf("  Stack used: %d bytes\n", (int)stack_used);
    printf("  Filtered Stack Usage: %.1f bytes\n", filtered_stack_usage);

    if (stack_used > 0.9 * STACK_SIZE_BYTES) {
        printf("  WARNING: High stack usage!\n");
    }

}
