class Doughnut
    method cook()
        print "Fry until golden brown."
    endmethod
endclass

class BostonCream extends Doughnut
    method cook() 
        super.cook()
        print "Pipe full of custard and coat with chocolate."
    endmethod
endclass

BostonCream().cook()