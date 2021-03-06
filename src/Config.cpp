//
//  Config.cpp
//  VDJartnet
//
//  Copyright © 2017-18 Jonathan Tanner. All rights reserved.
//
//This file is part of VDJartnet.
//
//VDJartnet is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//VDJartnet is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with VDJartnet.  If not, see <http://www.gnu.org/licenses/>.
//
//Additional permission under GNU GPL version 3 section 7
//
//If you modify this Program, or any covered work, by linking or
//combining it with VirtualDJ, the licensors of this Program grant you
//additional permission to convey the resulting work.
//
//If you modify this Program, or any covered work, by linking or
//combining it with the Visual C++ Runtime, the licensors of this Program grant you
//additional permission to convey the resulting work.
//Corresponding Source for a non-source form of such a combination shall not
//include the source code for the parts of the Visual C++ Runtime used as well as that of the covered work.

#include "Config.hpp"
#include "defaultPresets.h"

Config::Config(std::string configPathTMP) {
    configPath = configPathTMP;
    loadConfig();
}


void Config::loadConfig() {
    std::ifstream fin(configPath);
    if (fin.is_open()) {
        for (std::size_t i = 0; i < 512; i++) {
            channelCommands[i] = {"", ""};
        }

        //this file has been loaded mark it as loaded to prevent looping
        loadedConfigPaths.insert(configPath);
    }
    presets.clear();
    parseConfigFile(fin);
    loadDefaultPresets();
}

void Config::loadDefaultPresets() {
/*
#if defined(CS_Mac)
    std::string presetPresets =
#include "presets.txt"
    ;
#elif defined(CS_Win)
    std::string presetPresets = WinString::toStdString(System::Resources::GetString("IDR_PRESETS1"));
#endif
*/
    std::istringstream defaultPresetsStream(defaultPresets());
    parsePresetsStream(defaultPresetsStream);
}

void Config::parsePresetsStream(std::istream& stin) {
    std::string line;
    safeGetLine(stin, line);
    stin.eof();
    while (!line.empty()) {
        parsePresetsLine(line);
        safeGetLine(stin, line);
    }
}

void Config::parsePresetsLine(std::string line) {
    presets.push_back(Command(line));
}

void Config::saveConfig() {
    std::ofstream fout(configPath);
    if (fout.is_open()) {
        fout << "+H " << host << ":" << port << std::endl;
        fout << "+T " << skipPacketLimit << std::endl;
        fout << "+C " << std::to_string(checkRate.count()) << std::endl;
        //include all of the preset paths
        for(std::set<std::string>::iterator preset = loadedPresetPaths.begin(); preset != loadedPresetPaths.end(); preset++){
          fout << "@p "<<*preset<<std::endl;
        }

        for (std::size_t i = 0; i < 512; i++) {
            if (channelCommands[i].command != "") {
                fout << std::string(3 - ((unsigned long)floor(std::log10(i + 1)) + 1),'0') << std::to_string(i + 1) << '~' << channelCommands[i].name << '~' << channelCommands[i].command << std::endl;
            }
        }
        fout.close();
    }
}

void Config::parseConfigFile(std::ifstream& fin){
    if (fin.is_open()) {
        std::string line;
        safeGetLine(fin, line);
        while (!line.empty()) {
            parseConfigLine(line);
            safeGetLine(fin, line);
        }
        fin.close();
    }
}

void Config::parseConfigLine(std::string line){
    if (line.at(0) == '@'){
        //include statement
        //load a second file and begin a parse on that
        size_t pathStart = line.find_first_not_of(' ', 2);
        std::string path = line.substr(pathStart, std::string::npos);

        if(loadedConfigPaths.find(path) != loadedConfigPaths.end()){
          //the file has already been loaded
          //skip this line as we have already consumed this file
          return;
        }
        //insert into the set of loaded config files to stop a ensure the same file is not looped over
        loadedConfigPaths.insert(path);

        std::ifstream fin(path);
        if (line.at(1) == 'c') {
            parseConfigFile(fin);
        } else if (line.at(1) == 'p') {
            if (fin.is_open()) {
                parsePresetsStream(fin);
                fin.close();
                loadedPresetPaths.insert(path);
            }
        }
        //finished with new file
        return;
    }

    if (line.at(0) == '+') {
        size_t valueStart = line.substr(2, std::string::npos).find_first_not_of(' ') + 2;
        std::string value = line.substr(valueStart, std::string::npos);

        switch (line.at(1)) {
        case 'T':
            if (value.find_first_not_of("0123456789") == std::string::npos) {
                int splValue = stoi(value);
                if (splValue >= 0) {
                    skipPacketLimit = stoi(value);
                }
            }
            break;

        case 'C':
            if (value.find_first_not_of("0123456789") == std::string::npos) {
                int crValue = stoi(value);
                if(crValue > 0){
                  checkRate = std::chrono::milliseconds(crValue);
                }
            }
            break;

        case 'H':
            size_t delimPos = value.find(":");
            host = value.substr(0, delimPos);
            if (delimPos != std::string::npos) {
                std::string portS = value.substr(delimPos + 1, std::string::npos);
                if (portS.find_first_not_of("0123456789") == std::string::npos) {
                    port = (unsigned short)std::stoi(portS);
                }
            }
            break;
        }
        return;
    }

    //line does not match any special command line so assume it is a channel definition
    std::size_t delimPos = line.find('~');
    std::string channelNoS = line.substr(0, delimPos);
    if ((channelNoS.find_first_not_of("0123456789") == std::string::npos)) {
        std::size_t channelNo = stoul(channelNoS) - 1;
        if (channelNo >= 0 && channelNo < 512) {
            channelCommands[channelNo] = Command(line.substr(delimPos + 1, std::string::npos));
        }
    }
}

std::istream& safeGetLine(std::istream& is, std::string& t) {
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
            case '\n':
                return is;
            case '\r':
                if(sb->sgetc() == '\n')
                    sb->sbumpc();
                return is;
            case EOF:
                // Also handle the case when the last line has no line ending
                if(t.empty())
                    is.setstate(std::ios::eofbit);
                return is;
            default:
                t += (char)c;
        }
    }
}
