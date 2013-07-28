# tripping

Mine tripcodes using regex or just substring matching, generate X number of<br/>
tripcodes or just until you quit or test tripcodes.<br/>
Tested on OSX and Linux. Should work on Windows too, however that is untested.<br/>

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

<pre>
Copyright (c) 2013, Rusty Shackleford
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL RUSTY SHACKLEFORD BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</pre>

# Dependencies

Depends on PCRE ([BSD](https://en.wikipedia.org/wiki/BSD_licenses)) and PThreads (OSX/Linux)<br/>
C11 threads wrapper ([boost](https://en.wikipedia.org/wiki/Boost_Software_License)) made by [yahhoy](https://gist.github.com/yohhoy/2223710), modified by [me](https://gist.github.com/badassmofo/5897507)

