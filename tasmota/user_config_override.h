#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_


// #define USE_BERRY_DEBUG

#define USE_VITO

#ifdef CODE_IMAGE_STR
  #undef CODE_IMAGE_STR
#endif
#define CODE_IMAGE_STR "TasmoCompiler-esp32generic"


#define USE_BERRY
#define USE_BERRY_PSRAM
#define USE_BERRY_DEBUG
#define USE_BERRY_INT64

#ifdef USE_BLE_ESP32
  #undef USE_BLE_ESP32
#endif
#define USE_BLE_ESP32

#ifdef USE_MI_ESP32
  #undef USE_MI_ESP32
#endif
#define USE_MI_ESP32

#ifdef USE_RC_SWITCH
  #undef USE_RC_SWITCH
#endif
#define USE_RC_SWITCH

#ifdef USE_RF_SENSOR
  #undef USE_RF_SENSOR
#endif
#define USE_RF_SENSOR


#define USE_ETHERNET
#define USE_WT32_ETH01                         // Add support for Wireless-Tag WT32-ETH01
//  #define ETH_TYPE          0                    // [EthType] 0 = ETH_PHY_LAN8720, 1 = ETH_PHY_TLK110/ETH_PHY_IP101, 2 = ETH_PHY_RTL8201, 3 = ETH_PHY_DP83848, 4 = ETH_PHY_DM9051, 5 = ETH_PHY_KSZ8081
//  #define ETH_ADDRESS       1                    // [EthAddress] 0 = PHY0 .. 31 = PHY31
//  #define ETH_CLKMODE       0                    // [EthClockMode] 0 = ETH_CLOCK_GPIO0_IN, 1 = ETH_CLOCK_GPIO0_OUT, 2 = ETH_CLOCK_GPIO16_OUT, 3 = ETH_CLOCK_GPIO17_OUT


#define USE_HOME_ASSISTANT

#ifdef MY_LANGUAGE
  #undef MY_LANGUAGE
#endif
#define MY_LANGUAGE	de_DE

#endif // _USER_CONFIG_OVERRIDE_H_
