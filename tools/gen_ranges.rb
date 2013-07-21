#!/usr/bin/env ruby

a =* 33..126 # Start off with ASCII character range
File.open("sjis_dec.txt", "r").each_line { |x| a << x[0..x.length - 2].to_i }
# Measure, remove duplicates and sort the array
#o_len = a.length
a = a.uniq.sort
#n_len = a.length

# Find characters that don't fit into a range
r = []
for i in 0..a.length - 1
  r << a[i] if a[i] - a[i - 1] != 1 and a[i + 1] - a[i] != 1 if i != 0 and a[i + 1] != nil
end

puts  "/* SJIS characters not in ranges */"
puts  "// Left-over array {{{"
puts  "static unsigned short left_overs[] = {"
print "    "

# Put leftover characters into seperate array
l2 = 0
r.each_with_index do |x, j|
  last_val = (j == (r.length - 1))
  tmp_str  = (last_val ? x.to_s : "#{x},")

  if l2 != 9
    unless last_val
      print "#{tmp_str} "
      (6 - tmp_str.length).times { print " " } if tmp_str.length < 6
      l2 += 1
    else
      puts tmp_str
    end
  else
    puts tmp_str
    print "    " unless last_val
    l2 = 0
  end

  a.delete x
end
puts "};"
puts "#define TOTAL_LEFTOVERS #{r.length}"
puts "// }}}"

puts  "\n/* SJIS Character ranges */"
puts  "// Ranges arrays {{{"
puts  "static range_t ranges[] = {"
print "    "

# Loop through what is left of the original list and make ranges
x, y, t, l = 0, 0, 0, 0
a.each_with_index do |z, j|
  if x == 0
    x = y = z
    next
  end

  last_val = (j == (a.length - 1))
  if z - y != 1 and x != y or last_val
    xs = "#{x.to_s},"
    (6 - xs.length).times { xs << " " } if xs.length < 6
    ys = y.to_s
    (5 - ys.length).times { ys << " " } if ys.length < 5

    tmp_str =  "{ #{xs} #{ys} }"
    tmp_str << "," unless last_val

    if l != 3
      print "#{tmp_str} "
      l += 1
    else
      puts tmp_str
      print "    " unless last_val
      l = 0
    end

    x  = z
    t += 1
  end

  y = z
end
puts "};"
puts "#define TOTAL_RANGES #{t}"
puts "// }}}"

#puts "\nTotal ranges: #{c}, Duplicates: #{o_len - n_len}, Non-ranged: #{r.length}"

