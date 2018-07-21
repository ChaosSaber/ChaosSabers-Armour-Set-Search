# Dauntless Armour Set Search

This Software is distributed under the MIT license(see LICENSE.txt) and uses third party libraries that are distributed under their own terms (see LICENSE-3RD-PARTY.txt)
You can find the current version on: https://github.com/ChaosSaber/Dauntless-Armour-Set-Search
If you encounter any error please refer at first to troubleshooting.txt for general troubleshooting

## updating from an older version
you have two possibilities
- you can either copy the new files over your old files
- you can copy the settings.json and myCells.json files from your old installation over to your new installation (if they don't exist they were not yet created)

## Currently implemented
- Search function: Input the Skills and Weapontype you want and the app searchs for possible armour combinations (it assumes you are standing in goodl relations to RNGesus)
- Advanced search funtion: like normal search function but you can input what cells you have and filter out armour which is currently not in rotation
- Currently only supports heroic Maelstrom
- Save/load of searches
- manage your available cells

## planned features 
- maybe saving/loading of single sets
- Sorting filtering of search results
- Integrating unique skills
- Add pre heroic Maelstrom equipment
- maybe some kind of export function to something like https://www.dauntless-builder.com

## Building the program
- This software use cmake to organize the building process
- It needs Qt to be compiled
- It needs openssl to be executed
