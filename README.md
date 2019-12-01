# microbar
A verbose xorg micro(management)bar.
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
