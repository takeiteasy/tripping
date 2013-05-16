## xtrip

Multi-threaded tripcode generator/searcher, for Linux only.
Released under the BSD license.

<pre>
usage: xtrip [--option=value] [-vb]

Arguments:
 --help        Print this message
 --test=$      Evaluate a string
 --generate=$  Generate random trips
 --generate=0  Dont' stop generating random trips (non-stop mode)
 --search=$    Try and find tripcodes that contain a string (ignores case)
 --Search=$    Try and find tripcodes that find that contain a string
 --threads=$   How many threads to use, default is 1
 --timeout=$   Timeout after X seconds instead of waiting for key press (search mode only)
 --min-len=$   Minimum length of random string (Default is 3)
 --max-len=$   Maximum length of random string (Default is 15)
 --benchmark   Time/Benchmark the program
 --verboose    Print verboose messages (also enables benchmarking)
 --generate    Same as --generate=0
  -h           Same as --help
  -g           Same as --generate
  -b           Same as --benchmark
  -v           Same as --verboose

Examples:
 xtrip --generate=100 -b          Generate 100 random tripcodes & benchmark it
 xtrip --Search="TEST" -v         Search for trips that contain "TEST" & verboose output
 xtrip --test=faggot              Will test "faggot" as a trip and will produce "Ep8pui8Vw2"
 xtrip --threads=8 --search=test  Spawn 8 threads all searching for trips with "test" in them (ignoring case)

NOTE:
 To exit non-stop generate mode & search modes (when not using --timeout) press the C-c (SIGINT) to end process
</pre>

Tested with clang, built with: 
<pre>
clang -Weverything -O3 main.c -o ~/bin/xtrip -pthread -lcrypto
</pre>

