# PR TechTalk STM32 Projects

Welcome to the STM32 project repository for the PR TechTalk YouTube channel.

This repository contains STM32 example projects, experiments, and tutorial code used in my videos. 
The projects are mainly developed with Visual Studio Code, STM32CubeMX, CMake, and the STM32 HAL.
But olde projects are STM32CubeIDE and MX,

## Projects

| Project | Description |
|---|---|
| [1_ThreadX_Blinky](./1_ThreadX_Blinky/) | Introductory ThreadX project with a simple LED blinky example. |
| [2_ThreadX_Semaphore](./2_ThreadX_Semaphore/) | ThreadX example demonstrating task synchronization with a semaphore. |
| [3_ThreadX_Mailbox](./3_ThreadX_Mailbox/) | ThreadX example demonstrating message exchange with a mailbox. |
| [DTS_Sensor](./DTS_Sensor/) | STM32 project for working with a digital temperature sensor. |
| [G071_LCD_RTC](./G071_LCD_RTC/) | STM32G071 project using an LCD together with the real-time clock. |
| [SD_Card_SPI_FatFs](./SD_Card_SPI_FatFs/) | SD card project using SPI and the FatFs file system. |

More projects will be added as new tutorials are published.

## Development Tools

The projects may use some or all of the following tools:

- Visual Studio Code
- STM32 VS Code Extension
- STM32CubeMX & STM32CubeMX2
- STM32CubeCLT
- CMake
- STM32 HAL
- Azure RTOS ThreadX
- FatFs
- Mongoose.ws

## Getting Started

1. Clone this repository from GitHub.
2. Open the folder for the project you want to use in Visual Studio Code.
3. Check the project-specific files and documentation for hardware and build requirements.
4. Configure and build the project using the tools required by that project.

Different projects may require different STM32 boards, toolchain versions, or STM32Cube firmware packages.

## Repository Structure

```text
STM32/
├── README.md
├── .gitignore
├── 1_ThreadX_Blinky/
├── 2_ThreadX_Semaphore/
├── 3_ThreadX_Mailbox/
├── DTS_Sensor/
├── G071_LCD_RTC/
└── SD_Card_SPI_FatFs/
```

Build output and IDE-generated files are excluded using `.gitignore`.

## YouTube Channel

Tutorials and project demonstrations are published on [PR TechTalk on YouTube](https://www.youtube.com/@prtechtalk416).

## Disclaimer

These projects are provided for educational and experimental purposes. Always verify pin assignments, voltage levels, and hardware connections before connecting external hardware to an STM32 board.

## License

This repository is licensed under the MIT License unless otherwise stated inside an individual project directory.
