# This tool is a VERY EARLY WIP. Expect some crashes and instability!

This tool lets you define a UI for a shell script. It lets you create a set of inputs from the UI, and then generates a shell script which it then runs.

I originally made this utility for use with ffmpeg, because ffmpeg's usage is... obtuse to say the least.

This repo requires [nlohmann/json](https://github.com/nlohmann/json). You will need to download that and copy over the contents of include/nlohmann, so that json.hpp resides in Core/include/nlohmann.

