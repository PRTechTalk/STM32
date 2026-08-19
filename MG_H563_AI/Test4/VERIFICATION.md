# Build verification

This file records reproducible build/static checks separately from on-target validation.

## Build-verified

- Debug preset configures with ARM GCC 14.3.1.
- Clean Ninja build completes and emits ELF, BIN, HEX, and map outputs.
- The final Debug build uses 228,836 bytes of flash image, with 88,104 bytes of linked RAM sections before the linker-reserved heap and stack.
- Exactly one startup assembly source is compiled.
- The ELF is an ARM Cortex-M33 executable with `main`, `App_Init`, `mg_mgr_init`, the STM32H Mongoose driver, the HTTP listener, packed filesystem symbols, and exactly one linked `ETH_IRQHandler`.
- `.eth_ram` is 12,800 bytes at `0x200002e0`, is 32-byte aligned in internal DMA-accessible RAM, and contains Mongoose RX/TX descriptors and buffers.
- Effective linker reservations are 0x10000 bytes heap and 0x2000 bytes stack.
- The linker assertion prevents the static image plus minimum heap from overlapping the reserved stack.
- Packed `/index.html`, `/app.css`, `/app.js`, and `/logo.jpg` are retained in the ELF.
- Mongoose owns `ETH_IRQHandler`; the Cube HAL Ethernet IRQ handler is not retained in the final ELF.

## Pending physical validation

- ST-LINK VCP UART output
- PHY detection, link negotiation, DHCP lease, and link recovery
- Direct HTTP and browser requests
- Independent physical LED behavior
- User-button polarity and 500 ms browser update behavior
