package cotminterface

sealed trait RandOption extends Product with Serializable {
  def encode: String
  def encodeUrl: String
  def updatedFromUrl(urlOpts: Map[String, String]): RandOption
  def updatedFromPreset(pre: Preset): RandOption
  def resetDefault: RandOption
}
case class RandOptionBool(id: Int, name: String, descr: String, value: Boolean, default: Boolean) extends RandOption {
  override def encode: String = id.toString + "=" + (if(value) {"1"} else {"0"})
  override def encodeUrl = name + "=" + (if(value) {"1"} else {"0"})
  override def updatedFromUrl(urlOpts: Map[String, String]) = {
    urlOpts.get(name).map{ v =>
      this.copy(value = (v == "1"))
    }.getOrElse(this)
  }
  override def updatedFromPreset(pre: Preset): RandOption = {
    pre.opt.get(name).map{ v =>
      this.copy(value = (v == 1))
    }.getOrElse(this.copy(value = default))
  }
  override def resetDefault: RandOption = {
    this.copy(value = default)
  }
}
case class RandOptionSlider(id: Int, name: String, descr: String, min: Int, max: Int, value: Int, default: Int) extends RandOption {
  override def encode: String = id.toString + "=" + value.toString
  override def encodeUrl = name + "=" + value.toString
  override def updatedFromUrl(urlOpts: Map[String, String]) = {
    urlOpts.get(name).map{ v =>
      var newV = try {
        Integer.parseInt(v)
      }
      catch {
        case _: NumberFormatException => 0
      }
      if(newV < min) { newV = min}
      if(newV > max) { newV = max }

      this.copy(value = newV)
    }.getOrElse(this)
  }
  override def updatedFromPreset(pre: Preset): RandOption = {
    pre.opt.get(name).map{ v =>
      this.copy(value = v)
    }.getOrElse(this.copy(value = default))
  }
  override def resetDefault: RandOption = {
    this.copy(value = default)
  }
}

object RandOptions {
  val optBoolRegex = "opt,(\\d+),bool,(\\w+),([\\w\\s]+),(\\d+)".r
  val optSliderRegex = "opt,(\\d+),slider,(\\w+),([\\w\\s]+),(\\d+),(\\d+),(\\d+)".r

  def parse(optString: String) = {
    var options = Map.empty[Int, RandOption]

    optString.split("\n").foreach{os =>
      os match {
        case optBoolRegex(id, name, descr, default) => {
          options += ((id.toInt, RandOptionBool(id.toInt, name, descr, default.toInt != 0, default.toInt != 0)))
        }
        case optSliderRegex(id, name, descr, min, max, default) => {
          options += ((id.toInt, RandOptionSlider(id.toInt, name, descr, min.toInt, max.toInt, default.toInt, default.toInt)))
        }
        case _ => {System.out.println("Failed to parse option line: " + os)}
      }
    }

    options.toVector.sortWith(_._1 < _._1)
  }

  def updateFromUrlString(opt: Vector[(Int, RandOption)], urlString: String) = {
    val parsedOpts = urlString.split('&').map{ o =>
      val eq = o.indexOf('=')
      if(eq != -1) {
        val k = o.substring(0, eq)
        val v = o.substring(eq + 1)
        Some((k, v))
      }
      else { None }
    }.flatten.toMap

    opt.map(o => (o._1, o._2.updatedFromUrl(parsedOpts)))
  }

  def updateFromPreset(opt: Vector[(Int, RandOption)], pre: Preset) = {
    opt.map(o => (o._1, o._2.updatedFromPreset(pre)))
  }

  def resetToDefault(opt: Vector[(Int, RandOption)]) = {
    opt.map(o => (o._1, o._2.resetDefault))
  }
}
