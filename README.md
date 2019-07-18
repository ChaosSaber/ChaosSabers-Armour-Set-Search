# Dauntless Armour Set Search
This Software is distributed under the MIT license(see LICENSE.txt) and uses third party libraries that are distributed under their own terms (see LICENSE-3RD-PARTY.txt)
You can find the current version on: https://github.com/ChaosSaber/Dauntless-Armour-Set-Search

## Installation
You need the visual c++ redistributable to run this programm. If you don't already have it installed, you can download it from here: https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads. Download the zip with the executable from the release page: https://github.com/ChaosSaber/ChaosSabers-Armour-Set-Search/releases. You can unzip it anywhere, no further Installation needed. The data the programm is saving is put into your documents location in the folder "ChaosSaber's Armour Set Search". Under Windows this could look like this:
"C:\Users\<your name>\Documents\ChaosSaber's Armour Set Search"


## Troubleshooting
If you encounter any error please refer at first to troubleshooting.txt for general troubleshooting. If you can't find your problem there, go to the [issue page on github](https://github.com/ChaosSaber/ChaosSabers-Armour-Set-Search/issues) and look if someone else has a similar Problem. If not you can create a bug report.

## Currently implemented
- Search function: Input the Skills and Weapontype you want and the app searchs for possible armour combinations
- Advanced search funtion: like normal search function but you can filter out armours/weapons which you don't yet have or don't want to use
- Manage your owned cells
- Advanced search filters (only Weapons with specific elements, use only with your cells)
- Save/load of searches
- Partly integrated unique skills (only in the search results)
- Filtering of search results (weapon, empty cell slots, additional perks)
- experimental export function to text (file and clipboard)

## planned features
- increase Search speed (currently in work)
- ui overhaul (help screen, rework of the perk selection, restructuring of search options and filters)
- Saving/loading/comparing of single sets
- Sorting of search results
- Advanced Filters: only armour sets with a minimum elemental resistance
- Completely Integrate unique skills (search)
- export function to image and to https://www.dauntless-builder.com

## Building the program
- This software use cmake to organize the building process
- It needs Qt to be compiled
- It needs openssl to be executed (optional only feature which use this is the check if a new version is available)
