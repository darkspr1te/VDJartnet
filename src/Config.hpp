//
//  Config.hpp
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

/** @file Config.hpp */

#ifndef Config_hpp
#define Config_hpp

#include "CppStep/src/CSCore.hpp"

#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>
#include <set>

#include <iostream>

/*
#if defined(CS_Mac)
#elif defined(CS_Win)
#ifndef CLRFREE
#include "CppStep/src/WinString.hpp"
#endif
#endif
*/

/** A config parser and writer */
class Config {
public:
    /** A preset representing a common entry in the config file */
    struct Command {
        std::string name; /**< The name to be shown */
        std::string command; /**< The command to be used */
        
        Command(std::string name, std::string command) : name(name), command(command) {}
        Command() : Command("", "") {}
        explicit Command(std::string line) {
            std::size_t delimPos = line.find('~');
            if (delimPos == std::string::npos) {
                name = "";
                command = line;
            } else {
                name = line.substr(0, delimPos);
                command = line.substr(delimPos + 1, std::string::npos);
            }
        }
        
        std::string toShow() const {
            if (name == "") {
                return command;
            } else {
                return name;
            }
        }
        
        std::string toLine() const {
            return name + '~' + command;
        }
        
        inline bool operator == (const Command& that) const {
            return this->name == that.name && this->command == that.command;
        }
        inline bool operator != (const Command& that) const {
            return !(*this == that);
        }
    };
    
    std::array<Command, 512> channelCommands; /**< The list of commands to be sent to VirtualDJ */
    std::string host = "127.0.0.1"; /**< The host to which the Art-Net data should be sent */
    unsigned short port = 0x1936; /**< The port on which the Art-Net data should be sent */

    std::vector<Command> const& getPresets() { return presets; } /**< Get the list of presets */

    int getSkipPacketLimit() { return skipPacketLimit; } /**< Get the number of identical packets to be skipped before resending */
    std::chrono::milliseconds getCheckRate() { return checkRate; } /**< Get the time between requests to VirtualDJ */

    Config(std::string configPathTMP); /**< Constuct a config parser for the given config file */
    void loadConfig(); /**< Load the config file */
    void saveConfig(); /**< Save the config file */

private:
    std::string configPath; /**< The config file to parse */

    std::vector<Command> presets; /**< The list of presets */

    std::set<std::string> loadedConfigPaths; /**< The set of config files already parsed. This is used to prevent an infinite loop. */
    std::set<std::string> loadedPresetPaths; /**The set of preset files loaded, tracked so that they can be insterted into the saved config file**/
    int skipPacketLimit = 20; /**< The number of identical packets to be skipped before resending */
    std::chrono::milliseconds checkRate = std::chrono::milliseconds(10); /**< The time between requests to VirtualDJ */

    void loadDefaultPresets(); /**< Load the pre compiled presets */
    void parsePresetsStream(std::istream& fin); /**< Parse the given stream for presets */
    void parsePresetsLine(std::string line); /**< Parse the given string for a preset. A ~ should be used between the name and the command. */

    void parseConfigFile(std::ifstream& fin); /**< Parse the given file for presets */
    void parseConfigLine(std::string line); /**< Parse the given string for a config command. A ~ should be used between the channel number and the command. */
};

std::istream& safeGetLine(std::istream& is, std::string& t); /**< Get a line from an istream dealing with line endings safely */

#endif /* VDJartnet_hpp */
