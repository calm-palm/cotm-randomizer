package cotminterface

import cotminterface.gui.RandoScreen
import org.scalajs.dom

object CotMInterface {
  //var loadTimer = 0
  //var timerLock  = Option.empty[Int]

  val elementName = "cotmr_interface"

  def main(args: Array[String]): Unit = {
    //timerLock = Some(org.scalajs.dom.window.setInterval(checkLoaded _, 1000))

    // remove old div if it exists due to someone using a web browser's 'save page as' function
    val oldElement = dom.document.getElementById(elementName)
    if(oldElement != null) {
      oldElement.parentNode.removeChild(oldElement)
    }

    val element = dom.document.createElement("div")
    element.setAttribute("id", elementName)
    dom.document.body.insertBefore(element, dom.document.body.children(0))

    val paramString = dom.window.location.search
    val initOpt = if(paramString.length > 1) {
      Some(paramString.tail)
    }
    else {
      None
    }

    RandoScreen.Comp(RandoScreen.Props(initOpt)) renderIntoDOM element
  }

  /*def checkLoaded: Unit = {
    val loaded = scalajs.js.Dynamic.global.cotmr_core_init.asInstanceOf[Boolean]
    if(loaded) {
      timerLock.foreach(org.scalajs.dom.window.clearInterval(_))
      timerLock = None

      test
    }
    else {
      loadTimer += 1
      if(loadTimer > 10) {
        timerLock.foreach(org.scalajs.dom.window.clearInterval(_))
        timerLock = None
        System.out.println("failed load")
      }
    }
  }

  def test: Unit = {
    System.out.println("Core: " + RandoCore.version)
    System.out.println("opt: " + RandoCore.options)

    val romBytes = Array(1, 2, 3, 4).map(_.toByte)
    val rand = RandoCore.randomize(romBytes, 1, "")
  }*/
}
