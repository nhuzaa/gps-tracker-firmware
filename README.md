# GPS Tracker Firmware

[![PlatformIO](https://img.shields.io/badge/PlatformIO-SIM800-orange.svg)](https://platformio.org/)

This project contains firmware designed to transmit GPS location data via the SIM800 module. The purpose is to send the device's latitude and longitude information to a server.

## Features
- **Real-time Tracking**: Send GPS coordinates over the network for accurate location data.
- **Integration with SIM800**: Utilize the SIM800 module to handle data transmission.
- **Customizable**: Easily modify the code to suit specific server configurations or communication protocols.

## Project Structure

- **`.vscode`**: Configuration files for Visual Studio Code.
- **`include`**: Header files and other supporting files.
- **`lib`**: External libraries required for the project.
- **`src`**: Source code files.
- **`test`**: Unit test files.
- **`.gitignore`**: Lists files and directories ignored by Git.
- **`.travis.yml`**: Configuration file for Travis CI.
- **`README.md`**: Project description and usage guide.
- **`platformio.ini`**: PlatformIO build system configuration.

## Dependencies
- [PlatformIO](https://platformio.org/)
- [DFRobot_SIM808 Library](https://github.com/DFRobot/DFRobot_SIM808)

## Usage
1. **Setup PlatformIO**: Make sure PlatformIO is installed and set up correctly.
2. **Clone this Repository**: 
   ```bash
   git clone <repo_url>
   ```
3. **Install Libraries**:
   ```bash
   pio lib install
   ```
4. **Build and Upload**:
   - To build the project, run:
   ```bash
   pio run
   ```
   - To upload the firmware to your hardware, execute:
   ```bash
   pio run --target upload
   ```

5. **Test**:
   - To execute unit tests:
   ```bash
   pio test
   ```

## Contribution
- We welcome contributions! Feel free to submit issues or pull requests to enhance this project.

## License
- This project is licensed under the MIT License.

---

You can customize it further to reflect your specific requirements. Let me know if you'd like to add or modify any part of it.
