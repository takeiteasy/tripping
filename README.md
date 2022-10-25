# tripping

Tripcode brute-forcer with support for regex searches and multiple different modes.


## Usage

<pre>
usage: tripping [mode] [option] [options]

modes
  single -> If no arguments are entered, it'll spit out a random tripcode
  test   -> Interactive test mode, or test specified trips
            tripping test these tripcodes or tripping test
  gen    -> Generate a specified amount of tripcodes, or until quit
            tripping gen 100 or tripping gen
  mine   -> Try and bruteforce match a tripcode using regex or just substring search
            tripping mine /^test/ or tripping mine test
  bench  -> Benchmark mode, same as gen without printing out the trips
  help   -> Prints this message!

options
  --benchmark/-b -> Benchmark operation (different to bench mode)
  --ascii/-a     -> ASCII only mode, don't use SJIS at all
  --caseless/-i  -> Caseless matching (mine mode only)
  --threads/-th  -> Specify amount of threads to use
  --timeout/-to  -> Specify timeout for non-stop gen/bench and mine (secs)
  --min-rnd/-mi  -> Specify the minimum random string length
  --max-rnd/-mx  -> Specify the maximum random string length

examples:
  tripping mine /^hello/ -a -b -i
    Search for tripcodes that begin with "hello", only using ASCII
    but ignore the case and benchmark it.

  tripping mine /^(d+)$/ -th 8
    Search for tripcodes that is just a number using 8 threads.

  tripping gen -to 10 -mi 5 -mx 10
    Generate random tripcodes for 10 seconds using the range 5 to 10

  tripping test all of these tripcodes for me
    Turn "all", "of", "these", "tripcodes", "for" and "me" into tripcodes
</pre>

## License

```
The MIT License (MIT)

Copyright (c) 2013 George Watson

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
