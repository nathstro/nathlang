func timeFunction(function, upTo)
    let start = clock().
    function(upTo).
    let end = clock().
    return end - start.
endfunc

func squarer(upTo)
   for i = 1 : upTo do
       print i * i.
   endfor
endfunc

print "That took " + timeFunction(squarer, 500000) + " seconds too long.".
