package cotminterface.gui

import cotminterface.{CoreLoadLoading, CoreLoadStatus, CoreLoadSuccess, CoreLoadTimeout, PresetList, RandOption, RandOptionBool, RandOptionSlider, RandOptions, RandoCore, RomStorage, SimpleChecksum}
import japgolly.scalajs.react._
import japgolly.scalajs.react.vdom.html_<^._
import org.scalajs.dom

import scala.scalajs.js.typedarray.{ArrayBuffer, Uint8Array}
import org.scalajs.dom.{Blob, FileList, FileReader, URL, window}

import scala.scalajs.js
import scala.util.Random

object RandoScreen {

  case class Props(initOpt: Option[String])
  case class State(
    coreStatus: CoreLoadStatus,
    storageState: RomStorage.StorageState,
    rom: Option[ArrayBuffer],
    checksum: Int,
    seed: Int,
    opt: Vector[(Int, RandOption)],
    obj: Option[String],
    spoilObj: Option[String]
  )

  class Backend($: BackendScope[Props, State]) {

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

       // $.modState(_.copy(coreStatus = CoreLoadSuccess(c.version), opt = options)).runNow()
        $.modState{(s, p) =>
          val initOptions = p.initOpt.map(io => RandOptions.updateFromUrlString(options, io)).getOrElse(options)
          val initSeed = p.initOpt.map(io => parseSeedFromLink(io)).flatten.getOrElse(s.seed)
          s.copy(coreStatus = CoreLoadSuccess(c.version), opt = initOptions, seed = initSeed)
        }.runNow()
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

    def render(props: Props, state: State) = <.div(
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
        TagMod(
          <.br,
          <.button(
            ^.tpe := "button",
            ^.onClick ==> doLoadFromLocal,
            "Load From Local Storage"
          )
        ).when(state.storageState.showLoad),
        ^.onDragOver ==> {_.preventDefaultCB},
        ^.onDrop ==> onDrop
      ).when(state.coreStatus.isInstanceOf[CoreLoadSuccess] && state.rom.isEmpty),
      <.div(
        if(state.checksum == SimpleChecksum.CORM_NTSC_U_CHECKSUM) {
          "Rom loaded. Checksum valid"
        } else {
          TagMod(
            "Unknown checksum: " + state.checksum,
            <.br,
            "The source rom might be corrupt"
          )
        },
        TagMod(
          <.br,
          <.button(
            ^.tpe := "button",
            ^.onClick ==> doSaveToLocal,
            "Save to Local Storage"
          ),
          <.br
        ).when(state.storageState.showSave),
        <.br,
        "Seed: ",
        <.input(^.onChange ==> onSeedChange, ^.value := Integer.toUnsignedString(state.seed), ^.disabled := props.initOpt.isDefined),
        <.button(^.tpe := "button", ^.onClick ==> doNewSeed, "Roll New").when(props.initOpt.isEmpty),
        <.br,
        TagMod(
          <.br,
          "Preset: ",
          <.select(
            ^.value := "ChoosePreset",
            <.option(^.value := "ChoosePreset", "Choose Preset"),
            PresetList.presets.map(p => <.option(^.value := p.name, p.name)).toTagMod,
            ^.onChange ==> onPresetChange
          ),
          <.br,
          <.br
        ).when(props.initOpt.isEmpty),
        TagMod(state.opt.map{case (i, o) =>
          OptionChoice(o, onOptCheck(i), props.initOpt.isEmpty)
        }.flatten :_*),
        <.br,
        "Share Link: ",
        {
          val sl = buildShareLink(state.seed, state.opt.map(_._2))
          <.a(^.href := sl, sl)
        },
        <.br,
        <.br,
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
      }.when(props.initOpt.isEmpty)
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

    def doLoadFromLocal(e: ReactEventFromInput) = {
      e.preventDefaultCB >>
      $.modState{ s =>
        val r = RomStorage.load
        val c = r.map(SimpleChecksum.calculate(_)).getOrElse(0)
        s.copy(rom = r, checksum = c)
      }
    }

    def doLoad(files: FileList): Unit = {
      if(files.length == 1) {
        val fr = new FileReader
        fr.onload = (_) => {
          val ab = fr.result.asInstanceOf[ArrayBuffer]
          val chk = SimpleChecksum.calculate(ab)
          System.out.println("Checksum: " + chk.toString)
          $.modState(_.copy(rom = Some(ab), checksum = chk, storageState = RomStorage.StorageReadFile)).runNow()
        }
        fr.readAsArrayBuffer(files(0))
      }
    }

    def doSaveToLocal(e: ReactEventFromInput) = {
      e.preventDefaultCB >>
      $.modState{ s =>
        s.rom.foreach(RomStorage.save(_))
        s.copy(storageState = RomStorage.StorageSaved)
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

    def buildShareLink(seed: Int, opt: Vector[RandOption]) = {
      val loc = dom.window.location.href
      val base = loc.takeWhile(_ != '?')

      val seedPart = "seed=" + Integer.toUnsignedString(seed)

      val optParts = opt.map(_.encodeUrl)

      base + "?" + ((Vector(seedPart) ++ optParts).mkString("&"))
    }

    def parseSeedFromLink(optString: String) = {
      optString.split("&").find(_.indexOf("seed=") == 0).map{ o =>
        try {
          Some(Integer.parseUnsignedInt(o.substring(5)))
        }
        catch {
          case _: NumberFormatException => None
        }
      }.flatten
    }

    def onPresetChange(e: ReactEventFromInput) = {
      val preset = PresetList.presets.find(_.name == e.target.value)

      revokeSeed >>
      $.modState{s =>
        val newOpt = preset.map(p => RandOptions.updateFromPreset(s.opt, p)).getOrElse(RandOptions.resetToDefault(s.opt))
        s.copy(opt = newOpt)
      }
    }
  }

  val Comp = ScalaComponent.builder[Props]
    .initialState(State(
      CoreLoadLoading,
      if(RomStorage.exists) { RomStorage.StorageFound } else { RomStorage.StorageNotFound },
      None,
      0,
      0,
      Vector.empty,
      None,
      None
    ))
    .renderBackend[Backend]
    .componentDidMount(_.backend.init)
    .build
}
