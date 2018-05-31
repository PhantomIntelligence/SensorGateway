/* DebugPrintf.cpp */
/*
	Copyright 2014, 2015 Phantom Intelligence Inc.

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#include <iostream>
#include <fstream>
#include <stdarg.h>

#ifndef Q_MOC_RUN

//#include <SensorCommunication/AWLSettings.h>
#include "../../include/SensorCommunication/DebugPrintf.h"

#endif

#include "AWLSettings.h"
#include "DebugPrintf.h"

namespace awl {

    const char* sDebugFileName = "debug.dbg";

// Field separator used in debug files.

    const char cFieldSeparator = ';';

#ifdef _DEBUG
    const bool bIsInDebug = true;
#else
    const bool bIsInDebug = false;
#endif

    void DebugFilePrintf(const char* format, ...) {

        if (AWLSettings::GetGlobalSettings()->bWriteDebugFile) {

            // Open the debug file
            ofstream debugFile;
            bool bOpen = OpenDebugFile(debugFile, sDebugFileName, true);
            if (!bOpen) return;


            // Format the string and output
            char str[255];

            boost::posix_time::ptime myTime(boost::posix_time::microsec_clock::local_time());

            std::string timeStr(boost::posix_time::to_simple_string(myTime));
            timeStr += " ";

            va_list argList;
            va_start (argList, format);
            vsprintf(str, format, argList);
            va_end(argList);

            timeStr += str;
            timeStr += "\n";

            debugFile << timeStr;
            // Close the debug file
            CloseDebugFile(debugFile);

        }
    }

    void DebugFilePrintf(ofstream& debugFile, const char* format, ...) {
        if (AWLSettings::GetGlobalSettings()->bWriteDebugFile) {
            if (!debugFile.is_open()) return;
            char str[255];

            boost::posix_time::ptime myTime(boost::posix_time::microsec_clock::local_time());

            std::string timeStr(boost::posix_time::to_simple_string(myTime));
            timeStr[11] = cFieldSeparator;
            timeStr += cFieldSeparator;

            va_list argList;
            va_start (argList, format);
            vsprintf(str, format, argList);
            va_end(argList);

            timeStr += str;
            timeStr += "\n";

            debugFile << timeStr;
        }
    }

    bool OpenDebugFile(ofstream& debugFile, const char* fileName, bool bAppend) {
        if (AWLSettings::GetGlobalSettings()->bWriteDebugFile) {
            if (debugFile.is_open()) {
                CloseDebugFile(debugFile);
            }

            if (bAppend) {
                debugFile.open(fileName, ofstream::out | ofstream::app);
            } else {
                debugFile.open(fileName);
            }

            if (debugFile.fail()) {
                std::string sErr = " Failed to open ";
                sErr += fileName;
                fprintf(stderr, sErr.c_str());

                return (false);
            }
        }
        return (true);
    }

    bool CloseDebugFile(ofstream& debugFile) {
        if (AWLSettings::GetGlobalSettings()->bWriteDebugFile) {
            if (debugFile.is_open())
                debugFile.close();
        }
        return (true);
    }


    bool OpenLogFile(ofstream& logFile, const char* fileName, bool bAppend) {
        if (AWLSettings::GetGlobalSettings()->bWriteLogFile) {
            if (logFile.is_open()) {
                CloseLogFile(logFile);
            }

            if (bAppend) {
                logFile.open(fileName, ofstream::out | ofstream::app);
            } else {
                logFile.open(fileName);
            }

            if (logFile.fail()) {
                std::string sErr = " Failed to open ";
                sErr += fileName;
                fprintf(stderr, sErr.c_str());

                return (false);
            }
        }

        return (true);
    }

    bool CloseLogFile(ofstream& logFile) {
        if (AWLSettings::GetGlobalSettings()->bWriteLogFile) {
            if (logFile.is_open())
                logFile.close();
        }

        return (true);
    }

    void LogFilePrintf(ofstream& logFile, const char* format, ...) {
        if (AWLSettings::GetGlobalSettings()->bWriteLogFile) {
            if (!logFile.is_open()) return;
            char str[255];

            boost::posix_time::ptime myTime(boost::posix_time::microsec_clock::local_time());

            std::string timeStr(boost::posix_time::to_simple_string(myTime));
            timeStr[11] = cFieldSeparator;
            timeStr += cFieldSeparator;

            va_list argList;
            va_start (argList, format);
            vsprintf(str, format, argList);
            va_end(argList);

            timeStr += str;
            timeStr += "\n";

            logFile << timeStr;
        }
    }

}  // namespace awl
