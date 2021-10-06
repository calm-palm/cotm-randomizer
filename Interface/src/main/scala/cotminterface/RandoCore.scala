package cotminterface

import scala.scalajs.js.annotation.JSGlobal
import scalajs.js
import scalajs.js.typedarray.{ArrayBuffer, ArrayBufferView, Int8Array, Uint8Array}

@js.native
@JSGlobal("Module")
object EmModule extends js.Object {
  def _malloc(size: Int): Int = js.native
  def _free(off: Int): Unit = js.native

  def HEAP8: Int8Array = js.native

  def cwrap(f: String, ret: String): js.Any = js.native
  def cwrap(f: String, ret: String, args: js.Array[String]): js.Any = js.native

  val FS: EmFS = js.native
}

@js.native
trait EmFS extends js.Object {
  def writeFile(path: String, data: ArrayBufferView): Unit = js.native
  def readFile(path: String): Uint8Array = js.native
  def unlink(path: String): Unit = js.native
}

object EmBindType {
  val string = "string"
  val number = "number"
}

sealed trait CoreLoadStatus extends Product with Serializable {
  def message: String
}
case object CoreLoadLoading extends CoreLoadStatus {
  override def message: String = "Loading core"
}
case class CoreLoadSuccess(name: String) extends CoreLoadStatus {
  override def message: String = "Core: " + name
}
case object CoreLoadTimeout extends CoreLoadStatus {
  override def message: String = "Core load timeout"
}

object RandoCore {
  def loaded = js.Dynamic.global.cotmr_core_init.asInstanceOf[Boolean]
  def get = new RandoCore
}

class RandoCore {
  private val verFunc = EmModule.cwrap("cotmr_core_version", EmBindType.string).asInstanceOf[js.Function0[String]]
  val version = verFunc()

  private val optFunc = EmModule.cwrap("cotmr_option_list", EmBindType.string).asInstanceOf[js.Function0[String]]
  val options = optFunc()

  //private val randFunc = EmModule.cwrap("cotmr_randomize", EmBindType.number, js.Array(EmBindType.number, EmBindType.number, EmBindType.number, EmBindType.string)).asInstanceOf[js.Function4[Int, Int, Int, String, Int]]
  private val randFunc = EmModule.cwrap("cotmr_randomize", EmBindType.number, js.Array(EmBindType.number, EmBindType.string)).asInstanceOf[js.Function2[Int, String, Int]]
  def randomize(rom: ArrayBuffer, seed: Int, opt: String) = {
    //val emBuf = EmModule._malloc(rom.length)
    //System.out.println("Allocated: " + emBuf)

    //import js.JSConverters._
    //EmModule.HEAP8.set(rom.toJSArray, emBuf)

    val ra = new Uint8Array(rom)

    EmModule.FS.writeFile("rom.gba", ra)

    val ret = randFunc(seed, opt)
    System.out.println("Ret: " + ret)

    val randRom = EmModule.FS.readFile("rom.gba")

    EmModule.FS.unlink("rom.gba")

    //val randRom = EmModule.HEAP8.slice(emBuf, emBuf + rom.length).toArray

    //EmModule._free(emBuf)

    val randSpoil = EmModule.FS.readFile("spoilerlog.txt")
    EmModule.FS.unlink("spoilerlog.txt")

    (randRom, randSpoil)
  }
}
