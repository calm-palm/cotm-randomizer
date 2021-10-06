package cotminterface

sealed trait RandOption extends Product with Serializable {
  def encode: String
}
case class RandOptionBool(id: Int, name: String, descr: String, value: Boolean) extends RandOption {
  override def encode: String = id.toString + "=" + (if(value) {"1"} else {"0"})
}
case class RandOptionSlider(id: Int, name: String, descr: String, min: Int, max: Int, value: Int) extends RandOption {
  override def encode: String = id.toString + "=" + value.toString
}

object RandOptions {
  val optBoolRegex = "opt,(\\d+),bool,(\\w+),([\\w\\s]+),(\\d+)".r
  val optSliderRegex = "opt,(\\d+),slider,(\\w+),([\\w\\s]+),(\\d+),(\\d+),(\\d+)".r

  def parse(optString: String) = {
    var options = Map.empty[Int, RandOption]

    optString.split("\n").foreach{os =>
      os match {
        case optBoolRegex(id, name, descr, default) => {
          options += ((id.toInt, RandOptionBool(id.toInt, name, descr, default.toInt != 0)))
        }
        case optSliderRegex(id, name, descr, min, max, default) => {
          options += ((id.toInt, RandOptionSlider(id.toInt, name, descr, min.toInt, max.toInt, default.toInt)))
        }
        case _ => {System.out.println("Failed to parse option line: " + os)}
      }
    }

    options.toVector.sortWith(_._1 < _._1)
  }
}
