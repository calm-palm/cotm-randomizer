package cotminterface.gui

import cotminterface.{RandOption, RandOptionBool, RandOptionSlider}
import japgolly.scalajs.react._
import japgolly.scalajs.react.vdom.html_<^._

object OptionChoice {
  def apply(opt: RandOption, f: ReactEventFromInput => Callback, editable: Boolean = true) = opt match {
    case RandOptionBool(id, name, descr, value, _) => Vector[TagMod](
      descr,
      <.input.checkbox(
        ^.checked := value,
        ^.onChange ==> f,
        ^.disabled := !editable
      ),
      <.br
    )
    case RandOptionSlider(id, name, descr, min, max, value, _) => Vector[TagMod](
      descr,
      <.input.range(
        ^.min := min,
        ^.max := max,
        ^.value := value,
        ^.onChange ==> f,
        ^.disabled := !editable
      ),
      value.toString,
      <.br
    )
  }
}
