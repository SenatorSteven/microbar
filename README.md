# microbar
A simple information bar for the X Window System.



## ...But what is microbar?
Well, if the comment alone does not provide enough information, microbar is a simple, fast and highly customizable information provider and command runner.

![microbar](https://github.com/TheSenatorSteven/microbar/blob/master/sample.png)



## Features:
- an extremely verbose and helpful default configuration
- fully customizable bar, sections and buttons
- ability to execute one-time commands e.g. "firefox"
- ability to execute text-returning commands e.g. "date"
- ability to quickly restart after a configuration edit



## Terminal help menu

#### :~$ microbar --help
<pre>
microbar: usage: microbar [parameters]
   [-h], [--help]     display this message
   [-c], [--config]   specify path to config, necessary
</pre>

#### :~$ microbar --config --help
<pre>
microbar: usage: microbar --config "/path/to/file/"
   # if the specified file doesn't exist, it will be created and it will contain the hardcoded default configuration
   # the $HOME variable can be used instead of "/path/to/home/", case sensitive
</pre>
