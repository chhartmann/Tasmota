#ifdef USE_VITO

#include "VitoWiFi.h"
#include <string>

#define XSNS_98              98

// TODOs:
// - Add visualisation in web UI of values defined in a list
// - Add button and function for "Warmwasser"
// - Add button for "Set system time to current time"

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

DPCycleTime VitoTimerDiWW("Warmwasser-Di", "timer-water", 0x2108);
DPCycleTime VitoTimerMiWW("Warmwasser-Mi", "timer-water", 0x2110);
DPCycleTime VitoTimerMoWW("Warmwasser-Mo", "timer-water", 0x2100);
DPCycleTime VitoTimerDoWW("Warmwasser-Do", "timer-water", 0x2108);
DPCycleTime VitoTimerFrWW("Warmwasser-Fr", "timer-water", 0x2120);
DPCycleTime VitoTimerSaWW("Warmwasser-Sa", "timer-water", 0x2108);
DPCycleTime VitoTimerSoWW("Warmwasser-So", "timer-water", 0x2130);

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
  char value_str[30] = {0};
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
//      VitoWiFi.enableLogger();
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
