#!/usr/bin/env ruby

# Generates ADB to USB keyboard codes mapping.
# See chapter 10 table 12 of http://www.usb.org/developers/hidpage/Hut1_12v2.pdf

def main
  case ARGV[0]
  when "keymap" then keymap
  when "keys" then keys
  else usage
  end
end

def usage
  $stderr.puts <<-END

  Usage: #{$0} <keymap|keys>
    keymap: emits ADB to USB keymap array declarations.
    keys:   emits ADB keycode name constant definitions.

  END
  exit(1)
end

def keymap
  puts header
  puts c_array("const uint8_t", "aek2_keymap", Map.load.adb_to_usb_lines)
end

def keys
  puts header
  puts Map.load.adb_define_lines
end

class Map
  def self.load
    new(SingleMapping.all)
  end

  def initialize(mappings)
    @mappings = mappings
  end

  def adb_to_usb_lines
    len = max_name_length
    128.times.map do |i|
      m = by_adb[i]
      "  /* 0x%02X: %-#{len}s */ 0x%02X" % [i, m.name, m.usb]
    end
  end

  def adb_define_lines
    len = max_name_length
    @mappings.map do |m|
      "#define %-#{len}s 0x%02X" % [m.name, m.adb]
    end
  end

  private

  def by_adb
    @_by_adb = build_by_adb
  end

  def build_by_adb
    Hash.new { |hash, key| hash[key] = SingleMapping.zero }.tap do |hash|
      @mappings.each { |m| hash[m.adb] = m }
    end
  end

  def max_name_length
    @mappings.map { |m| m.name.length }.max
  end

end

class SingleMapping < Struct.new(:usb, :adb, :name)
  def self.all
    MAP_VALUES.map { |values| new(*values) }
  end

  def self.zero
    new(0, 0, "")
  end
end

MAP_VALUES = [
  # USB, AEK2, NAME
  [0x27, 0x1D, "ADB_KEY_0"],
  [0x1E, 0x12, "ADB_KEY_1"],
  [0x1F, 0x13, "ADB_KEY_2"],
  [0x20, 0x14, "ADB_KEY_3"],
  [0x21, 0x15, "ADB_KEY_4"],
  [0x22, 0x17, "ADB_KEY_5"],
  [0x23, 0x16, "ADB_KEY_6"],
  [0x24, 0x1A, "ADB_KEY_7"],
  [0x25, 0x1C, "ADB_KEY_8"],
  [0x26, 0x19, "ADB_KEY_9"],
  [0x04, 0x00, "ADB_KEY_A"],
  [0x05, 0x0B, "ADB_KEY_B"],
  [0x06, 0x08, "ADB_KEY_C"],
  [0x07, 0x02, "ADB_KEY_D"],
  [0x08, 0x0E, "ADB_KEY_E"],
  [0x09, 0x03, "ADB_KEY_F"],
  [0x0A, 0x05, "ADB_KEY_G"],
  [0x0B, 0x04, "ADB_KEY_H"],
  [0x0C, 0x22, "ADB_KEY_I"],
  [0x0D, 0x26, "ADB_KEY_J"],
  [0x0E, 0x28, "ADB_KEY_K"],
  [0x0F, 0x25, "ADB_KEY_L"],
  [0x10, 0x2E, "ADB_KEY_M"],
  [0x11, 0x2D, "ADB_KEY_N"],
  [0x12, 0x1F, "ADB_KEY_O"],
  [0x13, 0x23, "ADB_KEY_P"],
  [0x14, 0x0C, "ADB_KEY_Q"],
  [0x15, 0x0F, "ADB_KEY_R"],
  [0x16, 0x01, "ADB_KEY_S"],
  [0x17, 0x11, "ADB_KEY_T"],
  [0x18, 0x20, "ADB_KEY_U"],
  [0x19, 0x09, "ADB_KEY_V"],
  [0x1A, 0x0D, "ADB_KEY_W"],
  [0x1B, 0x07, "ADB_KEY_X"],
  [0x1C, 0x10, "ADB_KEY_Y"],
  [0x1D, 0x06, "ADB_KEY_Z"],
  [0x2A, 0x33, "ADB_KEY_BKSP"],
  [0x31, 0x2A, "ADB_KEY_BSLASH"],
  [0x82, 0x39, "ADB_KEY_CAPS"],
  [0xE3, 0x37, "ADB_KEY_CMD"],
  [0x36, 0x2B, "ADB_KEY_COMMA"],
  [0xE0, 0x36, "ADB_KEY_CTRL"],
  [0x2D, 0x1B, "ADB_KEY_DASH"],
  [0x4C, 0x75, "ADB_KEY_DEL"],
  [0x4D, 0x77, "ADB_KEY_END"],
  [0x2E, 0x18, "ADB_KEY_EQUAL"],
  [0x29, 0x35, "ADB_KEY_ESC"],
  [0x3A, 0x7A, "ADB_KEY_F1"],
  [0x3B, 0x78, "ADB_KEY_F2"],
  [0x3C, 0x63, "ADB_KEY_F3"],
  [0x3D, 0x76, "ADB_KEY_F4"],
  [0x3E, 0x60, "ADB_KEY_F5"],
  [0x3F, 0x61, "ADB_KEY_F6"],
  [0x40, 0x62, "ADB_KEY_F7"],
  [0x41, 0x64, "ADB_KEY_F8"],
  [0x42, 0x65, "ADB_KEY_F9"],
  [0x43, 0x6D, "ADB_KEY_F10"],
  [0x44, 0x67, "ADB_KEY_F11"],
  [0x45, 0x6F, "ADB_KEY_F12"],
  [0x68, 0x69, "ADB_KEY_F13"],
  [0x69, 0x6B, "ADB_KEY_F14"],
  [0x6A, 0x71, "ADB_KEY_F15"],
  [0x54, 0x2C, "ADB_KEY_FSLASH"],
  [0x53, 0x32, "ADB_KEY_GRAVE"],
  [0x4A, 0x73, "ADB_KEY_HOME"],
  [0x49, 0x72, "ADB_KEY_INS"],
  [0x62, 0x52, "ADB_KEY_KP0"],
  [0x59, 0x53, "ADB_KEY_KP1"],
  [0x5A, 0x54, "ADB_KEY_KP2"],
  [0x5B, 0x55, "ADB_KEY_KP3"],
  [0x5C, 0x56, "ADB_KEY_KP4"],
  [0x5D, 0x57, "ADB_KEY_KP5"],
  [0x5E, 0x58, "ADB_KEY_KP6"],
  [0x5F, 0x59, "ADB_KEY_KP7"],
  [0x60, 0x5B, "ADB_KEY_KP8"],
  [0x61, 0x5C, "ADB_KEY_KP9"],
  [0x58, 0x4C, "ADB_KEY_KPENTER"],
  [0x67, 0x51, "ADB_KEY_KPEQUAL"],
  [0x56, 0x4E, "ADB_KEY_KPMINUS"],
  [0x63, 0x41, "ADB_KEY_KPPERIOD"],
  [0x57, 0x45, "ADB_KEY_KPPLUS"],
  [0x54, 0x4B, "ADB_KEY_KPSLASH"],
  [0x55, 0x43, "ADB_KEY_KPSTAR"],
  [0x2F, 0x21, "ADB_KEY_LBRACKET"],
  [0x30, 0x1E, "ADB_KEY_RBRACKET"],
  [0x53, 0x47, "ADB_KEY_NUM"],
  [0xE2, 0x3A, "ADB_KEY_OPTION"],
  [0x37, 0x2F, "ADB_KEY_PERIOD"],
  [0x4E, 0x79, "ADB_KEY_PGDN"],
  [0x4B, 0x74, "ADB_KEY_PGUP"],
  [0x28, 0x24, "ADB_KEY_RETURN"],
  [0x33, 0x29, "ADB_KEY_SEMICOL"],
  [0xE1, 0x38, "ADB_KEY_SHIFT"],
  [0x2C, 0x31, "ADB_KEY_SPACE"],
  [0x34, 0x27, "ADB_KEY_SQUOT"],
  [0x2B, 0x30, "ADB_KEY_TAB"],
  [0x4F, 0x3C, "ADB_KEY_RIGHT"],
  [0x50, 0x3B, "ADB_KEY_LEFT"],
  [0x51, 0x3D, "ADB_KEY_DOWN"],
  [0x52, 0x3E, "ADB_KEY_UP"],
]

def header
  <<-END
/**
 * Do not edit manually; generated by `#{$0} #{ARGV.join(" ")}`
 * #{Time.now}
 */
  END
end

def c_array(typedef, name, lines)
  "%s %s[%d] = {\n%s\n};" % [
    typedef,
    name,
    lines.length,
    lines.map { |l| "  " << l }.join(",\n")
  ]
end

main
