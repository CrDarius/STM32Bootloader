/*Memory Regions*/
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 256K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
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
        _text_end = . ;
    } > FLASH

    .data :
    {
        . = ALIGN(4);
        _data_start = . ;
        *(.data*)
        _data_end = . ;
        . = ALIGN(4);
    } > SRAM AT > FLASH

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);
        _init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        _init_array_end = .;
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