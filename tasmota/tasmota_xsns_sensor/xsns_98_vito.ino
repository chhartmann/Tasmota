#ifdef USE_VITO

#define XSNS_98              98

void VitoCommand() {
  AddLog(LOG_LEVEL_INFO, PSTR("VTO: Command received"));
  ResponseCmndChar("yo");
}

const char VitoCommandsString[] PROGMEM = "Vito|DoIt|Dump";
void (* const VitoCommandsList[])(void) PROGMEM = { &VitoCommand, &VitoCommand };


/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xsns98(uint8_t function) {
  bool result = false;

  if (FUNC_INIT == function) {
  }
  switch (function) {
    case FUNC_INIT:
      break;
    case FUNC_EVERY_SECOND:
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
