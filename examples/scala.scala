package awesome_package

import spire.implicits._

class Main extends App{

    def sum[F](a : F, b : F)(implicit field : Field[F]) = a + b

    println("hello from scala !") //scala world
    val a = 1
    val b = 2
    val c = sum (a, b)
    println("$a + $b = $c")
}
