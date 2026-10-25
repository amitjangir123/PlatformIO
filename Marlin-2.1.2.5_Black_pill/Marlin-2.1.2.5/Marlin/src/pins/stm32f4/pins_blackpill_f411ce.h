/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * 
 * 
 * 
 */

#define BOARD_INFO_NAME "Blackpill_f411ce"

#define DEFAULT_MACHINE_NAME "Blackpill_f411ce"

//
// Limit Switches
//
#define X_MIN_PIN                           PA13
#define Y_MIN_PIN                           PA14
#define Z_MIN_PIN                           PA15


//
// Steppers
//
#define X_STEP_PIN                          PB2
#define X_DIR_PIN                           PB1
#define X_ENABLE_PIN                        PB10

#define Y_STEP_PIN                          PB4
#define Y_DIR_PIN                           PB3
#define Y_ENABLE_PIN                        PB5

#define E0_STEP_PIN                          PB12
#define E0_DIR_PIN                           PB13
#define E0_ENABLE_PIN                        PB14

#define Z_STEP_PIN                         PC14
#define Z_DIR_PIN                          PC13
#define Z_ENABLE_PIN                       PC15

//
// Temperature Sensors
//
#define TEMP_0_PIN                          PA6   
#define TEMP_BED_PIN                        PA7   

//
// Heaters / Fans
//
#define HEATER_0_PIN                        PA0   // Heater0
#define HEATER_BED_PIN                      PA1   // Hotbed

#define FAN0_PIN                             PA2   // Fan0
#define FAN1_PIN                            PA3  // Fan1

//
// LCD / Controller
//

// SDCard
//#define SDSS                                PA4
//#define SD_DETECT_PIN                       PA10

// Rotary Encoder
//#define BTN_EN1                             PB13
//#define BTN_EN2                             PB12
//#define BTN_ENC                             PB14
