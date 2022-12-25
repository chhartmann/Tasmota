#ifdef USE_VITO

#include "VitoWiFi.h"
#include <string>

#define XSNS_98              98

VitoWiFi_setProtocol(P300);

DPTemp outsideTemp("outsideTemp", "boiler", 0x5525);
DPTemp boilerTemp("boilertemp", "boiler", 0x0810);
DPStat pumpStat("pump", "heating1", 0x2906);

class LogPrinter : public Print {
  std::string line;
  void print_on_newline() {
    if (line.back() == '\n') {
      AddLog(LOG_LEVEL_INFO, "VTO: %s", (const char*)line.c_str());
      line.clear();
    }
  }
  virtual size_t write(uint8_t c) {
    line += (char)c;
    print_on_newline();
    return 1;
  };
  virtual size_t write(const uint8_t *buffer, size_t size) {
    line += (char*)buffer;
    print_on_newline();
    return size;
  };
};

LogPrinter logPrinter;

void VitoCommandRead() {
  if (strlen(XdrvMailbox.data) == 0) {
    VitoWiFi.readAll();
    ResponseCmndChar("ReadAll");
  } else {
    VitoWiFi.readGroup(XdrvMailbox.data);
    ResponseCmndChar("ReadGroup");
  }
}

void VitoCommandLogOn() {
  VitoWiFi.enableLogger();
}

void VitoCommandLogOff() {
  VitoWiFi.disableLogger();
}

const char VitoCommandsString[] PROGMEM = "Vito|Read|LogOn|LogOff";
void (* const VitoCommandsList[])(void) PROGMEM = { &VitoCommandRead, &VitoCommandLogOn, &VitoCommandLogOff };

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
    case FUNC_INIT:
      ClaimSerial();
      VitoWiFi.setLogger(&logPrinter);
      VitoWiFi.enableLogger();
      VitoWiFi.setup(&Serial);
      VitoWiFi.setGlobalCallback(globalCallbackHandler);
      break;
    case FUNC_EVERY_50_MSECOND:
      VitoWiFi.loop();
      break;
    case FUNC_EVERY_SECOND:
      break;
    case FUNC_COMMAND_SENSOR:
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
