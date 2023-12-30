#ifdef USE_VITO

#include "VitoWiFi.h"
#include <string>

#define XSNS_98              98

// TODOs:
// - Fix datapoint addresses
// - Send response via MQTT
// - Add possibility to set system time to current time
// - Add visualisation in web UI of values defined in a list

VitoWiFi_setProtocol(P300);

DPTemp VitoTempAussen("Aussentemperatur", "temperatures", 0x0800);
DPTemp VitoTempSpeicher("Wassertemperatur", "temperatures", 0x0812);
DPTemp VitoTempVLSoll("Soll-Vorlauftemperatur", "temperatures", 0x2544);
DPTemp VitoTempRLIst("Ist-Ruecklauftemperatur", "temperatures", 0x0808);
DPTemp VitoTempKesselSoll("Soll-Kesseltemperatur", "temperatures", 0x555A);
DPUnsignedInt VitoStartsBrenner("pump", "burner", 0x088A);
DPHours VitoLaufzeitBrenner("pump", "burner", 0x08A7);

DPByte VitoStatusStoerung("Status", "status", 0x08A2);
DPTimeStamp VitoSystemzeit("Systemzeit", "status", 0x088E);

DPErrHist getVitoStoerung1("Stoerung-History1", "error-history", 0x7507);
DPErrHist getVitoStoerung2("Stoerung-History2", "error-history", 0x7510);
DPErrHist getVitoStoerung3("Stoerung-History3", "error-history", 0x7519);
DPErrHist getVitoStoerung4("Stoerung-History4", "error-history", 0x7522);
DPErrHist getVitoStoerung5("Stoerung-History5", "error-history", 0x752B);
DPErrHist getVitoStoerung6("Stoerung-History6", "error-history", 0x7534);
DPErrHist getVitoStoerung7("Stoerung-History7", "error-history", 0x753D);
DPErrHist getVitoStoerung8("Stoerung-History8", "error-history", 0x7546);
DPErrHist getVitoStoerung9("Stoerung-History9", "error-history", 0x754F);
DPErrHist getVitoStoerung10("Stoerung-History10", "error-history", 0x7558);

DPCycleTime VitoTimerMoHeizen("Heizen-Mo", "timer-heating", 0x2000);
DPCycleTime VitoTimerDiHeizen("Heizen-Di", "timer-heating", 0x2008);
DPCycleTime VitoTimerMiHeizen("Heizen-Mi", "timer-heating", 0x2010);
DPCycleTime VitoTimerDoHeizen("Heizen-Do", "timer-heating", 0x2008);
DPCycleTime VitoTimerFrHeizen("Heizen-Fr", "timer-heating", 0x2020);
DPCycleTime VitoTimerSaHeizen("Heizen-Sa", "timer-heating", 0x2008);
DPCycleTime VitoTimerSoHeizen("Heizen-So", "timer-heating", 0x2030);

DPCycleTime VitoTimerMoWW("Warmwasser-Mo", "timer-water", 0x2100);
DPCycleTime VitoTimerDiWW("Warmwasser-Di", "timer-water", 0x2108);
DPCycleTime VitoTimerMiWW("Warmwasser-Mi", "timer-water", 0x2110);
DPCycleTime VitoTimerDoWW("Warmwasser-Do", "timer-water", 0x2108);
DPCycleTime VitoTimerFrWW("Warmwasser-Fr", "timer-water", 0x2120);
DPCycleTime VitoTimerSaWW("Warmwasser-Sa", "timer-water", 0x2108);
DPCycleTime VitoTimerSoWW("Warmwasser-So", "timer-water", 0x2130);

DPValue backupVal;
IDatapoint* backupDp = nullptr;

class LogPrinter : public Print {
  std::string line;
  void print_on_newline() {
    if (line.back() == '\n') {
      line.pop_back();
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
    line.append((char*)buffer, size);
    print_on_newline();
    return size;
  };
};

LogPrinter logPrinter;

void VitoCommandRead() {
  if (strcmp(XdrvMailbox.data, "all") == 0) {
    VitoWiFi.readAll();
    ResponseCmndDone();
  } else {
    const std::vector<IDatapoint*>& v = VitoTempAussen.getCollection();
    bool found = false;
    for (uint8_t i = 0; (i < v.size()) && (found == false); ++i) {
      if (strcmp(XdrvMailbox.data, v[i]->getName()) == 0) {
        VitoWiFi.readDatapoint(*v[i]);
        found = true;
      } else if (strcmp(XdrvMailbox.data, v[i]->getGroup()) == 0) {
        VitoWiFi.readGroup(XdrvMailbox.data);
        found = true;
      }
    }
    if (found) {
      ResponseCmndDone();
    }
  }
}

void VitoCommandWriteTimer() {
  if (ArgC() > 0) {
    char sub_string[XdrvMailbox.data_len];
    (void)ArgV(sub_string, 1);

    const std::vector<IDatapoint*>& v = VitoTempAussen.getCollection();
    IDatapoint* datapoint = nullptr;
    for (uint8_t i = 0; i < (v.size()) && (datapoint == nullptr); ++i) {
      if (strcmp(sub_string, v[i]->getName()) == 0) {
        datapoint = v[i];
      }
    }

    if (datapoint != nullptr) {
      cycletime_s ct;
      const char* argv = nullptr;
      const char* defaultArg = "FF:FF-FF:FF";
      for (uint8_t para = 0; para < 4; ++para) {
        argv = defaultArg;
        if (ArgC() > (para + 1)) {
          argv = ArgV(sub_string, 2 + para);
        };

        int v1, v2, v3, v4;
        sscanf(argv, "%02x:%02x-%02x:%02x", &v1, &v2, &v3, &v4);
        ct.cycle[para].from_hour = v1;
        ct.cycle[para].from_minute = v2;
        ct.cycle[para].till_hour = v3;
        ct.cycle[para].till_minute = v4;
      }

      backupDp = datapoint;
      backupVal = datapoint->getLastValue();

      VitoWiFi.writeDatapoint(*datapoint, DPValue(ct));
      ResponseCmndDone();

      // debug log
      // char c[60];
      // size_t offset = 0;
      // for (uint8_t para = 0; para < 4; ++para) {
      //   uint8_t from_hour = ct.cycle[para].from_hour;
      //   uint8_t from_minute = ct.cycle[para].from_minute;
      //   uint8_t till_hour = ct.cycle[para].till_hour;
      //   uint8_t till_minute = ct.cycle[para].till_minute;
      //   offset += sprintf(c + offset, "%02X:%02X-%02X:%02X ", from_hour, from_minute, till_hour, till_minute);
      // }
      // c[offset - 1] = '\0';
      // AddLog(LOG_LEVEL_INFO, "VTO: Write %s", c);
    }
  }
}

void VitoCommandRestoreTimer() {
  if ((backupDp != nullptr) && (backupVal.getType() == CYCLETIME_T)) {
    VitoWiFi.writeDatapoint(*backupDp, backupVal);
    ResponseCmndDone();
  }
}

void VitoCommandLogOn() {
  VitoWiFi.enableLogger();
  ResponseCmndDone();
}

void VitoCommandLogOff() {
  VitoWiFi.disableLogger();
  ResponseCmndDone();
}

const char VitoCommandsString[] PROGMEM = "Vito|Read|WriteTimer|RestoreTimer|LogOn|LogOff";
void (* const VitoCommandsList[])(void) PROGMEM = { &VitoCommandRead, &VitoCommandWriteTimer, &VitoCommandRestoreTimer, &VitoCommandLogOn, &VitoCommandLogOff };

void globalCallbackHandler(const class IDatapoint& dp, class DPValue value) {
  char value_str[60] = {0};
  value.getString(value_str, sizeof(value_str));
  AddLog(LOG_LEVEL_INFO, "VTO: %s %s %s", dp.getGroup(), dp.getName(), value_str);
}

void VitoDrvInit() {
  ClaimSerial();

  VitoSystemzeit.setWriteable(true);
   VitoTimerDiHeizen.setWriteable(true);
   VitoTimerMoHeizen.setWriteable(true);
   VitoTimerMiHeizen.setWriteable(true);
   VitoTimerDoHeizen.setWriteable(true);
   VitoTimerFrHeizen.setWriteable(true);
   VitoTimerSaHeizen.setWriteable(true);
   VitoTimerSoHeizen.setWriteable(true);
   VitoTimerMoWW.setWriteable(true);
   VitoTimerDiWW.setWriteable(true);
   VitoTimerMiWW.setWriteable(true);
   VitoTimerDoWW.setWriteable(true);
   VitoTimerFrWW.setWriteable(true);
   VitoTimerSaWW.setWriteable(true);
   VitoTimerSoWW.setWriteable(true);

  VitoWiFi.setLogger(&logPrinter);
  // VitoWiFi.enableLogger();
  VitoWiFi.setup(&Serial);
  VitoWiFi.setGlobalCallback(globalCallbackHandler);
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
      VitoDrvInit();
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
