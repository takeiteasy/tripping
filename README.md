# xtrip

Generate & search for tripcodes. Works on *nix & Windows.
Note: Is only single-threaded, and untested on Windows.
<pre><code>
Arguments:
-h  -help:        Print help
-g  -generate:    Generate tripcode
-s  -search:      Search for tripcode
-ic -ignore-case: Ignore case when searching
-b  -benchmark:   Time & measure program
-v  -verboose:    Verboose mode
-f  -file:        Print to file

Examples:
./xtrip -g 10 -b:      Generates 10 random trips & time
./xtrip or ./xtrip -g: The same as ./xtrip -g 1
./xtrip -g test:       Generates trip from "test"
./xtrip -s test -ic:   Search for trips with "test" in them & ignore case

Notes:
Search mode will continue searching until ESC key is pressed.
Using -f with search mode will print to both terminal & file.
</pre></code>