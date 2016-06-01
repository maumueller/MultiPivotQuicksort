2.upto(32) do |i|
    print "template <typename ValueType>\n"
    print "void rotate#{i}("
    0.upto(i - 2) { |j| print "ValueType& a#{j}, "}
    print "ValueType& a#{i - 1})\n{\n"
    print "    ValueType tmp = a0;\n"
    1.upto(i - 1) { |j| print "    a#{j - 1} = a#{j};\n"}
    print "    a#{i - 1} = tmp; \n"
    print "}\n\n"
end
   


