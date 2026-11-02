# UART Printf — NUCLEO-C562RE

STM32CubeMX2 UART printf example. The application prints a greeting over USART2,
then toggles PA5 and prints a message once per second.

## Project layout

- `UART_Printf.ioc2`: CubeMX2 configuration.
- `UART_Printf_cmake/`: generated CMake project and application source.

Keep the `.ioc2` file next to `UART_Printf_cmake/`: its CMake export output path
is `UART_Printf_cmake`. Open the `.ioc2` file in CubeMX2 to edit the configuration;
open `UART_Printf_cmake/` in the STM32 development environment to build and debug.

Use the STM32 tool bundle providing GCC 14.3.1, CMake and Ninja. From
`UART_Printf_cmake/`, with these tools on PATH:

```sh
cmake --preset debug_GCC_NUCLEO-C562RE --fresh
cmake --build --preset debug_GCC_NUCLEO-C562RE
```

Build output is excluded from Git. Reconfigure with `--fresh` after moving the
project, since an existing CMake cache can contain the previous absolute path.
