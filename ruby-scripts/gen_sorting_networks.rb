require 'open-uri'

2.upto(32) do |i|
    print <<-EOF
template <typename ValueType>
static inline
EOF
    print "void sort#{i}("
    0.upto(i - 2) do |j|
        print "ValueType& A" + j.to_s + ", "
    end
    puts "ValueType& A#{i - 1})"
    puts "{"
    open("http://jgamble.ripco.net/cgi-bin/nw.cgi?inputs=#{i}&algorithm=best&output=macro") do |f|
        f.each_line do |line|
            if line =~ /(SWAP.*;)/
                puts "    " + $1.gsub(/(\d+)/,"A\\1")
            end
        end
    end
    puts "}\n"
    puts ""
end


