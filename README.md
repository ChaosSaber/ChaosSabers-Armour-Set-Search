# Dauntless Armour Set Search

This Software is distributed under the MIT license(see LICENSE.txt) and uses third party libraries that are distributed under their own terms (see LICENSE-3RD-PARTY.txt)
You can find the current version on: https://github.com/ChaosSaber/Dauntless-Armour-Set-Search
If you encounter any error please refer at first to troubleshooting.txt for general troubleshooting
You need the visual c++ runtime environment to run this programm. If you don't already have it installed, you can download it from here: https://www.microsoft.com/en-us/download/details.aspx?id=48145

## updating from an older version
Since Version 0.10 the data is in "my Documents/ChaosSaber's Armour Set Search" so you can just unzip the new version anywhere you want and can delete the old on without data loss. If you update from an older version you have two possibilities:
- you can either copy the new files over your old files
- you can copy the settings.json, lastSearch.ass and myCells.json/myCells.cells(depending on version) files from your old installation over to your new installation (if they don't exist they were not yet created)

## Currently implemented
- Search function: Input the Skills and Weapontype you want and the app searchs for possible armour combinations
- Advanced search funtion: like normal search function but you can filter out armour which you don't yet have
- Save/load of searches
- Manage your available cells
- Partly integrated unique skills (only in the search results)
- Filtering of search results

## planned features 
- Saving/loading/comparing of single sets
- Sorting of search results
- Completely Integrate unique skills (search)
- Maybe some kind of export function to something like https://www.dauntless-builder.com

## Building the program
- This software use cmake to organize the building process
- It needs Qt to be compiled
- It needs openssl to be executed (optional only feature which use this is the check if a new version is available)
