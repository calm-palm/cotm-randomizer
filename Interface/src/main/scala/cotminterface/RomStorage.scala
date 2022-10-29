package cotminterface

import org.scalajs.dom

import scala.scalajs.js
import scala.scalajs.js.typedarray.{ArrayBuffer, Uint8Array}

object RomStorage {
  val romKeyName = "cotmrom"

  def exists = {
    (0 until dom.window.localStorage.length).map(dom.window.localStorage.key(_)).contains(romKeyName)
  }

  /*
  Okay so this part is going to look really fucking stupid, and quite frankly, it is.
  The problem is that while we have 10 MB of local storage, and need to use 8 of it,
  that 10MB available is split into 2 byte chunks, so if you did 1 byte per array element,
  you'd run out of space after 5MB.
  So the obvious approach would be to chunk things up into 2 byte per array element.
  And that works great on chrome, but not necessarily elsewhere.
  Both Internet Explorer and Firefox will corrupt the data when storing some values.
  Internet Explorer is a bigger problem that Firefox, and no longer supported anyway,
  so we're going to disregard that.
  Under Firefox, it will seem like it works at first, but after closing the browser,
  or leaving it open for a while and then trying to read the data, it will then be corrupt.
  So it's the flushing to disk thats breaking things, but the problem is a little hidden
  as writing data and then immediatly reading back will always be fine, even if it corrupts long term.
  So if you actually figure out what combinations break Firefox, it turns out that every
  problem combination has the highest bit set out of a 2 byte chunk. So we can chunk
  up the rom data into 15 bit chunks and store that in a 16 bit array element. The overhead is
  small enough that the rom still fits in local storage that way.

  Shoutout to this person for catalogueing what combinations are ok, as this would have
  been a massive pain nowdays when you need to get a disk flush before reading back:
  https://stackoverflow.com/questions/11170716/are-there-any-characters-that-are-not-allowed-in-localstorage/11173673#11173673
   */
  def load = {
    val strData = dom.window.localStorage.getItem(romKeyName)
    Option(strData).map{ s =>
      val jsStr = s.asInstanceOf[js.Dynamic]
      val sizeHigh = jsStr.charCodeAt(0).asInstanceOf[Int]
      val sizeLow = jsStr.charCodeAt(1).asInstanceOf[Int]
      val size = ((sizeHigh & 0x7FFF) << 15) | (sizeLow & 0x7FFF)

      val buf = new ArrayBuffer(size)
      val byteBuf = new Uint8Array(buf)

      var remaining = 15
      var readPos = 2

      (0 until size by 2) foreach { i =>
        val part1 = jsStr.charCodeAt(readPos).asInstanceOf[Int]
        val part1Mask = (1 << remaining) - 1
        val part1Shifted = (part1 & part1Mask) << (16 - remaining)

        val part2 = jsStr.charCodeAt(readPos + 1).asInstanceOf[Int]
        val part2Mask = ((1 << (16 - remaining)) - 1) << (remaining - 1)
        val part2Shifted = (part2 & part2Mask) >> (remaining - 1)

        val combined = part1Shifted | part2Shifted
        byteBuf(i) = ((combined & 0xFF00) >> 8).toByte
        byteBuf(i + 1) = (combined & 0xFF).toByte

        remaining -= 1
        readPos += 1
        if(remaining == 0) {
          remaining = 15
          readPos += 1
        }
      }

      buf
    }
  }

  def save(buf: ArrayBuffer): Unit = {
    val byteBuf = new Uint8Array(buf)

    val sizeHigh = (byteBuf.length & 0x3FFF8000) >> 15
    val sizeLow = byteBuf.length & 0x7FFF

    var strData = js.Dynamic.global.String.fromCharCode(sizeHigh, sizeLow).asInstanceOf[String]

    var bitbuffer = 0
    var used = 0

    (0 until byteBuf.length by 2) foreach { i =>
      val chunk = ((byteBuf(i) & 0xFF) << 8) | (byteBuf(i + 1) & 0xFF)

      val part1Bits = 15 - used
      val part1Mask = ((1 << part1Bits) - 1) << (16 - part1Bits)
      val part1Shifted = (chunk & part1Mask) >> (16 - part1Bits)

      val part1Char = bitbuffer | part1Shifted
      strData += js.Dynamic.global.String.fromCharCode(part1Char).asInstanceOf[String]

      val part2Bits = 16 - part1Bits
      val part2Mask = (1 << part2Bits) - 1
      if(part2Bits == 15) {
        val part2Char = chunk & part2Mask
        strData += js.Dynamic.global.String.fromCharCode(part2Char).asInstanceOf[String]
        bitbuffer = 0
        used = 0
      }
      else {
        bitbuffer = (chunk & part2Mask) << (15 - part2Bits)
        used = part2Bits
      }
    }

    if(used > 0) {
      strData += js.Dynamic.global.String.fromCharCode(bitbuffer).asInstanceOf[String]
    }

    try {
      dom.window.localStorage.setItem(romKeyName, strData)
    }
    catch {
      case e: Exception =>
    }
  }

  sealed trait StorageState {
    def showLoad: Boolean
    def showSave: Boolean
  }
  case object StorageNotFound extends StorageState {
    override def showLoad: Boolean = false
    override def showSave: Boolean = true
  }
  case object StorageFound extends StorageState {
    override def showLoad: Boolean = true
    override def showSave: Boolean = false
  }
  case object StorageReadFile extends StorageState {
    override def showLoad: Boolean = false
    override def showSave: Boolean = true
  }
  case object StorageSaved extends StorageState {
    override def showLoad: Boolean = true
    override def showSave: Boolean = false
  }
}
