# microbar
A simple text-based status bar for the X Window System.



## ...But what is microbar?
Well, if the comment alone does not provide enough information, microbar is a simple, fast and highly customizable information provider and command runner.

![microbar](https://github.com/TheSenatorSteven/microbar/blob/master/sample.png)



## Features:
- the lack of a sleep function
- multi-monitor support
- an extremely verbose and helpful default configuration
- a fully customizable bar, sections and buttons
- the ability to execute one-time commands e.g. "firefox"
- the ability to execute text-returning commands e.g. "date"
- the ability to quickly restart after a configuration edit



## Philosophy
CPU usage is everything. When some information is needed only once every little while, said information is effectively useless. Microbar completely lacks the use of a sleep function so nothing ever gets updated. Ever. When the user demands the full power of the CPU, the user shall have it. When the user needs to know something as trivial as time, the user shall know it. It's up to the user, and the user only.
<br>
<br>
Somebody could argue that the CPU power being used in updating the bar is as trivial as the information obtained from it. As this is the philosophical part of the equation, this applies not. The user controls the programs, not the opposite.



## Trivia

"Microbar" comes from the words "micromanagement" and, of course, "bar". It is a bar that needs quite a lot of management on a micro, small, level to get it to look and work the exact way the user wants. It requires simple mathematical operations knowledge, shell commands knowledge and at least a handful of screenshots to get it to look and work perfectly.



## Terminal help menu

#### :~$ microbar --help
<pre>
microbar: usage: microbar [parameters] or microbar [parameter] [--help]
    [-h], [--help]      display this message
    [-c], [--config]    specify path to config, necessary
</pre>

#### :~$ microbar --config --help
<pre>
microbar: usage: microbar --config "/path/to/file/"
    # if the specified file doesn't exist, it will be created and it will contain the hardcoded default configuration
    # environment variables may be used
</pre>
