<!-- README.md

MIT License

Copyright (C) 2019 Stefanos "Steven" Tsakiris

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. -->

# microbar
Text-based status bar for the X Window System.



## ...But what _is_ microbar?
Well, if the comment alone does not provide enough information, microbar is a simple, fast and highly customizable information provider and command runner.

![microbar](https://github.com/TheSenatorSteven/microbar/blob/master/sample.png)



## Features
- Extremely verbose and helpful default configuration
- Fully customizable bar, "sections", "containers" and "rectangles"
- Ability to execute one-time commands e.g. "firefox"
- Ability to execute text-returning commands e.g. "date"
- Ability to restart after a configuration edit
- Multi-monitor support



## Philosophy
CPU usage is everything. When some information is needed only once every little while, said information is effectively useless. microbar completely lacks the use of a sleep function so nothing ever gets updated, ever. When the user demands the full power of the CPU, the user shall have it. When the user needs to know something as trivial as time, the user shall know it. It's up to the user, and the user only.
<br>
<br>
Somebody could argue that the CPU power being used in updating the bar is as trivial as the information obtained from it. As this is the philosophical part of the equation, this applies not. The user controls the programs, not the opposite.



## Trivia
"microbar" comes from the words "micromanagement" and, of course, "bar". It is a bar that needs quite a lot of management on a micro, small, level to get it to look and work the exact way the user wants. It requires simple mathematical operations knowledge, shell commands knowledge and a screenshot button to get it to work well and look perfect.



## Installation
1. Download
2. Unzip
3. Go to the microbar-master directory
4. Allow "compile.sh" to execute as program
5. Execute "compile.sh", preferably via terminal
6. Done



## Configuration

Before compilation, while possible to edit all files, it is important to pay a visit to headers/defines.h. Through it, you can set the tab size, the amount of characters to read per line and the default number of lines to read.
<br>
<br>
The program name is read at runtime. The first argument passed to the program, that being the name used to execute it, will be the name of the program.



## Terminal help menu

#### $ microbar --help
<pre>
microbar: usage: microbar [parameters] or microbar [parameter] [--help]
    [-h], [--help]         display this message
    [-c], [--config]       specify path to config, necessary
    [-w], [--workplace]    specify path to directory used for temporary files, optional
</pre>

#### $ microbar --config --help
<pre>
microbar: usage: microbar --config "/path/to/file"
    # if the specified file doesn't exist, it will be created and it will contain the hardcoded default configuration
    # environment variables may be used
</pre>

#### $ microbar --workplace --help
<pre>
microbar: usage: microbar --workplace "/path/to/directory"
    # if the specified directory doesn't exist, it will not be created
    # if not specified, workspace directory will be the directory of config
    # environment variables may be used
</pre>
