#ifdef USE_VITO

#include "VitoWiFi.h"

#define XSNS_98              98

VitoWiFi_setProtocol(P300);

DPTemp outsideTemp("outsideTemp", "boiler", 0x5525);
DPTemp boilerTemp("boilertemp", "boiler", 0x0810);
DPStat pumpStat("pump", "heating1", 0x2906);

class LogPrinter : public Print {

    virtual size_t write(uint8_t) {
      AddLog(LOG_LEVEL_INFO, PSTR("VTO: not implemented"));
      return 1;
    };
    virtual size_t write(const uint8_t *buffer, size_t size) {
      AddLog(LOG_LEVEL_INFO, "VTO: %s", (const char*)buffer);
      return size;
    };
};

LogPrinter logPrinter;

void VitoCommand() {
  VitoWiFi.readAll();
  AddLog(LOG_LEVEL_INFO, PSTR("VTO: Command received"));
  ResponseCmndChar("yo");
}

const char VitoCommandsString[] PROGMEM = "Vito|DoIt|Dump";
void (* const VitoCommandsList[])(void) PROGMEM = { &VitoCommand, &VitoCommand };

void globalCallbackHandler(const class IDatapoint& dp, class DPValue value) {
  char value_str[15] = {0};
  value.getString(value_str, sizeof(value_str));
  AddLog(LOG_LEVEL_INFO, "VTO: %s %s %s", dp.getGroup(), dp.getName(), value_str);
}


/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xsns98(uint8_t function) {
  bool result = false;

  if (FUNC_INIT == function) {
  }
  switch (function) {
    case FUNC_PRE_INIT:
      ClaimSerial();
    case FUNC_INIT:
      VitoWiFi.setLogger(&logPrinter);
      VitoWiFi.enableLogger();
      VitoWiFi.setup(&Serial);
      VitoWiFi.setGlobalCallback(globalCallbackHandler);
      break;
    case FUNC_EVERY_50_MSECOND:
      break;
    case FUNC_EVERY_SECOND:
      VitoWiFi.loop();
      break;
    case FUNC_COMMAND_SENSOR:
      if (XSNS_98 == XdrvMailbox.index) {
        VitoCommand();
        result = true;
      }
      break;
      case FUNC_COMMAND:
        DecodeCommand(VitoCommandsString, VitoCommandsList);
        result = true;
        break;
#ifdef USE_WEBSERVER
    case FUNC_WEB_SENSOR:
      WSContentSend_P("Hello World");
      break;
#endif  // USE_WEBSERVER
  }
  return result;
}

#endif  // USE_VITO
