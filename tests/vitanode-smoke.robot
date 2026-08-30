*** Settings ***
Documentation     VitalNode CI smoke test: boot the firmware on the Renode
...               STM32F767 model and assert LED telemetry on USART3.
...               Exits non-zero if the asserts fail — the PR merge gate.
Suite Setup       Setup
Suite Teardown    Teardown
Test Teardown     Test Teardown
Resource          ${RENODEKEYWORDS}

*** Variables ***
${PLATFORM}       ${CURDIR}/../platforms/nucleo_f767_ci.repl
${ELF}            ${CURDIR}/../firmware/blink.elf
${UART}           sysbus.usart3

*** Test Cases ***
Boots And Emits Telemetry On USART3
    [Documentation]    Fails the PR if the firmware does not boot and print.
    Execute Command             mach create
    Execute Command             machine LoadPlatformDescription @${PLATFORM}
    Execute Command             sysbus LoadELF @${ELF}
    Create Terminal Tester      ${UART}
    Start Emulation
    Wait For Line On Uart       LED ON       timeout=10
    Wait For Line On Uart       LED OFF      timeout=10

Scheduler Keeps Toggling
    [Documentation]    Confirms it keeps running, not a one-shot print.
    Execute Command             mach create
    Execute Command             machine LoadPlatformDescription @${PLATFORM}
    Execute Command             sysbus LoadELF @${ELF}
    Create Terminal Tester      ${UART}
    Start Emulation
    Wait For Line On Uart       BANANA       timeout=10
    Wait For Line On Uart       LED ON       timeout=10
