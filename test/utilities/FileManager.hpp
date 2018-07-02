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
#include <vector>
#include <iostream>

namespace TestUtilities {

    template <class T>
    class FileManager {

    public:
        FileManager() = default;

        virtual ~FileManager() = default;

        std::vector<T> readMessagesFromFile(std::string const& filename) {
            std::vector<T> messages;
            auto file = std::ifstream(filename);
            if (file) {
                std::string line;
                while (std::getline(file, line)) {
                    T message = readMessageFromLine(line);
                    messages.push_back(message);
                }
            } else {
                std::cerr << "An error occurred while opening the file.\n";
            }
            return messages;
        }

        void writeFileWithMessages(std::vector<T> messages, char const* filename) {
            auto file = std::fopen(filename, "w+");
            for (auto message : messages){
                writeBlockWithMessage(message, file);
            }
            fflush(file);
            fclose(file);
        }
    protected:
        virtual T readMessageFromLine(std::string const& line) = 0;

        virtual void writeBlockWithMessage(T const& message, std::FILE* file) = 0;
    };
}

#endif //SPIRITSENSORGATEWAY_FILEMANAGER_H
