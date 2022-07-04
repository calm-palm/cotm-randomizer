package cotminterface.gui

import cotminterface.{CoreLoadLoading, CoreLoadStatus, CoreLoadSuccess, CoreLoadTimeout, RandOption, RandOptionBool, RandOptionSlider, RandOptions, RandoCore}
import japgolly.scalajs.react._
import japgolly.scalajs.react.vdom.html_<^._

import scala.scalajs.js.typedarray.{ArrayBuffer, Uint8Array}
import org.scalajs.dom.{Blob, FileList, FileReader, URL, window}

import scala.scalajs.js
import scala.util.Random

object RandoScreen {

  case class State(coreStatus: CoreLoadStatus, rom: Option[ArrayBuffer], seed: Int, opt: Vector[(Int, RandOption)], obj: Option[String], spoilObj: Option[String])

  class Backend($: BackendScope[Unit, State]) {

    var core = Option.empty[RandoCore]
    var coreLoadTimer = Option.empty[Int]
    var coreLoadAttempts = 0

    def init = Callback {
      coreLoadTimer = Some(window.setInterval(loadTick _, 1000))
    }

    def loadTick: Unit = {
      if(RandoCore.loaded) {
        coreLoadTimer.foreach(window.clearInterval(_))
        coreLoadTimer = None

        val c = RandoCore.get
        core = Some(c)

        val options = RandOptions.parse(c.options)

        $.modState(_.copy(coreStatus = CoreLoadSuccess(c.version), opt = options)).runNow()
      }
      else {
        coreLoadAttempts += 1
        if(coreLoadAttempts > 10) { // target 10s, but don't hardcode this
          coreLoadTimer.foreach(window.clearInterval(_))
          coreLoadTimer = None

          $.modState(_.copy(coreStatus = CoreLoadTimeout)).runNow()
        }
      }
    }

    def render(state: State) = <.div(
      <.h2("Circle of the Moon Randomizer"),
      <.br,
      "Disclaimer: This page does not upload or download any data from the game. Any downloads are generated locally from a user-supplied source rom. Full documentation is available here: https://github.com/calm-palm/cotm-randomizer",
      <.br, <.br,
      state.coreStatus.message,
      <.br, <.br,
      <.div(
        <.h2("Drag rom here"),
        <.br,
        "Or select from file browser",
        <.br,
        <.input(
          ^.tpe := "file",
          ^.multiple := false,
          ^.onChange ==> onChooseFile
        ),
        ^.onDragOver ==> {_.preventDefaultCB},
        ^.onDrop ==> onDrop
      ).when(state.coreStatus.isInstanceOf[CoreLoadSuccess] && state.rom.isEmpty),
      <.div(
        "Loaded " + state.rom.map(_.byteLength).getOrElse(0) + " bytes",
        <.br,
        "Seed: ",
        <.input(^.onChange ==> onSeedChange, ^.value := Integer.toUnsignedString(state.seed)),
        <.button(^.tpe := "button", ^.onClick ==> doNewSeed, "Roll New"),
        <.br,
        TagMod(state.opt.map{case (i, o) =>
          OptionChoice(o, onOptCheck(i))
        }.flatten :_*),
        <.button(
          ^.tpe := "button",
          ^.onClick ==> doRandomize,
          "Randomize"
        )
      ).when(state.rom.isDefined),
      state.obj.whenDefined{o =>
        <.a(
          ^.href := o,
          ^.download := "cotmr_" + Integer.toUnsignedString(state.seed) + ".gba",
          "Download"
        )
      },
      <.br,
      state.spoilObj.whenDefined{o =>
        <.a(
          ^.href := o,
          ^.download := "cotm_" + Integer.toUnsignedString(state.seed) + "_spoilerlog.txt",
          "Spoiler log"
        )
      }
    )

    def onChooseFile(e: ReactEventFromInput) = {
      val files = e.target.files
      doLoad(files)

      e.preventDefaultCB
    }

    def onDrop(e: ReactDragEvent) = {
      val files = e.dataTransfer.files
      doLoad(files)

      e.preventDefaultCB
    }

    def doLoad(files: FileList): Unit = {
      if(files.length == 1) {
        val fr = new FileReader
        fr.onload = (_) => {
          val ab = fr.result.asInstanceOf[ArrayBuffer]
          $.modState(_.copy(rom = Some(ab))).runNow()
        }
        fr.readAsArrayBuffer(files(0))
      }
    }

    def revokeSeed = $.modState{s =>
      s.obj.foreach(URL.revokeObjectURL(_))
      s.spoilObj.foreach(URL.revokeObjectURL(_))
      s.copy(obj = None, spoilObj = None)
    }

    def doRandomize(e: ReactEventFromInput) = {
      e.preventDefaultCB >>
      revokeSeed >>
      $.modState{s =>
        val (randomized, spoilers) = (for{
          r <- s.rom
          c <- core
        } yield {
          val opts = s.opt.map(_._2.encode).mkString(",")
          val (arRom, arSpoil) = c.randomize(r, s.seed, opts)
          val blob = new Blob(js.Array(arRom))
          val blobSpoil = new Blob(js.Array(arSpoil))
          (URL.createObjectURL(blob), URL.createObjectURL(blobSpoil))
        }).unzip

        s.copy(obj = randomized, spoilObj = spoilers)
      }
    }

    def onSeedChange(e: ReactEventFromInput) = {
      val newSeed = try {
        Some(Integer.parseUnsignedInt(e.target.value))
      }
      catch {
        case e: Exception => None
      }

      revokeSeed >>
      $.modState{s =>
        s.copy(seed = newSeed.getOrElse(s.seed))
      }
    }

    def doNewSeed(e: ReactEventFromInput) = {
      val r = new Random(System.currentTimeMillis())
      val newSeed = r.nextInt()

      e.preventDefaultCB >>
      revokeSeed >>
      $.modState(_.copy(seed = newSeed))
    }

    def onOptCheck(i: Int)(e: ReactEventFromInput) = {
      val v = e.target.value // capture value because modState runs later

      revokeSeed >>
      $.modState{s =>
        val newOpt = s.opt.map{o =>
          if(o._1 == i) {
            o._2 match {
              case ob: RandOptionBool => {
                (o._1, ob.copy(value = !(ob.value)))
              }
              case os: RandOptionSlider => {
                val newVal = try {
                  v.toInt
                }
                catch {
                  case _: Exception => os.value
                }
                (o._1, os.copy(value = newVal))
              }
            }
          }
          else {o}
        }
        s.copy(opt = newOpt)
      }
    }
  }

  val Comp = ScalaComponent.builder[Unit]
    .initialState(State(CoreLoadLoading, None, 0, Vector.empty, None, None))
    .renderBackend[Backend]
    .componentDidMount(_.backend.init)
    .build
}
