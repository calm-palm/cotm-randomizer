package cotminterface

import scala.scalajs.js.typedarray.{ArrayBuffer, Uint8Array}

object SimpleChecksum {

  val CORM_NTSC_U_CHECKSUM = -2025575379

  // algorithm based on https://stackoverflow.com/questions/811195/fast-open-source-checksum-for-small-strings
  def calculate(buf: ArrayBuffer) = {
    val byteBuf = new Uint8Array(buf)

    (0 until byteBuf.length).foldLeft(0x12345678)((chk, i) => chk + (byteBuf(i).toInt * (i + 1)))
  }
}
