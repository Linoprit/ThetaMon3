/*
 * ComLineInterpreter.cpp
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

// #include <Application/Sensors/SensorIdTable.h>
// #include <Application/Sensors/Sensors.h>
// #include <Application/Radio/RadioSlave.h>

#include <CommandLine.h>
#include <CrcSocket.h>
#include <Interpreter.h>
#include <Measurement.h>
#include <OsHelpers.h>
#include <cstring>

namespace cLine {
using namespace msmnt;

Interpreter::Interpreter() {}

bool Interpreter::doit(CmdBufferType comLine) {
  bool result = false;
  Lexer lex(&comLine);

  CmdToken *cmdToken = (CmdToken *)lex.getNextToken();
  if (cmdToken->getType() != Token::Command) {
    return false;
  }
  uint32_t cmd = cmdToken->getVal();

  //@formatter:off
  // clang-format off
  if 			(cmd == 2358927868)	{    result = setSensId(&lex);  			} // setSensId
  else if (cmd == 3913104177) {    result = getSensIdTable(&lex);  	} // getSensIdTable
  else if (cmd == 1925253141) {    result = setStationId(&lex);  		} // setStationId
  else if (cmd == 2948963465) {    result = getStationId(&lex);  		} // getStationId
  else if (cmd == 4035361305) {    result = clrSensIdTable(&lex);  	} // clrSensIdTable
  else if (cmd == 1050090700) {    result = calcHash(&lex);  				} // calcHash
  else if (cmd == 959926194) 	{    result = shutup();  							} // shutup
  else if (cmd == 1639364146) {    result = talk();  								} // talk
  else if (cmd == 1676458703) {		 Serial.println();	
	  															 OsHelpers::SYSTEM_REBOOT();  		} // reboot
  else if (cmd == 1973435441) {    OsHelpers::SYSTEM_EXIT();  			} // exit
	else if (cmd == 3288008560) {		 CommandLine::termDisplayClear();    
																	 CommandLine::termResetCursor();
																	 result = true;										} //	clear
 	// clang-format off
	//@formatter:on

	// setSensId example	
  // cmd       ID (hash)	min   max  	type  rel  shortname (8 bytes)
  // setSensId 3822322055 10.0  12.5  0   	1    "Test 007"
  // setSensId 3822322055 -12.5 -12.7 0   	0    "Test 007"




  return result;
}

bool Interpreter::shutup() {
  // bool isMaster =
  // 		snsrs::Sensors::instance().getNonVolatileData()->getStationType()
  // 				== snsrs::SensorIdTable::MASTER;
  // if (isMaster) {
  // 	radio::RadioMaster::instance().setShutup(true);
  // }
  Serial.println("\nDoing 'shutup'");
  return true;
}

bool Interpreter::talk() {
  // bool isMaster =
  // 		snsrs::Sensors::instance().getNonVolatileData()->getStationType()
  // 				== snsrs::SensorIdTable::MASTER;
  // if (isMaster) {
  // 	radio::RadioMaster::instance().setShutup(false);
  // }
  Serial.println("\nDoing 'talk'");
  return true;
}

bool Interpreter::clrSensIdTable(Lexer *lex) {
  // snsrs::Sensors::instance().getNonVolatileData()->clrIdTableData();
  // tx_printf("\nID-Table cleared.\n");
  Serial.println("\nDoing 'clrSensIdTable'");
  return true;
}

bool Interpreter::calcHash(Lexer *lex) {
  CmdToken *cmdToken = (CmdToken *)lex->getNextToken();
  if (cmdToken->getType() != Token::Command) {
    Serial.printf("\nUsage: calcHash <commandString>\n");
    return false;
  }
  Serial.printf("\nHash = %lu\n", cmdToken->getVal());

  return true;
}

bool Interpreter::setStationId(Lexer *lex) {

  UInt64Token *intToken = (UInt64Token *)lex->getNextToken();
  if (intToken->getType() != Token::UInt64) {
    return false;
  }
  // snsrs::Sensors::instance().getNonVolatileData()->writeStatId(
  // 		intToken->getVal());
  Serial.printf("\nsetStationId %lu\n", intToken->getVal());
  return true;
}

bool Interpreter::getSensIdTable(Lexer *lex) {
  Serial.printf("\nPrinting sensor-table from E2:\n");
  // snsrs::Sensors::instance().getNonVolatileData()->printIdTableRaw();
  return true;
}

bool Interpreter::getStationId(Lexer *lex) {
  uint32_t stationId = 524;
  // snsrs::Sensors::instance().getNonVolatileData()->getStationId();
  Serial.printf("\nStation ID = %u\n", stationId);
  return true;
}

bool Interpreter::setSensId(Lexer *lex) {
  SensorConfigType sensConf;

  UInt64Token *intToken = (UInt64Token *)lex->getNextToken();
  if (intToken->getType() != Token::UInt64) {
    return false;
  }
  sensConf.sensorId = intToken->getVal();

  FltToken *fltToken = (FltToken *)lex->getNextToken();
  if (fltToken->getType() != Token::Float) {
    return false;
  }
  sensConf.minVal = fltToken->getVal();

  fltToken = (FltToken *)lex->getNextToken();
  if (fltToken->getType() != Token::Float) {
    return false;
  }
  sensConf.maxVal = fltToken->getVal();

  intToken = (UInt64Token *)lex->getNextToken();
  if (intToken->getType() != Token::UInt64) {
    return false;
  }
  sensConf.sensType = static_cast<Measurement::SensorType>(intToken->getVal());

  intToken = (UInt64Token *)lex->getNextToken();
  if (intToken->getType() != Token::UInt64) {
    return false;
  }
  sensConf.relayNr = static_cast<Measurement::RelayChannel>(intToken->getVal());

  ChrToken *chrToken = (ChrToken *)lex->getNextToken();
  if (chrToken->getType() != Token::String) {
    return false;
  }
  memset(sensConf.shortname, ' ', Measurement::SHORTNAME_LEN);
  memcpy(sensConf.shortname, chrToken->getVal(), Measurement::SHORTNAME_LEN);

  // snsrs::Sensors::instance().getNonVolatileData()->writeIdTableData(sens);
  //  Measurement.UpdateConfig(sensConf)
  Serial.println("\nDoing sensorConfig update.");

  return true;
}

} /* namespace cLine */
