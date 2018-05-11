//
//  ConfigMacPresetDataSource.h
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

#ifndef ConfigMacPresetDataSource_h
#define ConfigMacPresetDataSource_h

#include <AppKit/AppKit.h>
#ifdef __cplusplus
#include "Config.hpp"
#endif

/** A data source that provides the preset rows */
@interface ConfigMacPresetDataSource : NSObject <NSTableViewDataSource, NSTableViewDelegate>

- (id)initWithConfig:(Config*)configTMP; /**< Initialise a data source with the given instance of the cofig */
- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView; /**< The number of rows in the list */
- (id)tableView:(NSTableView*)tableView objectValueForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)row; /**< The name of the preset in the given row */

- (BOOL)tableView:(NSTableView*)tableView writeRowsWithIndexes:(NSIndexSet*)rowIndexes toPasteboard:(NSPasteboard *)pboard; /**< Write the contents of the given rows to the clipboard */

@end

#endif /* ConfigMacPresetDataSource_h */