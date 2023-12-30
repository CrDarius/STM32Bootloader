/*Memory Regions*/
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

/*Entry point*/
ENTRY(_Z13Reset_Handlerv)

/*Define Sections*/
SECTIONS
{
    /* The startup code into "FLASH" Rom type memory */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Startup code */
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text*)
        . = ALIGN(4);
    } > FLASH

    .data :
    {
        . = ALIGN(4);
        *(.data*)
        . = ALIGN(4);
    } > SRAM AT > FLASH

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    .bss :
    {
        . = ALIGN(4);
        _bss_start = . ;
        *(.bss*)
        _bss_end = . ;
        end = _bss_end;
        . = ALIGN(4);
    } > SRAM


}