# WadSelector

Build instructions:
```
git clone https://github.com/SpaceEnjoyer/WadSelector.git
cd WadSelector
cmake .
make
```
Running the program
```
./WadSelector
```
By placing the WadSelector binary inside /usr/local/bin it is possible to make the .desktop shortcut to GZDoom run WadSelector, simply add
```
Wadselector &&
```
after Exec=

When run this program will create a file in ~/.config named "wadsel_def.txt" that contains the path to your mods' folder and the path to the gzdoom ini file, if this file is deleted the program will use its default paths
