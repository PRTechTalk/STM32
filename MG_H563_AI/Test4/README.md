# NUCLEO-H563ZI Web Control

Bare-metal firmware for the ST NUCLEO-H563ZI. Mongoose's built-in STM32 Ethernet driver obtains an IPv4 address over DHCP and serves an embedded English web interface over plain HTTP. The interface independently controls the three onboard LEDs and polls the User button state every 500 ms.

## Project configuration

| Item | Configuration |
|---|---|
| Board / MCU | NUCLEO-H563ZI / STM32H563ZIT6 |
| CPU clock | 250 MHz |
| Network stack | Mongoose built-in TCP/IP and STM32H Ethernet driver; bare metal |
| Address mode | DHCP (IPv4) |
| Services | HTTP port 80 and REST/JSON API |
| Security | Plain HTTP; no TLS or authentication |
| Browser updates | 500 ms polling |
| Diagnostic UART | USART3, PD8 TX / PD9 RX, 115200 8N1 via ST-LINK VCP |
| LEDs | LD1 PB0 (green), LD2 PF4 (yellow), LD3 PG4 (red), active high |
| User button | PC13, polled, high means pressed |

MQTT, SNTP, TLS, authentication, OTA, mDNS, static addressing, WebSocket, and Server-Sent Events are intentionally not enabled.

## Exact tool and source versions

- STM32CubeMX 6.17.0, project-generator 4.10.0-B28
- STM32Cube firmware package H5 1.7.0
- STM32CubeIDE 2.1.1 bundled GNU Tools for STM32 14.3.rel1 (GCC 14.3.1)
- CMake 4.3.1 and Ninja 1.13.2
- Mongoose 7.22 source revision `fc699a0b0b976521f142540d195775ce67c75ebc` (2026-08-02)

Third-party licensing is in `Mongoose/LICENSE`.

## Build

```sh
cmake --preset Debug
cmake --build --preset Debug --clean-first -j 8
```

Outputs are deterministic:

- `build/Debug/NUCLEO_H563ZI_WebControl.elf`
- `build/Debug/NUCLEO_H563ZI_WebControl.bin`
- `build/Debug/NUCLEO_H563ZI_WebControl.hex`
- `build/Debug/NUCLEO_H563ZI_WebControl.map`

Use the `CMake: clean build` VS Code task or the `STM32Cube: ST-LINK` launch configuration with the installed STMicroelectronics STM32 extension.

## Regeneration

The `.ioc` file is the hardware source of truth. Regenerate with the installed CubeMX:

```sh
/Applications/STMicroelectronics/STM32CubeMX.app/Contents/MacOS/STM32CubeMX -q tools/cubemx_generate.script
```

Custom application code lives under `App/`, Mongoose under `Mongoose/`, and maintained web sources under `Web/`. Cube-generated source changes are restricted to `USER CODE` blocks. After every regeneration, confirm that the linker script still contains `.eth_ram`, 64 KiB heap, 8 KiB stack, and the RAM-overlap assertion, then perform a clean build.

Regenerate packed web assets after changing the HTML, CSS, JavaScript, or logo:

```sh
NODE_BIN=/Users/peter/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/bin/node tools/pack_web.sh
```

The packed table is explicitly activated before Mongoose initialization. The root URI is served explicitly as `/index.html` to avoid packed-filesystem path-joining ambiguity.

## REST API

- `GET /api/state` returns `{"leds":[false,false,false],"button":false}`.
- `POST /api/led/1`, `/api/led/2`, or `/api/led/3` accepts `{"on":true}` or `{"on":false}` and returns the complete new state.

Example:

```sh
curl -v http://BOARD_IP/api/state
curl -v -H 'Content-Type: application/json' -d '{"on":true}' http://BOARD_IP/api/led/1
```

## Hardware acceptance procedure

1. Flash the current ELF and open the ST-LINK VCP at 115200 8N1.
2. Confirm the startup banner, packed-filesystem activation, PHY identification, link state, DHCP progress, assigned IP, and successful port-80 listener.
3. From the same host, use the exact DHCP address with `curl -v http://BOARD_IP/api/state` and `curl -v http://BOARD_IP/`.
4. Open `http://BOARD_IP/` explicitly. Confirm `/app.css`, `/app.js`, and `/logo.jpg` return successfully.
5. Toggle each LED separately and verify LD1, LD2, and LD3 physically match the web state.
6. Press and release the blue User button and verify the UI changes within approximately 500 ms.
7. Disconnect/reconnect Ethernet and verify link and DHCP recovery in the UART log.

Compilation and static ELF/map verification do not prove physical UART, Ethernet, GPIO, or browser behavior. Those checks remain pending until performed on the board.

