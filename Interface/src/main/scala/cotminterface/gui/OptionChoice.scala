package cotminterface.gui

import cotminterface.{RandOption, RandOptionBool, RandOptionSlider}
import japgolly.scalajs.react._
import japgolly.scalajs.react.vdom.html_<^._

object OptionChoice {
  def apply(opt: RandOption, f: ReactEventFromInput => Callback) = opt match {
    case RandOptionBool(id, name, descr, value) => Vector[TagMod](
      descr,
      <.input.checkbox(
        ^.checked := value,
        ^.onChange ==> f
      ),
      <.br
    )
    case RandOptionSlider(id, name, descr, min, max, value) => Vector[TagMod](
      descr,
      <.input.range(
        ^.min := min,
        ^.max := max,
        ^.value := value,
        ^.onChange ==> f
      ),
      value.toString,
      <.br
    )
  }
}
