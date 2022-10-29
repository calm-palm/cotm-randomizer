package cotminterface

import scala.scalajs.js
import scala.scalajs.js.annotation.JSGlobal

object PresetList {
  @js.native
  @JSGlobal("cotmr_presets")
  val presets: js.Array[Preset] = js.native
}

@js.native
trait Preset extends js.Object {
  val name: String = js.native
  val opt: js.Dictionary[Int] = js.native
}
