/**
	Copyright 2014-2018 Phantom Intelligence Inc.
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

#ifndef SPIRITSENSORGATEWAY_FILEMANAGER_H
#define SPIRITSENSORGATEWAY_FILEMANAGER_H

#include <fstream>
#include <iostream>
#include <vector>

namespace TestUtilities {

    template<class T>
    class FileManager {

    public:
        FileManager() = default;

        virtual ~FileManager() = default;

        std::vector<T> readMessagesFromFile(std::string const& filename) {
            std::vector<T> messages;
            auto file = std::ifstream(filename);
            if (file) {
                std::string fileLine;
                std::string fileBlock;
                while (std::getline(file, fileLine)) {
                    if (fileLine.compare(MESSAGES_SEPARATOR) != 0) {
                        fileBlock += fileLine + "\n";
                    } else {
                        T message = readMessageFromFileBlock(fileBlock);
                        messages.push_back(message);
                        fileBlock = "";
                    }
                }
            } else {
                std::cerr << "An error occurred while opening the file.\n";
            }
            return messages;
        }

        void writeFileWithMessages(std::vector<T> messages, std::string filename) {
            auto file = std::fopen(filename.c_str(), "w+");
            for (auto message : messages) {
                writeFileBlockWithMessage(message, file);
            }
            fflush(file);
            fclose(file);
        }

        static bool areFilesEqual(std::string const& firstFilename, std::string const& secondFilename) {
            bool filesAreEqual = true;
            auto firstFile = std::ifstream(firstFilename);
            auto secondFile = std::ifstream(secondFilename);

            if (firstFile and secondFile) {
                std::string lineFromFirstFile;
                std::string lineFromSecondFile;
                while (std::getline(firstFile, lineFromFirstFile) and std::getline(secondFile, lineFromSecondFile)) {
                    if (lineFromFirstFile != lineFromSecondFile) {
                        filesAreEqual = false;
                        break;
                    }
                }
            } else {
                std::cerr << "An error occurred while opening a file.\n";
            }

            return filesAreEqual;
        }

    protected:
        std::string fetchSubstringBetweenDelimiters(std::string const& fileBlock, std::string const& firstDelimiter,
                                                    std::string const& secondDelimiter) {
            auto startPositionOfFirstDelimiter = fileBlock.find(firstDelimiter);
            auto endPositionOfFirstDelimiter = startPositionOfFirstDelimiter + firstDelimiter.length();
            auto restOfFileBlockAfterFirstDelimiter = fileBlock.substr(endPositionOfFirstDelimiter);
            auto startPositionOfSecondDelimiter = restOfFileBlockAfterFirstDelimiter.find(secondDelimiter);
            auto fetchedSubstring = fileBlock.substr(endPositionOfFirstDelimiter,
                                                     startPositionOfSecondDelimiter - endPositionOfFirstDelimiter);
            return fetchedSubstring;
        }

        virtual T readMessageFromFileBlock(std::string const& fileBlock) = 0;

        virtual void writeFileBlockWithMessage(T message, std::FILE* file) = 0;

        const std::string MESSAGES_SEPARATOR =
                "======================================================================================";

        const std::string MESSAGE_LABEL_VALUE_ASSOCIATOR = " : ";

        const std::string MESSAGE_CONTENT_TABULATOR = "  ";
    };
}

#endif //SPIRITSENSORGATEWAY_FILEMANAGER_H
