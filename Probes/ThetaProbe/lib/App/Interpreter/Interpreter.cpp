/*
 * ComLineInterpreter.cpp
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

// #include <Application/Sensors/SensorIdTable.h>
// #include <Application/Sensors/Sensors.h>
// #include <Application/Radio/RadioSlave.h>

#include "CommandLine/CommandLine.h"
#include "Interpreter/Interpreter.h"
#include "Sensors/Measurement.h"
#include <CommandLine/ComLineConfig.h>
#include <CrcSocket.h>
#include <FileSystem/LittleFsHelpers.h>
#include <OsHelpers.h>
#include <Sensors/Sensors.h>
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
  // else if (cmd == 1925253141) {    result = setStationId(&lex);  		} // setStationId
  else if (cmd == 2948963465) {    result = getStationId(&lex);  		} // getStationId
  // else if (cmd == 4035361305) {    result = clrSensIdTable(&lex);  	} // clrSensIdTable
  else if (cmd == 1213712647) {    result = printMeasures(&lex); } // printMeasures
  else if (cmd == 1996702945) {    result = listDir(&lex); } // listDir
  else if (cmd == 3796804437) {    result = readFile(&lex); } // listDir  
  else if (cmd == 2221071754) {    result = saveSensIdTable(&lex); } // saveSensIdTable
  else if (cmd == 1050090700) {    result = calcHash(&lex);  				} // calcHash
  else if (cmd == 959926194) 	{    result = shutup();  							} // shutup
  else if (cmd == 1639364146) {    result = talk();  								} // talk
  else if (cmd == 1676458703) {		 Serial.println();	
	  															 OsHelpers::SYSTEM_REBOOT();  		} // reboot
  else if (cmd == 1973435441) {    OsHelpers::SYSTEM_EXIT();  			} // exit
	else if (cmd == 3288008560) {		 CommandLine::termDisplayClear();    
																	 CommandLine::termResetCursor();
																	 result = true;										} //	clear
  // clang-format on
  //@formatter:on

  // setSensId example
  // cmd       ID (hash)	min   max  	type  rel  shortname (8 bytes)
  // setSensId 3822322055 10.0  12.5  0   	1    "Test 007"
  // setSensId 3822322055 -12.5 -12.7 0   	0    "Test 007"

  // saveSensIdTable 2221071754 

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

bool Interpreter::printMeasures(Lexer *lex) {
  msmnt::MeasurementPivot *measurementPivot =
      msmnt::Sensors::instance().getMeasurementPivot();

  if (measurementPivot == nullptr) {
    Serial.println("measurementPivot is null.");
    return false;
  } else {
    Serial.println();
    measurementPivot->Dump();
  }
  return true;
}

bool Interpreter::listDir(Lexer *lex) {
  Serial.println();
  nvm::LittleFsHelpers::instance().listDir("/", 3);
  return true;
}

bool Interpreter::readFile(Lexer *lex) {
  ChrToken *chrToken = (ChrToken *)lex->getNextToken();
  if (chrToken->getType() != Token::String) {
    Serial.printf(
        "\nUsage: readFile \"<filename>\". (don't forget the slash!)\n");
    return false;
  }
  Serial.println();
  nvm::LittleFsHelpers::instance().readFile(chrToken->getVal());
  return true;
}

bool Interpreter::saveSensIdTable(Lexer *lex) {
  return msmnt::Sensors::instance().saveSensorIdTable();
}

// bool Interpreter::clrSensIdTable(Lexer *lex) {
//   // snsrs::Sensors::instance().getNonVolatileData()->clrIdTableData();
//   // tx_printf("\nID-Table cleared.\n");
//   Serial.println("\nDoing 'clrSensIdTable'");
//   return true;
// }

bool Interpreter::calcHash(Lexer *lex) {
  CmdToken *cmdToken = (CmdToken *)lex->getNextToken();
  if (cmdToken->getType() != Token::Command) {
    Serial.printf("\nUsage: calcHash <commandString>\n");
    return false;
  }
  Serial.printf("\nHash = %lu\n", cmdToken->getVal());

  return true;
}

// bool Interpreter::setStationId(Lexer *lex) {

//   UInt64Token *intToken = (UInt64Token *)lex->getNextToken();
//   if (intToken->getType() != Token::UInt64) {
//     return false;
//   }
//   // snsrs::Sensors::instance().getNonVolatileData()->writeStatId(
//   // 		intToken->getVal());
//   Serial.printf("\nsetStationId %lu\n", intToken->getVal());
//   return true;
// }

bool Interpreter::getSensIdTable(Lexer *lex) {
  nvm::LittleFsHelpers::instance().readFile(idTableFile);
  return true;
}

bool Interpreter::getStationId(Lexer *lex) {
  uint64_t sensorId = 0;
  uint8_t *mac = (uint8_t *)&sensorId; // the last two bytes should stay empty
  OsHelpers::GetMacAddress(mac);
  std::string stationId = msmnt::Measurement::DumpSensId(sensorId);

  Serial.printf("\nStation ID = %llu (%s)\n", sensorId, stationId.c_str());
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
  msmnt::Measurement::CopyToShortname(sensConf.shortname, chrToken->getVal());

  bool success =
      msmnt::Sensors::instance().getMeasurementPivot()->UpdateConfig(sensConf);

  if (success) {
    Serial.println("\nUpdateConfig done.");
  } else {
    std::string stationId = msmnt::Measurement::DumpSensId(sensConf.sensorId);
    Serial.printf("\nSensId ignored: %s\n", stationId.c_str());
  }

  return true;
}

} /* namespace cLine */
