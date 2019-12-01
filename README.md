# microbar
A basic information bar for the X Window System.



## ...But what is microbar?
Well, if the comment alone does not provide enough information, microbar is a very simple information provider. It is very easy to customize as the default config is extremely verbose and the help menu has enough information and then some.

![microbar]()



## Features:



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
