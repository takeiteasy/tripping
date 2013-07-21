#!/usr/bin/env ruby

src = []
File.open("../src/random.c", 'r').each_line { |x| src << x }

a, b, c = 0, 0, false
src.each_with_index do |x, i|
  x = x[0..-2]
  a = i - 1 if x == "/* SJIS characters not in ranges */" and b == 0
  (c ? (b = i + 1) : (c = true)) if x == "// }}}" and  a != 0
end

new = src[0..a].join() + %x(ruby gen_ranges.rb) + src[b..-1].join()
File.open("../src/random.c", 'w') { |f| f.write new }

