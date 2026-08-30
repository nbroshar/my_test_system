/* Minimal freestanding STM32F767 blink — no HAL, builds with arm-none-eabi-gcc.
 * Toggles PB0 (green LED) and prints "LED ON"/"LED OFF" on USART3.
 * Purpose: a self-contained firmware so CI can build + run it with no CubeMX. */
#include <stdint.h>

#define REG(a)        (*(volatile uint32_t *)(a))
#define RCC_AHB1ENR   REG(0x40023830u)
#define RCC_APB1ENR   REG(0x40023840u)
#define GPIOB_MODER   REG(0x40020400u)
#define GPIOB_ODR     REG(0x40020414u)
#define USART3_CR1    REG(0x40004800u)
#define USART3_BRR    REG(0x4000480Cu)
#define USART3_ISR    REG(0x4000481Cu)
#define USART3_TDR    REG(0x40004828u)

static void uart_puts(const char *s) {
    while (*s) {
        while (!(USART3_ISR & (1u << 7))) { }   /* wait for TXE */
        USART3_TDR = (uint32_t)(uint8_t)*s++;
    }
}

int main(void) {
    RCC_AHB1ENR |= (1u << 1);                    /* GPIOB clock  */
    RCC_APB1ENR |= (1u << 18);                   /* USART3 clock */
    GPIOB_MODER  = (GPIOB_MODER & ~(3u << 0)) | (1u << 0);  /* PB0 = output */
    USART3_BRR   = 0x1117u;                       /* ~115200 (Renode ignores) */
    USART3_CR1   = (1u << 3) | (1u << 0);         /* TE | UE */

    for (;;) {
        GPIOB_ODR ^= (1u << 0);
        uart_puts((GPIOB_ODR & 1u) ? "LED ON\r\n" : "LED OFF\r\n");
        for (volatile uint32_t i = 0; i < 200000u; i++) { }  /* crude delay */
    }
}

/* ---- startup ---- */
extern uint32_t _estack;
void Reset_Handler(void) {
    __asm volatile ("ldr sp, =_estack");         /* set MSP explicitly */
    main();
    for (;;) { }
}

__attribute__((section(".isr_vector"), used))
void *const g_vectors[] = {
    (void *)&_estack,        /* 0x00: initial stack pointer */
    (void *)Reset_Handler,   /* 0x04: reset vector          */
};
