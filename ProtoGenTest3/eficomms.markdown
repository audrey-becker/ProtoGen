<style>
body {
    text-align:justify;
    max-width: 25cm;
    margin-left: auto;
    margin-right: auto;
    font-family: Georgia;
    counter-reset: h1counter h2counter  h3counter toc1counter toc2counter toc3counter;
 }

table {
   border: 1px solid #e0e0e0;
   border-collapse: collapse;
   margin-bottom: 25px;
}

th, td {
    border: 1px solid #e0e0e0;
    font-family: Courier, monospace;
    font-size: 90%;
    padding: 2px;
}

/*
 * Alternate colors for the table, including the heading row
 */
th {
background-color: #e0e0e0   
}
tr:nth-child(even){background-color: #e0e0e0}

h1, h2, h3, h4, h5 { font-family: Arial; }
h1 { font-size:120%; margin-bottom: 25px; }
h2 { font-size:110%; margin-bottom: 15px; }
h3 { font-size:100%; margin-bottom: 10px;}
h4, li { font-size:100%; }

caption{ font-family:Arial; font-size:85%;}

code, pre, .codelike {
    font-family: Courier, monospace;
    font-size: 100%;
    color: darkblue;
}

/*
 * Counters for the main headings
 */

h1:before {
    counter-increment: h1counter;
    content: counter(h1counter) "\00a0 ";
}
h1 {
    counter-reset: h2counter;
}

h2:before {
    counter-increment: h2counter;
    content: counter(h1counter) "." counter(h2counter) "\00a0 ";
}
h2 {
    counter-reset: h3counter;
}

h3:before {
  counter-increment: h3counter;
  content: counter(h1counter) "." counter(h2counter) "." counter(h3counter) "\00a0 ";
}

/*
 * The document title, centered
 */
doctitle {font-family: Arial; font-size:120%; font-weight: bold; margin-bottom:25px; text-align:center; display:block;}
titlepagetext {text-align:center; display:block;}

/*
 * The table of contents formatting
 */
toctitle {font-family: Arial; font-size:120%; font-weight: bold; margin-bottom:25px; display:block;}
toc1, toc2, toc3 {font-family: Arial; font-size:100%; margin-bottom:2px; display:block;}
toc1 {text-indent: 0px;}
toc2 {text-indent: 15px;}
toc3 {text-indent: 30px;}

toc1:before {
    content: counter(toc1counter) "\00a0 ";
    counter-increment: toc1counter;
}
toc1 {
    counter-reset: toc2counter;
}

toc2:before {
    content: counter(toc1counter) "." counter(toc2counter) "\00a0 ";
    counter-increment: toc2counter;
}
toc2 {
    counter-reset: toc3counter;
}

toc3:before {
  content: counter(toc1counter) "." counter(toc2counter) "." counter(toc3counter) "\00a0 ";
  counter-increment: toc3counter;
}

/* How it looks on a screen, notice the fancy hr blocks and lack of page breaks */
@media screen {
  body {
    background-color: #f0f0f0;
  }
  .page-break { display: none; }
  hr { 
    height: 25px; 
    border-style: solid; 
    border-color: gray; 
    border-width: 1px 0 0 0; 
    border-radius: 10px; 
  } 
  hr:before { 
    display: block; 
    content: ""; 
    height: 25px; 
    margin-top: -26px; 
    border-style: solid; 
    border-color: gray; 
    border-width: 0 0 1px 0; 
    border-radius: 10px; 
  }
}

/* How it looks when printed, hr turned off, in favor of page breaks*/
@media print {
  hr {display: none;}
  body {background-color: white;}
  .page-break{page-break-before: always;}
}
</style>



# IntelliJect Communications protocol

This ICD describes the communications of the IntelliJect electronic fuel
injection (EFI) system from [Power4Flight](http://www.power4flight.com/).
IntelliJect communicates using packets of data transported over asyncrhonous
serial, USB, Controller Area Network (CAN), or over a network connection. The
IntelliJect standard developers kit (SDK) includes C source code for
implementing packet encoding and decoding routines.

IntelliJect Communications protocol Protocol API is 10.

## Version

This document is for version 2.0 of the IntelliJect EFI system. The differences between version 1.x and 2.x are mostly related to the implementation of the two dimensional tables. The packet format and most of the messages remain compatible with the older versions. However the 2D tables format changed substantially - both in terms of the packet, and in how the data are used. 2.x tables are now more generic and include information that specifies what independent varible is used to index the row and column axes. Soruce code is provided in the SDK to convert from the 1.x to 2.x style tables. Any software which reads or modifies the table packets must understand the distinction and take the correct action when bridgine software versions.


## Packet format


## Packets over Controller Area Network (CAN)

In the case of transport over a CAN interface the format is modified to work with the 8 byte data limit of a CAN frame. For packets containing 8 or less data bytes the packet header and crc are discarded. In that case the packet type is included in the CAN ID, the packet data size is the CAN frame size, and the CAN layer is responsible for transporting the data without corruption.

The packet type is encoded into the CAN identifier (29 or 11 bit) by specifying a base identifier and reserving 6 bits in the identifier for holding the packet type information. See the [communications configuration packet](#EFI_PKT_COMMCONFIGURATION) for details of the CAN configuration options. The CAN configuration includes three different base identifiers: one for CAN frames IntelliJect sends, one for CAN frames IntelliJect receives, and one for broadcast CAN frames that all IntelliJects on the CAN bus can receive.

If the packet contains more than 8 bytes of data the entire packet (including header and crc) are encoded in a series of CAN frames with a special CAN identifier (packet type of zero). This process is referred to as *packet-over-CAN*. When using packet-over-CAN the CAN driver software must ensure that frames with the same identifier are transmitted in the order they are queued, and that receive frames are processed in the order they appear on the CAN bus. If this cannot be guaranteed IntelliJect should be configured to only support command and telemetry frames on the CAN bus (which are always 8 bytes or less).


## Packet identifiers

The list of packet type identifiers.

| Name                                                            | Value | Description                                                                                     |
| --------------------------------------------------------------- | :---: | ----------------------------------------------------------------------------------------------- |
| CAN transport                                                   | 0     | Identifier for CAN transport of packets with more than 8 bytes of data (packet-over-CAN schema) |
| [Fuel pump](#EFI_PKT_PUMPCONFIGURATION)                         | 1     | Fuel pump configuration information                                                             |
| [Communications](#EFI_PKT_COMMCONFIGURATION)                    | 2     | Communications configuration information                                                        |
| [Sensor](#EFI_PKT_SENSORCONFIGURATION)                          | 3     | Sensor configuration information for a single sensor                                            |
| [Throttle](#EFI_PKT_THROTTLECONFIGURATION)                      | 4     | Throttle configuration information                                                              |
| [Injector](#EFI_PKT_INJECTORCONFIGURATION)                      | 6     | Injector configuration information                                                              |
| [RPM controller](#EFI_PKT_RPMCONTROLLER)                        | 7     | RPM Controller configuration                                                                    |
| [Engine wear](#EFI_PKT_ENGINEWEAR)                              | 8     | Engine wear information                                                                         |
| [Table data](#EFI_PKT_TABLE)                                    | 9     | Table settings for a single table                                                               |
| [Table data value](#EFI_PKT_TABLEVALUE)                         | 10    | Table single value                                                                              |
| [Software version](#EFI_PKT_SOFTWAREINFO)                       | 12    | Software version information                                                                    |
| [Sticky error telemetry](#EFI_PKT_STICKY_ERRORS)                | 13    | Sticky error information                                                                        |
| [Enable/disable command](#EFI_PKT_ENABLE)                       | 14    | User enables                                                                                    |
| [Throttle command](#EFI_PKT_USERTHROTTLE)                       | 15    | Throttle command                                                                                |
| [RPM command](#EFI_PKT_RPMCOMMAND)                              | 16    | RPM command                                                                                     |
| [Packet request command](#EFI_PKT_REQUEST)                      | 17    | Request for multiple configuration packets                                                      |
| [Time telemetry](#EFI_PKT_TELEMETRYTIME)                        | 18    | Telemetry time information                                                                      |
| [Fast telemetry](#EFI_PKT_TELEMETRYFAST)                        | 19    | Fast telemetry                                                                                  |
| [Sensors telemetry](#EFI_PKT_TELEMETRYSENSORS)                  | 20    | Telemetry information for sensors                                                               |
| [Sensors2 telemetry](#EFI_PKT_TELEMETRYSENSORS2)                | 21    | Telemetry2 information for sensors                                                              |
| [Fuel consumption telemetry](#EFI_PKT_TELEMETRYFUEL)            | 22    | Fueling information                                                                             |
| [Slow telemetry](#EFI_PKT_TELEMETRYSLOW)                        | 23    | Miscellanious slow telemetry                                                                    |
| [CPU telemetry](#EFI_PKT_TELEMETRYCPU)                          | 24    | Telemetry about the CPU performance                                                             |
| [Injector telemetry](#EFI_PKT_TELEMETRYINJECTOR)                | 25    | Telemetry about the injectors                                                                   |
| [Dynamic error telemetry](#EFI_PKT_TELEMETRYERRORS)             | 26    | Dynamic error information                                                                       |
| [Not-acknowledge report](#EFI_PKT_NACK)                         | 27    | Packet not-acknowledge                                                                          |
| [Unlock command](#EFI_PKT_UNLOCK)                               | 28    | Configuration unlock                                                                            |
| [Locking](#EFI_PKT_LOCKCONTROL)                                 | 29    | Configuration locking for the system                                                            |
| [Reset report](#EFI_PKT_RESET)                                  | 30    | Report or command a reset of the system                                                         |
| [File name](#EFI_PKT_FILE)                                      | 31    | File name for the settings file                                                                 |
| [Comment](#EFI_PKT_COMMENT)                                     | 32    | Configuration comment                                                                           |
| [Storage command](#EFI_PKT_STORAGE)                             | 33    | Storage configuration command                                                                   |
| [Hardware version](#EFI_PKT_HARDWAREINFO)                       | 34    | Information about the hardware                                                                  |
| [Fast telemetry summary](#EFI_PKT_TELEMETRYFASTSUM)             | 35    | A super packet containing all the fast telemetry                                                |
| [Slow telemetry summary](#EFI_PKT_TELEMETRYSLOWSUM)             | 36    | A super packet containing all the slow telemetry                                                |
| [Bootloader command and report](#EFI_PKT_BOOTLOADER)            | 37    | Bootloader control packet                                                                       |
| [Sensors3 telemetry](#EFI_PKT_TELEMETRYSENSORS3)                | 38    | Telemetry3 information for sensors                                                              |
| [Validation](#EFI_PKT_CONFIGHASH)                               | 39    | Send this packet to flag the current configuration as valid                                     |
| [Telemetry about communications](#EFI_PKT_TELEMETRYCOMMS)       | 40    | Telemetry about communications performance and errors                                           |
| [Oscilloscope setup packet](#EFI_PKT_OSCILLOSCOPE_SETUP)        | 41    | Oscilloscope setup packet                                                                       |
| [Oscilloscope data packet](#EFI_PKT_OSCILLOSCOPE_DATA)          | 42    | Oscilloscope data packet                                                                        |
| [IntelliJect debug packet](#EFI_PKT_DEBUG)                      | 43    | Packet used for debug                                                                           |
| [IntelliJect test mode packet](#EFI_PKT_TESTMODE)               | 44    | Packet used for testing                                                                         |
| [Telemetry about the SD card](#EFI_PKT_TELEMETRYSDCARD)         | 45    | Telemetry about the SD card                                                                     |
| [Fuel used](#EFI_PKT_FUELUSED)                                  | 46    | Packet used to set or report the fuel used                                                      |
| [Sensors4 telemetry](#EFI_PKT_TELEMETRYSENSORS4)                | 48    | Telemetry4 information for sensors                                                              |
| [Extended engine wear](#EFI_PKT_ENGINEWEAREXT)                  | 49    | Extended engine wear information                                                                |
| [Maintenance schedule](#EFI_PKT_MAINTENANCECONFIGURATION)       | 50    | Maintenance schedule configuration                                                              |
| [Maintenance status](#EFI_PKT_MAINTENANCESTATUS)                | 51    | Maintenance status                                                                              |
| [Engine logbook](#EFI_PKT_LOGENTRY)                             | 52    | Engine log entry                                                                                |
| [Crank wheel timing](#EFI_PKT_CRANKSENSETIMING)                 | 53    | Crank wheel timing details                                                                      |
| [SD card journal information](#EFI_PKT_SDCARDJOURNAL)           | 54    | SD Card journal information                                                                     |
| [Interrupt details](#EFI_PKT_INTERRUPTDETAILS)                  | 55    | Detailed information about interrupts                                                           |
| [Cooling](#EFI_PKT_COOLINGCONFIGURATION)                        | 56    | Cooling configuration information                                                               |
| [Engine](#EFI_PKT_NEWENGINECONFIGURATION)                       | 57    | Engine configuration information for api 7 and later                                            |
| [GCU Telemetry](#EFI_PKT_TELEMETRYGCU)                          | 58    | Telemetry information for the generator control unit                                            |
| [Extended outputs telemetry](#EFI_PKT_TELEMETRYEXTENDEDOUTPUTS) | 59    | Telemetry for injector3, spark2, and spark3                                                     |
| [Oil Injection Telemetry](#EFI_PKT_TELEMETRYOILINJ)             | 60    | Telemetry for the oil injection system                                                          |
[<a name="efiPacketIds"></a>Packet identifiers enumeration]


<div class="page-break"></div>


---


# Packets for commanding IntelliJect at runtime

Use these packets to send commands at engine runtime. They are 8 bytes or less so they can be transmitted in a single CAN frame. Reception of commands typically do not generate a reply; instead the command reception can be verified by observing the relevant information in the telemetry output.


## <a name="EFI_PKT_ENABLE"></a>EFI Enable

Enable/disable command. Use this packet to shut down the engine by clearing `userEnable` or to selectively disable the first or second spark output by clearing `spark1Enable` or `spark2Enable`. The EFI will boot up with all the enables set. The current enable status is visible in the [fast telemetry](#EFI_PKT_TELEMETRYFAST) packet.

- packet identifier: `EFI_PKT_ENABLE` : 14
- minimum data length: 1
- maximum data length: 2


| Bytes     | Name           | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                                                                                                                    |
| --------- | -------------- | :---------: | :----: | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7       | 1)ioEnable     | B1          | 1      | Global enable based on physical input. This enable cannot be changed by this packet.                                                                                                                                                                                                                                                                                                                           |
| 0:6       | 2)userEnable   | B1          | 1      | User global enable. Set to zero to disable the engine.                                                                                                                                                                                                                                                                                                                                                         |
| 0:5       | 3)spark1Enable | B1          | 1      | User enable for spark1. Set to zero to disable spark 1.                                                                                                                                                                                                                                                                                                                                                        |
| 0:4       | 4)spark2Enable | B1          | 1      | User enable for spark2. Set to zero to disable spark 2.                                                                                                                                                                                                                                                                                                                                                        |
| 0:3       | 5)spark3Enable | B1          | 1      | User enable for spark3. Set to zero to disable spark 3.                                                                                                                                                                                                                                                                                                                                                        |
| 0:2...0:1 | 6)reserved     | B2          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                                                   |
| 0:0       | 7)crankEnable  | B1          | 1      | If set command the GCU system to crank for start, if clear command the GCU system to stop cranking. Cranking status will be visible in the GCU telemetry packet.                                                                                                                                                                                                                                               |
| 1         | 8)oilInjPrime  | U8          | 1      | Number of oil injection pulses to deliver to prime the system. This only applies if an injector is configured for oil injection. Use zero to stop the priming operation. Priming will be ignored if the system is not enabled. Priming will be stopped if the engine starts running.<br>Scaled by 0.05 from 0.0 to 5100.0.<br>This field is optional. If it is not included then the value is assumed to be 0. |
[EFI Enable packet bytes]


## <a name="EFI_PKT_USERTHROTTLE"></a>User Throttle Command

Send a user throttle command. The actual throttle used by the EFI will depend upon its configuration and the status of the throttle input sensors. This command will be ignored if the throttle configuration does not have `efiDrivesThrottle` set. The current throttle status (input, output, source) are visible in the [fast telemetry](#EFI_PKT_TELEMETRYFAST) packet.

- packet identifier: `EFI_PKT_USERTHROTTLE` : 15
- data length: 1


| Bytes | Name       | [Enc](#Enc) | Repeat | Description                                                               |
| ----- | ---------- | :---------: | :----: | ------------------------------------------------------------------------- |
| 0     | 1)throttle | U8          | 1      | The user commanded throttle in percent.<br>Scaled by 2 from 0.0 to 127.5. |
[User Throttle Command packet bytes]


## <a name="EFI_PKT_RPMCOMMAND"></a>User RPM Command

Send a RPM command; which will engage the RPM controller. The RPM command will be ignored if the throttle configuration does not have `efiDrivesThrottle` set. The RPM controller can be disengaged by sending a [user throttle](#EFI_PKT_USERTHROTTLE) command. The current RPM command value and command source is visible in the [slow telemetry](#EFI_PKT_TELEMETRYSLOW) packet.

- packet identifier: `EFI_PKT_RPMCOMMAND` : 16
- data length: 2


| Bytes | Name     | [Enc](#Enc) | Repeat | Description                                                                         |
| ----- | -------- | :---------: | :----: | ----------------------------------------------------------------------------------- |
| 0...1 | 1)rpmcmd | U16         | 1      | Engine speed command in revolutions per minute.<br>Scaled by 2 from 0.0 to 32767.5. |
[User RPM Command packet bytes]

<div class="page-break"></div>


---


# Packets for manipulating IntelliJect

Use these packets to send commands to the EFI. These commands are not typically used for normal engine running. The response to these packets will depend on the state of the engine, the lock configuration, and the lock level. For more information about configuration locking see the [lock control](#EFI_PKT_LOCKCONTROL) packet.


## EFI lock levels

Different levels of locking/unlocking

| Name                        | Value | Description                                                            |
| --------------------------- | :---: | ---------------------------------------------------------------------- |
| `EFI_LCK_LOCKED`            | 0     | EFI configuration is locked, unlock must be issued to change anything  |
| `EFI_LCK_PARTIAL_UNLOCKED`  | 1     | EFI configuration partially unlocked, some data may still be protected |
| `EFI_LCK_PASSWORD_UNLOCKED` | 2     | EFI configuration is fully unlocked, all data are unprotected          |
[<a name="efiunlocklevels"></a>EFI lock levels enumeration]



## <a name="EFI_PKT_REQUEST"></a>Request Configuration Data

Use this packet to request one or more configuration and status packets from the EFI. After receiving this packet the EFI will transmit a response packet for every bit which is set. If the EFI is configured to hide configuration data the response packet(s) may be a [nack](#EFI_PKT_NACK) rather than the desired packet(s).

- packet identifier: `EFI_PKT_REQUEST` : 17
- data length: 13


| Bytes       | Name                  | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                |
| ----------- | --------------------- | :---------: | :----: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 0...11      | 1)bits                             || 1      | One bit for every configuration packet                                                                                                                                                     |
| 0:7...1:0   | 1.1)sensors           | B16         | 1      | One bit for each of the sensors. The least significant bit is the first sensor.                                                                                                            |
| 2:7...2:5   | 1.2)reservedbits      | B3          | 1      | Remaining bits for future expansion.                                                                                                                                                       |
| 2:4         | 1.3)cooling           | B1          | 1      | The cooling configuration.                                                                                                                                                                 |
| 2:3         | 1.4)newengine         | B1          | 1      | The new engine configuration.                                                                                                                                                              |
| 2:2         | 1.5)sdjournal         | B1          | 1      | The SD Card journal data.                                                                                                                                                                  |
| 2:1         | 1.6)wear              | B1          | 1      | The engine wear data.                                                                                                                                                                      |
| 2:0         | 1.7)logbook           | B1          | 1      | The logbook data.                                                                                                                                                                          |
| 3:7         | 1.8)maintenancestatus | B1          | 1      | The maintenance status data.                                                                                                                                                               |
| 3:6         | 1.9)maintenanceconfig | B1          | 1      | The maintenance configuration data.                                                                                                                                                        |
| 3:5         | 1.10)scopesetup       | B1          | 1      | The oscilloscope setup data.                                                                                                                                                               |
| 3:4         | 1.11)confighash       | B1          | 1      | The configuration hash data.                                                                                                                                                               |
| 3:3         | 1.12)pump             | B1          | 1      | The fuel pump configuration.                                                                                                                                                               |
| 3:2         | 1.13)comms            | B1          | 1      | The communications configuration.                                                                                                                                                          |
| 3:1         | 1.14)throttle         | B1          | 1      | The throttle configuration.                                                                                                                                                                |
| 3:0         | 1.15)reservedbit      | B1          | 1      | A reserved bit for future expansion.                                                                                                                                                       |
| 4:7         | 1.16)injector         | B1          | 1      | The injector configuration.                                                                                                                                                                |
| 4:6         | 1.17)rpmcontroller    | B1          | 1      | The rpm controller configuration.                                                                                                                                                          |
| 4:5         | 1.18)lockcontrol      | B1          | 1      | The lock control information.                                                                                                                                                              |
| 4:4         | 1.19)softwareinfo     | B1          | 1      | The software information data.                                                                                                                                                             |
| 4:3         | 1.20)resetinfo        | B1          | 1      | The reset report data.                                                                                                                                                                     |
| 4:2         | 1.21)file             | B1          | 1      | The file data.                                                                                                                                                                             |
| 4:1         | 1.22)comment          | B1          | 1      | The comment data.                                                                                                                                                                          |
| 4:0         | 1.23)hardwareinfo     | B1          | 1      | The hardware information data.                                                                                                                                                             |
| 5:7...7:0   | 1.24)tables           | B24         | 1      | One bit for each of the tables. The least significant bit is the first table.                                                                                                              |
| 8:7...10:5  | 1.25)reservedbits2    | B19         | 1      | More bits for future expansion.                                                                                                                                                            |
| 10:4...11:0 | 1.26)tables2          | B13         | 1      | One bit for each of the tables above table 23. The least significant bit is table 24.                                                                                                      |
| 12:7...12:5 | 2)reserved            | B3          | 1      | More bits for future expansion.                                                                                                                                                            |
| 12:4...12:0 | 3)api                 | B5          | 1      | Application programming interface number of the protocol doing the requesting. Changes to the ICD will increment this number.<br>Data are given constant value on encode geteficommsApi(). |
[Request Configuration Data packet bytes]


## <a name="EFI_PKT_UNLOCK"></a>Unlock Command

Send this packet to unlock the EFI so configuration data can be changed (or viewed if hidden). For password locked configurations, if the password is lost you can use the string 'CLEAR ALL DATA'. This will allow the EFI to be password unlocked, but will also reset the EFI to defaults, clearing all existing configuration data from both user and factory storage. Note that the 'CLEAR ALL DATA' password cannot be used if the engine is running, in which case a [nack](#EFI_PKT_NACK) will be generated. After receiving this packet the EFI will send this packet back to the user, to report the current lock status. Anytime the EFI sends this packet the password field will be empty. The lock status is always visible in the [cpu telemetry](#EFI_PKT_TELEMETRYCPU) packet. The cpu telemetry packet also indicates if unlocking will require a password.

This packet can be used to lock the EFI by requesting an unlock level which is lower than the current level. In that case the password is not needed.

- packet identifier: `EFI_PKT_UNLOCK` : 28
- minimum data length: 2
- maximum data length: 33


| Bytes     | Name          | [Enc](#Enc)                           | Repeat | Description                                                                                                                                                                                                                  |
| --------- | ------------- | :-----------------------------------: | :----: | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7...0:2 | 1)reserved    | B6                                    | 1      | Reserved bits in the packet.                                                                                                                                                                                                 |
| 0:1...0:0 | 2)unlockLevel | B2                                    | 1      | For packets going to the EFI this is the desired unlock level. For packets coming from the EFI this the current unlock level.                                                                                                |
| 1...32    | 3)password    | Zero-terminated string up to 32 bytes         || Password used to unlock. This password must match the password used in the lock configuration. If the password is incorrect the EFI will respond with nack. When the EFI sends this packet the password field will be empty. |
[Unlock Command packet bytes]


## <a name="EFI_PKT_RESET"></a>Reset Command

Packet used to command an EFI reset. This is primarily used to reset the EFI into bootloader mode for firmware update. This packet is also used for software development testing and to test the behavior of the engine during EFI reset. This packet will not be obeyed unless the unlock level is [`EFI_LCK_PASSWORD_UNLOCKED`](#efiunlocklevels), *or* the engine is not running and the reset requested is `bootloaderReset`.

- packet identifier: `EFI_PKT_RESET` : 30
- data length: 5


| Bytes | Name                     | [Enc](#Enc) | Repeat | Description                                                                                                                  |
| ----- | ------------------------ | :---------: | :----: | ---------------------------------------------------------------------------------------------------------------------------- |
| 0:7   | 1)watchdogReset          | B1          | 1      | If set perform a reset by entering an infinite loop so that the watchdog triggers a reset.                                   |
| 0:6   | 2)softwareReset          | B1          | 1      | If set perform a software reset.                                                                                             |
| 0:5   | 3)invalidAddressReset    | B1          | 1      | If set perform a memory write to an invalid address in order to trigger a memory exception reset.                            |
| 0:4   | 4)unalignedAddressReset  | B1          | 1      | If set perform a memory write to an unaligned address in order to trigger a unaligned exception reset.                       |
| 0:3   | 5)divideByZeroReset      | B1          | 1      | If set perform a integer divide by zero in order to trigger an exception reset.                                              |
| 0:2   | 6)floatDivideByZeroReset | B1          | 1      | If set perform a floating point divide by zero in order to trigger an floating point interrupt reset.                        |
| 0:1   | 7)bootloaderReset        | B1          | 1      | If set perform a reset to enter into bootloader mode for firmware update.                                                    |
| 0:0   | 8)safemodereset          | B1          | 1      | If set perform a reset that results in safe mode.                                                                            |
| 1...4 | 9)extraVerification      | U32         | 1      | This field must be set to `0xABCD8765` or the packet will be ignored.<br>Data are given constant value on encode 0xABCD8765. |
[Reset Command packet bytes]


## <a name="EFI_PKT_STORAGE"></a>StorageLong packet

Commands for manipulating storage. The EFI maintains two storage locations for non-volatile configuration data. The user storage data is always updated whenever the configuration is changed. Any configuration data which is not present in the user storage will be supplied by the factory storage when the EFI starts up. Factory storage is intended to hold the configuration as it left the factory, providing a way for users to revert to the original configuration. Factory configuration data is stored in on-chip flash, and will take several seconds to change, therefore factory storage cannot be changed while the engine is running.

Storage commands that affect the EEPROM or flash (i.e. non-sdcard commands) will not be obeyed unless the unlock level is [`EFI_LCK_PASSWORD_UNLOCKED`](#efiunlocklevels). In addition any command which changes the factory storage will not be obeyed if the engine is running. Note that clearing the storage will not clear the engine wear information. If this command alters configuration data the EFI will send the configuration packets.

The storage command is also used with the SD card. SD card storage commands can be sent when the engine is running, and do not require the system to be unlocked. To read a record from the SD card send the command `efiStorageTransferSD` with the record number to be read. The progress of the SD transfer can be monitored with the SD card telemetry packet. When the transfer is complete this packet will be sent from the EFI with the command `efiStorageStopTransferSD` to indicate the transfer is complete. You can also stop the transfer by sending the command `efiStorageStopTransferSD`.

- packet identifier: `EFI_PKT_STORAGE` : 33
- data length: 4

Enumeration of storage commands

| Name                       | Value | Description                                                                                                                                                                     |
| -------------------------- | :---: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `efiStorageClearAll`       | 0     | Use this command to clear user and factory settings, causing the EFI to revert to hard-coded default settings. This will trigger a send of all configuration data.              |
| `efiStorageRestoreFactory` | 1     | Use this command to clear any user settings, causing the EFI to revert to any stored settings in the factory storage space. This will trigger a send of all configuration data. |
| `efiStorageSaveFactory`    | 2     | Use this command to save the current settings to factory storage. This will also clear the user storage, except for the engine wear information.                                |
| `efiStorageSetupSD`        | 3     | Use this command to setup the SD card, which lays out the root directory filesystem.                                                                                            |
| `efiStorageFormatSD`       | 4     | Use this command to format the SD card.                                                                                                                                         |
| `efiStorageEraseSD`        | 5     | Erase the SD card.                                                                                                                                                              |
| `efiStorageTransferSD`     | 6     | Transfer a record from the SD card.                                                                                                                                             |
| `efiStorageStopTransferSD` | 7     | Stop Transferring a record from the SD card.                                                                                                                                    |
[<a name="efiStorageCmds"></a>Storage Commands enumeration]





| Bytes     | Name                         | [Enc](#Enc) | Repeat | Description                                                                                                                           |
| --------- | ---------------------------- | :---------: | :----: | ------------------------------------------------------------------------------------------------------------------------------------- |
| 0         | 1)[command](#efiStorageCmds) | U8          | 1      | The storage command to execute.                                                                                                       |
| 1:7       | 2)aborted                    | B1          | 1      | If the command is `efiStorageStopTransferSD` this flag will be set if the transfer was stopped because of a problem with the SD card. |
| 1:6...1:0 | 3)reserved                   | B7          | 1      | Reserved bits in the packet.                                                                                                          |
| 2...3     | 4)sdrecord                   | U16         | 1      | The record identifier for the `efiStorageTransferSD` command. This must be a valid record in the SD card journal.                     |
[StorageLong packet bytes]


## <a name="EFI_PKT_STORAGE"></a>Storage packet

Short version of the storage command. This version can be used for all commands except the `efiStorageTransferSD`

- packet identifier: `EFI_PKT_STORAGE` : 33
- data length: 1


| Bytes | Name                         | [Enc](#Enc) | Repeat | Description                     |
| ----- | ---------------------------- | :---------: | :----: | ------------------------------- |
| 0     | 1)[command](#efiStorageCmds) | U8          | 1      | The storage command to execute. |
[Storage packet bytes]


## <a name="EFI_PKT_FUELUSED"></a>Fuel and oil used

Use this packet to set or request the total fuel and oil used. The fuel and oil totalizers start at zero when the EFI boots up and increases as the engine runs. The fuel used is also reported in the fuel telemetry packet. The oil used is also reported in the oil injection telemetry packet. Send this packet with zero length to request the fuel and oil used.

- packet identifier: `EFI_PKT_FUELUSED` : 46
- minimum data length: 2
- maximum data length: 4


| Bytes | Name              | [Enc](#Enc) | Repeat | Description                                                                                                                              |
| ----- | ----------------- | :---------: | :----: | ---------------------------------------------------------------------------------------------------------------------------------------- |
| 0...1 | 1)fuelConsumption | F16:10      | 1      | Fuel consumption in grams since the system turned on.                                                                                    |
| 2...3 | 2)oilConsumption  | F16:10      | 1      | Oil consumption in grams since the system turned on.<br>This field is optional. If it is not included then the value is assumed to be 0. |
[Fuel and oil used packet bytes]


## <a name="EFI_PKT_MAINTENANCESTATUS"></a>Maintenance status

Set or request the maintenance status. To request the status send this packet with zero length. If unlocked you can send this packet to overwrite the maintenance status (except for `engineSerial`). To change the `engineSerial` `setEngineSerial` must be set and IntelliJect must be unlocked. Typically you do not send this packet to change the maintenance status, instead you use the PerformMaintenance packet for that purpose.

- packet identifier: `EFI_PKT_MAINTENANCESTATUS` : 51
- minimum data length: 5
- maximum data length: 425


| Bytes       | Name                    | [Enc](#Enc)                                          | Repeat                                                      | Description                                                                                                                                                                                                            |
| ----------- | ----------------------- | :--------------------------------------------------: | :---------------------------------------------------------: | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7         | 1)domaintenance         | B1                                                   | 1                                                           | This bit must be clear, if not clear than this packet is interpreted as a PerformMaintenance packet. This bit will never be set when this packet comes from IntelliJect.<br>Data are given constant value on encode 0. |
| 0:6         | 2)setEngineSerial       | B1                                                   | 1                                                           | Set this bit when sending to IntelliJect to change the engine serial number. This change can only be performed if IntelliJect is unlocked. This bit will never be set when this packet comes from IntelliJect.         |
| 0:5         | 3)reserved              | B1                                                   | 1                                                           | Reserved bits in the packet.                                                                                                                                                                                           |
| 0:4...0:0   | 4)numItems              | B5                                                   | 1                                                           | The number of maintenance status items in this packet.                                                                                                                                                                 |
| 1...4       | 5)engineSerial          | U32                                                  | 1                                                           | The engine serial number. This can only be changed if IntelliJect is unlocked and `setEngineSerial` is set.                                                                                                            |
| 5...424     | 6)MaintenanceItemStatus                                                       || numItems, up to [maxMaintenanceItems](#MaxMaintenanceItems) | List of maintenance item status                                                                                                                                                                                        |
| 5:7         | 6.1)triggered           | B1                                                   | 1                                                           | This bit is set if the maintenance item has elapsed.                                                                                                                                                                   |
| 5:6...5:5   | 6.2)reserved            | B2                                                   | 1                                                           | Reserved bits in the packet.                                                                                                                                                                                           |
| 5:4...5:0   | 6.3)index               | B5                                                   | 1                                                           | The index number of this maintenance item status. the index number matches the index from the maintenance schedule configuration.                                                                                      |
| 6...10      | 6.4)servicetime                                                               || 1                                                           | The date and time when this maintenance item was last serviced. This field can only be changed with the `DoMaintenance` command                                                                                        |
| 6...8       | 6.4.1)jiffytime         | U24                                                  | 1                                                           | 10s of milliseconds of the day from 0 to 8640000.                                                                                                                                                                      |
| 9:7...9:3   | 6.4.2)day               | B5                                                   | 1                                                           | The day of the month from 1 to 31.                                                                                                                                                                                     |
| 9:2...10:7  | 6.4.3)month             | B4                                                   | 1                                                           | The month of the year from 1 (January) to 12 (December).                                                                                                                                                               |
| 10:6...10:0 | 6.4.4)year              | B7                                                   | 1                                                           | The year (AD) of the date.<br>Scaled by 1 from 2000.0 to 2127.0.                                                                                                                                                       |
| 11...14     | 6.5)lastdone            | U32                                                  | 1                                                           | The last time this maintenance item was done, zero if never.                                                                                                                                                           |
| 15...34     | 6.6)name                | Zero-terminated string up to maxNameCharacters bytes                                                              || Name of the person who did the most recent maintenance.                                                                                                                                                                |
[Maintenance status packet bytes]


## <a name="EFI_PKT_MAINTENANCESTATUS"></a>Perform maintenance

Send this packet to IntelliJect to perform a single maintenance item. Performing maintenance means clearing the triggered status, updating the `lastdone` and `name` fields on the maintenance status, and adding a log entry. This packet is never sent by IntelliJect, the response will always be the MaintenanceStatus packet, followed by a LogEntry packet.

- packet identifier: `EFI_PKT_MAINTENANCESTATUS` : 51
- minimum data length: 8
- maximum data length: 1407


| Bytes     | Name            | [Enc](#Enc)                                                                | Repeat | Description                                                                                                                                                                                                           |
| --------- | --------------- | :------------------------------------------------------------------------: | :----: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7       | 1)domaintenance | B1                                                                         | 1      | Set this bit when sending to IntelliJect to indicate that this maintenance item has been performed. This bit will never be set when this packet comes from IntelliJect.<br>Data are given constant value on encode 1. |
| 0:6...0:5 | 2)reserved      | B2                                                                         | 1      | Reserved bits in the packet.                                                                                                                                                                                          |
| 0:4...0:0 | 3)index         | B5                                                                         | 1      | The index number of the maintenance item that is being performed. the index number matches the index from the maintenance schedule configuration.                                                                     |
| 1...5     | 4)servicetime                                                                               || 1      | The date and time when this maintenance item was serviced.                                                                                                                                                            |
| 1...3     | 4.1)jiffytime   | U24                                                                        | 1      | 10s of milliseconds of the day from 0 to 8640000.                                                                                                                                                                     |
| 4:7...4:3 | 4.2)day         | B5                                                                         | 1      | The day of the month from 1 to 31.                                                                                                                                                                                    |
| 4:2...5:7 | 4.3)month       | B4                                                                         | 1      | The month of the year from 1 (January) to 12 (December).                                                                                                                                                              |
| 5:6...5:0 | 4.4)year        | B7                                                                         | 1      | The year (AD) of the date.<br>Scaled by 1 from 2000.0 to 2127.0.                                                                                                                                                      |
| 6...25    | 5)name          | Zero-terminated string up to maxNameCharacters bytes                               || Name of the person who did the maintenance.                                                                                                                                                                           |
| 26...1406 | 6)description   | Zero-terminated string up to EFI_PACKET_SIZE - maxNameCharacters - 7 bytes         || The text that goes into the log as part of performing this maintenance.                                                                                                                                               |
[Perform maintenance packet bytes]


## <a name="EFI_PKT_LOGENTRY"></a>Engine Logbook Entry

Log entry packet used to request, add, or change entries in the digital engine logbook. The logbook consists of a series of entries, each contained in a single packet. Each log entry contains a, `date`, `name`, and `description` text. In addition each log entry has an `index` that simply identifies the location of the entry in the logbook.

The interpretation of the log entry packet depends on the value of the `cmd` field, and on whether the packet was sent to, or received from, the EFI. When requesting a specific log entry, requesting the number of log entries, or deleting a log entry; you only need to send the first two bytes, identifying the log entry by its index. Note that if a log entry is deleted the indices of the log entries above the deleted entry will be changed so that the list of indices alays starts at zero and ends at 1 less than the number of entries.

When requesting the number of log entries the EFI will respond with the two byte form of this packet, with `cmd` set to `numLogEntries` and `index` giving the number of log entries. When requesting a log entry the EFI will respond with the requested entry and the `cmd` value set to `logEntry`. When deleting a log entry the EFI will respond with the two byte form of this packet, with the `cmd` set to `deleteLogEntry` and the `index` set to the deleted entry's index.

To add a new log entry send this packet with `cmd` set to `logEntry` and `index` greater than the or equal to the `numLogEntries`. The EFI will respond by echoing the new log entry. To change an existing log entry set `index` equal to the entry to change. The EFI will first delete the `index` entry (resetting all the indices of later entries) and then append the updated entry to the logbook. In this case the EFI will respond as through two separate commands were sent: a delete command followed by a new log entry.

The logbook is stored onboard IntelliJect in 128Kbytes of flash. If each entry uses the maximum amount of text there is space for at least 127 entries. Most entries will have less than the maximum text so it should be possible to have many more than 127 entries. Because the logbook is stored in flash deleting or changing an entry requires zero-ing that flash location and re-writing the entry further on. Enough log entries, deletions, or edits, will use up or fragment the log storage space until no more entries can be made. In that case log additions will result in a nack. Use the command `eraseLogBook` to completely erase the log; at which point you can reload it without framentation, and/or remove older log entries to make room for new ones. The log cannot be changed while the engine is running, attempting to do so will result in a nack.

Adding a new log entry does not require IntelliJect to be unlocked. However deleting or changing a log entry does require IntelliJect to be unlocked. The log follows the same access restrictions as the maintenance schedule.

- packet identifier: `EFI_PKT_LOGENTRY` : 52
- minimum data length: 9
- maximum data length: 1408

Commands for the log entry packet.

| Name              | Value | Description                        |
| ----------------- | :---: | ---------------------------------- |
| `logEntry`        | 0     | A new or existing log entry        |
| `requestLogEntry` | 1     | Request a specific log entry       |
| `deleteLogEntry`  | 2     | Delete a log entry                 |
| `numLogEntries`   | 3     | Request the number of log entries. |
| `eraseLogBook`    | 4     | Erase the entire log.              |
[<a name="LogEntryCommands"></a>LogEntryCommands enumeration]




Maximum number of log entries.

| Name            | Value | Description                                          |
| --------------- | :---: | ---------------------------------------------------- |
| `maxLogEntries` | 1022  | Maximum number of log entries that can be supported. |
[<a name="MaxLogEntries"></a>MaxLogEntries enumeration]





| Bytes     | Name                       | [Enc](#Enc)                                                                | Repeat | Description                                                                                                                                                                                                                                                                                                                             |
| --------- | -------------------------- | :------------------------------------------------------------------------: | :----: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7...0:5 | 1)[cmd](#LogEntryCommands) | B3                                                                         | 1      | Command for the log entry packet. The interpretation of the command depends . Use `logEntry` or `requestLogEntry` to request a log. Use `deleteLogEntry` to delete a log. Use `numLogEntries` to request the number of log entries. The log is identified by either the `time` value, of if the `time` is invalid, then by the `index`. |
| 0:4...0:2 | 2)reserved                 | B3                                                                         | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                            |
| 0:1...1:0 | 3)index                    | B10                                                                        | 1      | The index number for this log.                                                                                                                                                                                                                                                                                                          |
| 2...6     | 4)time                                                                                                 || 1      | The date and time of the log entry.                                                                                                                                                                                                                                                                                                     |
| 2...4     | 4.1)jiffytime              | U24                                                                        | 1      | 10s of milliseconds of the day from 0 to 8640000.                                                                                                                                                                                                                                                                                       |
| 5:7...5:3 | 4.2)day                    | B5                                                                         | 1      | The day of the month from 1 to 31.                                                                                                                                                                                                                                                                                                      |
| 5:2...6:7 | 4.3)month                  | B4                                                                         | 1      | The month of the year from 1 (January) to 12 (December).                                                                                                                                                                                                                                                                                |
| 6:6...6:0 | 4.4)year                   | B7                                                                         | 1      | The year (AD) of the date.<br>Scaled by 1 from 2000.0 to 2127.0.                                                                                                                                                                                                                                                                        |
| 7...26    | 5)name                     | Zero-terminated string up to maxNameCharacters bytes                               || Name of the person who entered the log entry.                                                                                                                                                                                                                                                                                           |
| 27...1407 | 6)description              | Zero-terminated string up to EFI_PACKET_SIZE - maxNameCharacters - 7 bytes         || The log entry text.                                                                                                                                                                                                                                                                                                                     |
[Engine Logbook Entry packet bytes]


## <a name="EFI_PKT_SDCARDJOURNAL"></a>SDCardJournal packet

This packet reports the SD card journal, which is used to track information written to the SD card. The journal is stored on the card and cannot be changed with this packet. The journal can be requested from the EFI by sending this packet with zero length. If no card is installed, or the card has a problem, the journal will contain no records.

- packet identifier: `EFI_PKT_SDCARDJOURNAL` : 54
- minimum data length: 14
- maximum data length: 500

| Name                | Value | Description                                                         |
| ------------------- | :---: | ------------------------------------------------------------------- |
| `maxJournalEntries` | 81    | Maximum number of journal entries that can fit in a 512 byte packet |
[<a name="NumJournal"></a>NumJournal enumeration]





| Bytes       | Name                         | [Enc](#Enc) | Repeat                                             | Description                                                                                                                                                                                                                                                                                                                                                                                |
| ----------- | ---------------------------- | :---------: | :------------------------------------------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 0...7       | 1)sdcardtm                                || 1                                                  | SD card telemetry information. The only field from this telemetry that is used for journaling is `numDataBlocks`.                                                                                                                                                                                                                                                                          |
| 0:7...0:5   | 1.1)[sdstatus](#efiSDStatus) | B3          | 1                                                  | Status of the SD card.                                                                                                                                                                                                                                                                                                                                                                     |
| 0:4...0:0   | 1.2)errorCount               | B5          | 1                                                  | The number of SD card errors experienced since bootup.                                                                                                                                                                                                                                                                                                                                     |
| 1...3       | 1.3)sequence                 | U24         | 1                                                  | If `sdstatus` is `EFI_SD_READY` this is the number of 512-byte blocks that have been written to the card since the system booted up. If `sdstatus` is `EFI_SD_SETTINGUP` this gives the progress of the directory setup in tenths of a percent. If `sdstatus` is `EFI_SD_TRANSFER` this gives the progress of the transfer in tenths of a percent. In all other states this value is zero. |
| 4...7       | 1.4)numDataBlocks            | U32         | 1                                                  | The total number of data blocks that have been written to the card, this does not include the journal blocks.                                                                                                                                                                                                                                                                              |
| 8...11      | 2)numBlocksInVolume          | U32         | 1                                                  | The number of data blocks the card volume can hold, including the journal blocks.                                                                                                                                                                                                                                                                                                          |
| 12:7...13:7 | 3)reserved                   | B9          | 1                                                  | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                               |
| 13:6...13:0 | 4)numRecords                 | B7          | 1                                                  | Number of records in the journal. This may be less than the number of records on the card.                                                                                                                                                                                                                                                                                                 |
| 14...499    | 5)journal                                 || numRecords, up to [maxJournalEntries](#NumJournal) | Journal information for records of data on the SD card                                                                                                                                                                                                                                                                                                                                     |
| 14...15     | 5.1)record                   | U16         | 1                                                  | The record identifier.                                                                                                                                                                                                                                                                                                                                                                     |
| 16...19     | 5.2)lastBlock                | U32         | 1                                                  | The last 512 byte block written for this record, relative to the start of the card volume. The first and last blocks on the card contains the journal.                                                                                                                                                                                                                                     |
[SDCardJournal packet bytes]

<div class="page-break"></div>


---


# Packets used to configure IntelliJect

These packets are used to change or report the configuration. Configuration information can be queried by sending a zero length packet, or a packet with a single byte specifying which table or sensor to request. Configuration packets can be longer than 8 bytes and are transported over CAN using the packet-over-CAN schema. When changing configuration information the packets are always sent back to the sender as confirmation of their receipt. In some cases the echoed packet contents may be modified to correct inconsistencies in the data.


## <a name="EFI_PKT_FILE"></a>File Information

The file packet gives the name of a file. This is typically used to store the name of the file that was used to configure this EFI. The file name is not used by the EFI, it is provided as a convenient means of configuration management.

- packet identifier: `EFI_PKT_FILE` : 31
- minimum data length: 11
- maximum data length: 138


| Bytes     | Name       | [Enc](#Enc)                            | Repeat | Description                                                      |
| --------- | ---------- | :------------------------------------: | :----: | ---------------------------------------------------------------- |
| 0...7     | 1)reserved | U8                                     | 8      | Reserved bytes in the packet.                                    |
| 8...9     | 2)date                                             || 1      | The date when the file configuration was sent                    |
| 8:7...8:3 | 2.1)day    | B5                                     | 1      | The day of the month from 1 to 31.                               |
| 8:2...9:7 | 2.2)month  | B4                                     | 1      | The month of the year from 1 (January) to 12 (December).         |
| 9:6...9:0 | 2.3)year   | B7                                     | 1      | The year (AD) of the date.<br>Scaled by 1 from 2000.0 to 2127.0. |
| 10...137  | 3)filename | Zero-terminated string up to 128 bytes         || The name of the file used to load configuration.                 |
[File Information packet bytes]


## <a name="EFI_PKT_COMMENT"></a>Comment packet

This packet gives a user comment to describe the configuration. The comment is not used by the EFI, it is provided as a means of configuration management.

- packet identifier: `EFI_PKT_COMMENT` : 32
- minimum data length: 9
- maximum data length: 520


| Bytes   | Name       | [Enc](#Enc)                            | Repeat | Description                                             |
| ------- | ---------- | :------------------------------------: | :----: | ------------------------------------------------------- |
| 0...7   | 1)userdata | U8                                     | 8      | 8 bytes of user writable and readable nonvolatile data. |
| 8...519 | 2)comment  | Zero-terminated string up to 512 bytes         || User comment string.                                    |
[Comment packet bytes]


## <a name="EFI_PKT_LOCKCONTROL"></a>Locking Control

This packet is used to lock the configuration and set the unlock password. Each time the EFI starts the configuration is automaticaly locked and cannot be changed unless the [unlock](#EFI_PKT_UNLOCK) command is given. EFI locking has three levels: [`EFI_LCK_LOCKED`](#efiunlocklevels), [`EFI_LCK_PARTIAL_UNLOCKED`](#efiunlocklevels), and [`EFI_LCK_PASSWORD_UNLOCKED`](#efiunlocklevels).

For each set of configuration data (tables, governor, throttle, engine, pump, sensors, comms) this packet specifies the behavior of the protections when the password *has not yet been supplied*. There are three options: read/write, read only, and hidden. When configuration data are read/write the data can be requested when the efi is locked, and can be changed when the efi unlock level is [`EFI_LCK_PARTIAL_UNLOCKED`](#efiunlocklevels). When configuration data are read only the data can be requested when the efi is locked, but changing the data requires unlocking with the password. When configuration data are hidden the data cannot be requested or changed without unlocking using the password.

Upon receipt of this packet, if the EFI is currently unlocked, the configuration will be locked. The lock control configuration data are always protected at the read only level: therefore you can always request the lock control configuration but you cannot change it without doing a password unlock. Note that when sent from the EFI this packet will never include the password.

- packet identifier: `EFI_PKT_LOCKCONTROL` : 29
- minimum data length: 13
- maximum data length: 44

Different levels of configuration data protecting

| Name                | Value | Description                                                                  |
| ------------------- | :---: | ---------------------------------------------------------------------------- |
| `EFI_PRT_READWRITE` | 0     | Configuration data is not password protected                                 |
| `EFI_PRT_READONLY`  | 1     | Configuration data can be requested but requires a password unlock to change |
| `EFI_PRT_HIDDEN`    | 2     | Configuration can only be requested or changed with a password unlock        |
[<a name="efiprotectionlevels"></a>EFI protection levels enumeration]





| Bytes     | Name                                            | [Enc](#Enc)                           | Repeat | Description                                                                                                                                                                        |
| --------- | ----------------------------------------------- | :-----------------------------------: | :----: | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7...0:6 | 1)[tablesProtection](#efiprotectionlevels)      | B2                                    | 1      | Protection level for all the table configuration data.                                                                                                                             |
| 0:5...0:4 | 2)[governorProtection](#efiprotectionlevels)    | B2                                    | 1      | Protection level for the RPM controller configuration data.                                                                                                                        |
| 0:3...0:2 | 3)[throttleProtection](#efiprotectionlevels)    | B2                                    | 1      | Protection level for the throttle configuration data.                                                                                                                              |
| 0:1...0:0 | 4)[engineProtection](#efiprotectionlevels)      | B2                                    | 1      | Protection level for the engine configuration data.                                                                                                                                |
| 1:7...1:6 | 5)[injectorProtection](#efiprotectionlevels)    | B2                                    | 1      | Protection level for the injector configuration data.                                                                                                                              |
| 1:5...1:4 | 6)[pumpProtection](#efiprotectionlevels)        | B2                                    | 1      | Protection level for the pump configuration data.                                                                                                                                  |
| 1:3...1:2 | 7)[sensorsProtection](#efiprotectionlevels)     | B2                                    | 1      | Protection level for all sensor configuration data.                                                                                                                                |
| 1:1...1:0 | 8)[commsProtection](#efiprotectionlevels)       | B2                                    | 1      | Protection level for the communications configuration data.                                                                                                                        |
| 2:7...2:6 | 9)[maintenanceProtection](#efiprotectionlevels) | B2                                    | 1      | Protection level for the maintenance and logbook data.                                                                                                                             |
| 2:5...2:4 | 10)[coolingProtection](#efiprotectionlevels)    | B2                                    | 1      | Protection level for the cooling configuration data.                                                                                                                               |
| 2:3...3:0 | 11)reserved                                     | B12                                   | 1      | Reserved bits in the packet.                                                                                                                                                       |
| 4...7     | 12)reserved                                     | U32                                   | 1      | Reserved bytes in the packet.                                                                                                                                                      |
| 8...11    | 13)reserved                                     | U32                                   | 1      | Reserved bytes in the packet.                                                                                                                                                      |
| 12...43   | 14)password                                     | Zero-terminated string up to 32 bytes         || Password for unlocking. This can be an empty string in which case no password is needed for unlocking. Requesting this packet will always return an empty string for the password. |
[Locking Control packet bytes]


## <a name="EFI_PKT_CONFIGHASH"></a>Valid Configuration

Send this packet to mark the current configuration as valid. Upon receipt of this packet, if the EFI is unlocked, a parameter hash of the configuration will be computed and stored. The EFI maintains a hash of the actual configuration which is compared with the stored value, generating a parameter mismatch error if the two values are different. In addition, if a hash is computed and stored, the EFI will be locked.

When sending this packet to the EFI the values `storedParameterHash` and `actualParameterHash` are used as a command flag. Setting these to `0x00000000` and `0xFFFFFFFF` respectively will cause the EFI to disable the parameter mismatch check (and the EFI will not be locked). Any other value results in the computation and storage of the parameter hash, triggering the parameter mismatch check.

When this packet comes from the EFI `storedParameterHash` will be `0x00000000` if the parameter mismatch check is disabled. If you change the EFI configuration (by sending a configuration packet) the EFI will recompute and resend the new configuration hash the next time it outputs slow telemetry. Alternatively you can request the configuration hash packet to get the updated `actualParameterHash`.

- packet identifier: `EFI_PKT_CONFIGHASH` : 39
- data length: 8


| Bytes | Name                  | [Enc](#Enc) | Repeat | Description                                                                                                                                                                               |
| ----- | --------------------- | :---------: | :----: | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0...3 | 1)storedParameterHash | U32         | 1      | Parameter hash that was stored the last time this packet was sent to the EFI, for packets coming from the EFI. If this value is zero the parameter mismatch check is not being performed. |
| 4...7 | 2)actualParameterHash | U32         | 1      | The parameter hash that was computed from the stored configuration. This value is ignored for packets going to the EFI, it is available only on request.                                  |
[Valid Configuration packet bytes]


## <a name="EFI_PKT_PUMPCONFIGURATION"></a>Fuel Pump Configuration

Configuration information for the fuel pump control. The fuel pump can be controlled using bang-bang feedback, or proportional-integral (PI) feedback. Bang-bang feedback simply toggles between minimum and maximum pump duty cycle based on comparing the fuel pressure error with the `fuelPressureBangBangThreshold`. In PI feedback the pump duty cycle is controlled using proportional and integral feedback of fuel pressure error combined with feedforward of fuel flow rate. The current fuel pump duty cycle is visible in the [slow telemetry](#EFI_PKT_TELEMETRYSLOW) packet.

- packet identifier: `EFI_PKT_PUMPCONFIGURATION` : 1
- minimum data length: 22
- maximum data length: 24


| Bytes       | Name                            | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                     |
| ----------- | ------------------------------- | :---------: | :----: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0...1       | 1)fuelPressureCmd               | U16         | 1      | Desired fuel pressure in kilo-Pascals.<br>Scaled by 50 from 0.0 to 1310.7.                                                                                                                                                                                                                                      |
| 2...3       | 2)fuelPressureErrorThreshold    | U16         | 1      | Fuel pressures in kilo-Pascals relative to the fuelPressureCmd which is used to define a fuel pressure error (too low or too high).<br>Scaled by 250 from 0.0 to 262.14.                                                                                                                                        |
| 4           | 3)minDutyCycle                  | U8          | 1      | Minimum pump output duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                                                    |
| 5           | 4)maxDutyCycle                  | U8          | 1      | Maximum pump output duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                                                    |
| 6...7       | 5)fuelPressureBangBangThreshold | U16         | 1      | Fuel pressure error threshold in kilo-Pascals for bang-bang pump control. If this is non-zero the fuel pump is controlled by bang-bang feedback.<br>Scaled by 250 from 0.0 to 262.14.                                                                                                                           |
| 8...9       | 6)proGain                       | F16:10      | 1      | Feedback gain from fuel pressure error (in kilo-Pascals) to fuel pump duty cycle in percent. Must be positive.                                                                                                                                                                                                  |
| 10...11     | 7)intGain                       | F16:10      | 1      | Feedback gain from fuel pressure error integral (in kilo-Pascals-seconds) to fuel pump duty cycle in percent. Must be positive.                                                                                                                                                                                 |
| 12...13     | 8)forGain                       | F16:10      | 1      | Feedforward gain from fuel flow rate in grams per minute to fuel pump duty cycle in percent. Must be positive.                                                                                                                                                                                                  |
| 14:7        | 9)rpmEnablesPump                | B1          | 1      | If set the fuel pump output will remain at 0% until a non-zero engine speed is detected. If clear the pump is allowed to run even if the engine is not running.                                                                                                                                                 |
| 14:6...14:0 | 10)primeTime                    | B7          | 1      | The amount of time, in seconds, the pump is allowed to run to prime the fuel system. If `rpmEnablesPump` is set, and if the RPM is zero, the pump will be allowed to run until 'primeTime' seconds have elapsed. If `rpmEnablesPump` is clear `primeTime` will do nothing.                                      |
| 15:7        | 11)maxIfFailure                 | B1          | 1      | If set the fuel pump output will be `maxDutyCycle` if the fuel pressure sensor is failed. Otherwise the pump output will be set according to the feedforward gain.                                                                                                                                              |
| 15:6...15:0 | 12)reserved                     | B7          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                    |
| 16...18     | 13)reserved                     | U8          | 3      | Reserved bytes in the packet.                                                                                                                                                                                                                                                                                   |
| 19          | 14)positiveRateLimit            | U8          | 1      | Rate limit on pump output, if the output is increasing, in units of % duty cycle per second. 0 will disable the positive rate limiter.<br>Scaled by 0.02 from 0.0 to 12750.0.                                                                                                                                   |
| 20          | 15)negativeRateLimit            | U8          | 1      | Rate limit on pump output, if the output is decreasing, in units of % duty cycle per second. 0 will disable the negative rate limiter.<br>Scaled by 0.02 from 0.0 to 12750.0.                                                                                                                                   |
| 21          | 16)_outputRateLimit             | U8          | 1      | Deprecated rate limit on pump output in units of % duty cycle per second. 0 will disable the positive rate limiter. This field will only be used if it is positive and `positiveRateLimit` and `negativeRateLimit` are zero.<br>Scaled by 0.2 from 0.0 to 1275.0.<br>Data are given constant value on encode 0. |
| 22:7...22:6 | 17)reserved                     | B2          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                    |
| 22:5...23:0 | 18)pwmPeriod                    | B14         | 1      | Fuel pump PWM period in microseconds.<br>Scaled by 1 from 20.0 to 16403.0.<br>This field is optional. If it is not included then the value is assumed to be 1000.                                                                                                                                               |
[Fuel Pump Configuration packet bytes]


## <a name="EFI_PKT_COMMCONFIGURATION"></a>Communication Configuration

Configuration information for communications, including Controller Area Network (CAN) setup, Universal Asynchronous Receiver Transmitter (UART) baud rate, and telemetry output options. If you select a baud rate (UART or CAN) which is too low to accommodate the telemetry output rates, the rates will be adjusted down until the expected bandwidth fits the available data rate. If you select the option `setToDefault` the UART baud rate will be set to 57600 bits per second, the fast telemetry rate will be 20Hz, and the slow telemetry rate will be 1Hz.

The CAN configuration supports multiple options and protocols. The primary CAN protocol is the *native protocol*, which implements the packet-over-CAN schema to move packets as documented in this ICD. The native protocol includes input and output base identifiers, as well as a broadcast identifier. The input and output identifier must be unique on the CAN bus; but the broadcast identifier can be common with other EFIs on the bus. If you select the option `setToDefault` the native protocol will be configured to run at 1Mbit, with long identifiers, a type shift of 16 bits, an input identifier of `0x1E00xxxx`, and an output identifier of `0x1F00xxxx` (where `xxxx` are the lower 16 bits of the EFI serial number).

In addition to the native communications protocol three other protocols are supported (Currawong, Piccolo, Performance Electronics). If you set `enableCurrawongECU` the EFI will emulate the communications protocol used by the [CE367 ECU](http://www.currawongeng.com/uav-engines/engine-control-unit/) from Currawong Engineering. The emulation of this interface includes telemetry, commands, and configuration packets, see the Currawong ECU ICD for details. The `enableCurrawongECU` bit is an option for both the UART and the CAN interface. On the UART interface the Currawong protocol runs at 57600 bits per second, and runs simultaneously with the native protocol (the EFI discriminates based on the packet format). The EFI will use the `alternativeProtocolAddress` for the address field in the Currawong serial packet. When using `enableCurrawongECU` with the CAN interface the EFI will generate CAN frames from `0x0800xxxx` through `0x0818xxxx` (where `xxxx` are the `alternativeProtocolAddress`), and will process CAN frames from `0x0900xxxx` through `0x0918xxxx` (where `xxxx` are `alternativeProtocolAddress`, or `0xFFFF`).

The second alternative protocol is enabled with the option `enablePiccoloECU`. This protocol is only available on the CAN bus. You cannot simultaneously set `enablePiccoloECU` and `enableCurrawongECU` on the CAN bus, if you do the EFI will negate `enablePiccoloECU`. When using `enablePiccoloECU` the EFI will generate CAN frames from `0x0880xxxx` through `0x0884xxxx` (where `xxxx` are the `alternativeProtocolAddress`), and will process CAN frames from `0x0990xxxx` through `0x0991xxxx` (where `xxxx` are the `alternativeProtocolAddress`, or `0xFFFF`). Details of this protocool can be had in the Piccolo communications ICD. The [Piccolo autopilot](http://www.cloudcaptech.com/products/auto-pilots) CAN bus can work with either the Piccolo ECU or the Currawong ECU protocol. When using either protocol the CAN baud rate will be forced to 1Mbit, and the `outputBaseID` and `inputBaseID` must *not* be `0x08xxxxxx` or `0x09xxxx`, or the native output and input should be disabled.

The third alternative protocol is enabled with the option `enablePE3ECU`. This is an output only protocol that emulates the CAN bus outputs of the Performance Electronics PE3 ECU. You can use the `enablePE3ECU` simultaneously with `enablePiccoloECU`, but not with `enableCurrawongECU` on the CAN bus. The PE3 protocol option does not force the CAN baud rate, and it does not follow the fast and slow telemetry rates; instead using the telemetry rates specified by the Performance Electronics protocol (20Hz, 10Hz, and 1Hz).

The fourth alternative protocol is enabled with the option `enableGCU`. This is a CAN bus protocol which should be enabled if you want to decode telemetry from the Northwest or Millswood generator control unit. This protocol can be enabled with all other protocols and does not force the CAN baud rate. If this protocol is enabled, and if a GCU is sending telemetry on the CAN bus, IntelliJect will parse the telemetry data and forward it using the [TelemetryGCU packet](#EFI_PKT_TELEMETRYGCU).

When using the Currawong, Piccolo, or PE3 alternative protocols the EFI will suppress the native protocol telemetry output on the interface. However if a native protocol packet is received on the interface the EFI will resume output of the native protocol telemetry.

- packet identifier: `EFI_PKT_COMMCONFIGURATION` : 2
- data length: 24

Enumeration for baud rates on the CAN bus

| Name               | Value | Description                               |
| ------------------ | :---: | ----------------------------------------- |
| `EFI_CAN_125K`     | 0     | Baud rate of 125 thousand bits per second |
| `EFI_CAN_250K`     | 1     | Baud rate of 250 thousand bits per second |
| `EFI_CAN_500K`     | 2     | Baud rate of 500 thousand bits per second |
| `EFI_CAN_1M`       | 3     | Baud rate of 1 million bits per second    |
| `NUM_EFI_CANBAUDS` | 4     |                                           |
[<a name="CANBaudEnums"></a>Baud rates for CAN enumeration]





| Bytes       | Name                             | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                                                                                                                                           |
| ----------- | -------------------------------- | :---------: | :----: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7         | 1)disableSDRecording             | B1          | 1      | If set SD card recording will be disabled all the time, even if the engine is running.                                                                                                                                                                                                                                                                                                                                                |
| 0:6...0:0   | 2)fastTelemetrySDRate            | B7          | 1      | SD recording rate for fast telemetry.                                                                                                                                                                                                                                                                                                                                                                                                 |
| 1           | 3)fastTelemetryRate              | U8          | 1      | Output rate for fast telemetry.                                                                                                                                                                                                                                                                                                                                                                                                       |
| 2:7         | 4)recordSDAlways                 | B1          | 1      | If set SD card recording will be enabled all the time, even if the engine is not running.                                                                                                                                                                                                                                                                                                                                             |
| 2:6...2:0   | 5)slowTelemetrySDRate            | B7          | 1      | SD recording rate for slow telemetry.                                                                                                                                                                                                                                                                                                                                                                                                 |
| 3           | 6)slowTelemetryRate              | U8          | 1      | Output rate for slow telemetry.                                                                                                                                                                                                                                                                                                                                                                                                       |
| 4:7         | 7)useSummaryPackets              | B1          | 1      | Set this bit to use summary packets for the telemetry on non-CAN interfaces.                                                                                                                                                                                                                                                                                                                                                          |
| 4:6         | 8)reserved                       | B1          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                                                                          |
| 4:5         | 9)setToDefault                   | B1          | 1      | Set this bit to configure all communications for default settings. If this bit is set all other fields are ignored when this packet is sent to the EFI.                                                                                                                                                                                                                                                                               |
| 4:4         | 10)enableCurrawongECU            | B1          | 1      | Set this bit to emulate the Currawong ECU Serial interface. If this is set the regular telemetry output in native packet format is suppressed, and the `uartBaud` is forced to 57600.                                                                                                                                                                                                                                                 |
| 4:3...4:2   | 11)reserved                      | B2          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                                                                          |
| 4:1...5:0   | 12)uartBaud                      | B10         | 1      | Baud rate for the UART in units of bits per second.<br>Scaled by 1/1200 from 0.0 to 1227600.0.                                                                                                                                                                                                                                                                                                                                        |
| 6...23      | 13)CANconfig                                          ||| Configuration data for CAN communciations                                                                                                                                                                                                                                                                                                                                                                                             |
| 6:7...6:5   | 13.1)[baud](#CANBaudEnums)       | B3          | 1      | Baud rate enumeration for the CAN bus.                                                                                                                                                                                                                                                                                                                                                                                                |
| 6:4...6:0   | 13.2)typeShift                   | B5          | 1      | The bit number of the least significant bit of the packet type ([efiPacketIds](#efiPacketIds)) in the native protocol CAN ID. This is the number of bits that the packet type will be left shifted to place it into the CAN ID.                                                                                                                                                                                                       |
| 7:7         | 13.3)longid                      | B1          | 1      | Set this bit to use 29-bit identifiers in the native protocol, else the CAN bus will use 11-bit identifiers.                                                                                                                                                                                                                                                                                                                          |
| 7:6         | 13.4)reserved                    | B1          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                                                                          |
| 7:5         | 13.5)disableNativeInput          | B1          | 1      | Set this bit to disable receipt of native protocol CAN communications (inputBaseId is don't care).                                                                                                                                                                                                                                                                                                                                    |
| 7:4...10:0  | 13.6)inputBaseId                 | B29         | 1      | The base value used to form the CAN identifier for input CAN frames. The CAN identifier is formed by left shifting the packet type according to typeShift and then ORing the result with this value.                                                                                                                                                                                                                                  |
| 11:7...11:6 | 13.7)reserved                    | B2          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                                                                          |
| 11:5        | 13.8)disableNativeOutput         | B1          | 1      | Set this bit to disable transmission of native protocol CAN communications.                                                                                                                                                                                                                                                                                                                                                           |
| 11:4...14:0 | 13.9)outputBaseId                | B29         | 1      | The base value used to form the CAN identifier for outputput CAN frames. The CAN identifier is formed by left shifting the packet type according to typeShift and then ORing the result with this value.                                                                                                                                                                                                                              |
| 15:7...15:6 | 13.10)reserved                   | B2          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                                                                          |
| 15:5        | 13.11)disableNativeBroadcast     | B1          | 1      | Set this bit to disable receipt of native protocol broadcast CAN communications (broadcastBaseId is don't care).                                                                                                                                                                                                                                                                                                                      |
| 15:4...18:0 | 13.12)broadcastBaseId            | B29         | 1      | The base value used to form the CAN identifier for broadcast input CAN frames. The CAN identifier is formed by left shifting the packet type according to typeShift and then ORing the result with this value.                                                                                                                                                                                                                        |
| 19:7        | 13.13)forcePacketOverCAN         | B1          | 1      | Set this bit to force the system to use *only* the Packet-over-CAN schema, even for packets that have 8 or less bytes of data. This option is not recommended unless the EFI must be limited to a single identifier on the CAN bus. When this option is set the EFI will only output CAN frames that match exactly the `outputBaseId`, and will only accept CAN frames that exactly match the `inputBaseId` or the `broadcastBaseId`. |
| 19:6        | 13.14)cmdAndTmOnly               | B1          | 1      | Set this bit to limit the CAN bus to command and telemetry messages only. Attempts to request or change configuration using the CAN bus will be ignored. Packet-over-CAN will not be used since all remaining packets have 8 or less bytes of data (unless `forcePacketOverCAN` is set).                                                                                                                                              |
| 19:5...19:3 | 13.15)reserved                   | B3          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                                                                          |
| 19:2        | 13.16)enableGCU                  | B1          | 1      | Set this bit to enable receive of the Generator Control Unit telemetry (Northwest or Millswood). This can be enabled simultaneously with the other alternative protocols.                                                                                                                                                                                                                                                             |
| 19:1        | 13.17)enablePE3ECU               | B1          | 1      | Set this bit to emulate the Performance Electronics PE3 ECU CAN interface. This can be set at the same time as `enablePiccoloECU`.                                                                                                                                                                                                                                                                                                    |
| 19:0        | 13.18)enableCurrawongECU         | B1          | 1      | Set this bit to emulate the Currawong ECU CAN interface.                                                                                                                                                                                                                                                                                                                                                                              |
| 20:7        | 13.19)enablePiccoloECU           | B1          | 1      | Set this bit to enable support for the Piccolo ECU CAN interface.                                                                                                                                                                                                                                                                                                                                                                     |
| 20:6...20:0 | 13.20)reserved                   | B7          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                                                                          |
| 21          | 13.21)reserved                   | U8          | 1      | Reserved bytes in the packet.                                                                                                                                                                                                                                                                                                                                                                                                         |
| 22...23     | 13.22)alternativeProtocolAddress | U16         | 1      | The 16-bit address to use with either the Currawong ECU or Piccolo ECU protocols. This address applies to Currawong ECU over serial as well as CAN.                                                                                                                                                                                                                                                                                   |
[Communication Configuration packet bytes]


## <a name="EFI_PKT_SENSORCONFIGURATION"></a>Sensors Configuration

This packet gives configuration information for a single sensor. Send this packet with just the first byte to request a specific sensor configuration. Sensor configuration includes the sensor gain and offset, the minimum and maximum sensor readings beyond which the sensor is considered bad, and the default sensor value to use if the sensor is bad or disabled. If the sensor is configured as a kty84 or kty83 temperature sensor the gain should be nominally 1 and the offset nominally 0. If `enableAutocorrect` is set the sensor's calibration will be automatically adjusted to correct errors (if possible). The only automatically corrected sensor is the MAP sensor (pressure offset is corrected against Baro when engine is off).

Sensor readings and error status are available in the [telemetry packets](#telemetrypacketsoutputbyintelliject).

- packet identifier: `EFI_PKT_SENSORCONFIGURATION` : 3
- minimum data length: 16
- maximum data length: 20

Enumeration for sensors. Not all sensors exist in all variants of IntelliJect hardware.

| Name                 | Value                 | Description                                               |
| -------------------- | :-------------------: | --------------------------------------------------------- |
| Input Voltage        | 0                     | Input voltage sensor, units of Volts                      |
| MAT                  | 1                     | Manifold air temperature sensor, units of Celsius         |
| CHT1                 | 2                     | First cylinder head temperature sensor, units of Celsius  |
| CHT2                 | 3                     | Second cylinder head temperature sensor, units of Celsius |
| MAP                  | 4                     | Manifold pressure sensor, units of kilo-Pascals           |
| Fuel pressure        | 5                     | Fuel pressure sensor, units of kilo-Pascals               |
| Analog TPS           | 6                     | Analog throttle sensor, units of percent                  |
| CPU Temp             | 7                     | Temperature of the CPU, units of Celsius                  |
| Spare temp           | 8                     | Spare temperature sensor, units of Celsius                |
| Digital Barometer    | 9                     | Barometric pressure sensor, units of kilo-Pascals         |
| Outside Air Temp     | 10                    | Temperature of the outside air, units of Celsius          |
| Current              | 11                    | Input current sensor, units of Amps                       |
| 12 Volts             | 12                    | 12 Volt rail voltage sensor, units of Volts               |
| 12 Current           | 13                    | 12 Volt rail current sensor, units of Amps                |
| Analog Barometer     | 14                    | Analog barometric pressure sensor, units of kilo-Pascals  |
[<a name="efiSensorEnum"></a>EFI Sensor List enumeration]




Size of the sensor list

| Name              | Value                   | Description                       |
| ----------------- | :---------------------: | --------------------------------- |
| `NUM_EFI_SENSORS` | EFI_SENSOR_ANALOGBARO+1 | The number of sensors in the EFI. |
[<a name="efiSensorEnumSize"></a>EFI Sensor List size enumeration]





| Bytes       | Name                       | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                   |
| ----------- | -------------------------- | :---------: | :----: | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7         | 1)enabled                  | B1          | 1      | Set to 1 to enable this sensor, disabled sensors will use the `failedvalue` or be ignored.                                                                                                                                                                                    |
| 0:6         | 2)kty84                    | B1          | 1      | Set to 1 to indicate that this sensor is a KTY84 temperature sensor.                                                                                                                                                                                                          |
| 0:5         | 3)sealedGauge              | B1          | 1      | Set to 1 to indicate this sensor is of the sealed gauge type. This only applies to the fuel pressure sensor. If clear the fuel pressure sensor is considered a normal gauge type.                                                                                             |
| 0:4...0:0   | 4)[sensor](#efiSensorEnum) | B5          | 1      | The enumeration identifying the sensor to which this packet applies.<br>Value is limited on encode to be less than or equal to EFI_SENSOR_MAXINDEX.                                                                                                                           |
| 1           | 5)filterlpf                | U8          | 1      | Cutoff frequency for the low pass filter to apply to the sensor after conversion, 0 will disable the filter.<br>Scaled by 10 from 0.0 to 25.5.                                                                                                                                |
| 2...3       | 6)offset                   | I16         | 1      | Offset to be subtracted from the raw sensor reading, as a fraction of the sensor raw span.<br>Scaled by 32767/(1) from -1.0 to 1.0.                                                                                                                                           |
| 4...7       | 7)gain                     | F32         | 1      | Gain of the sensor in output units per full scale range. Sensor output = gain(reading - offset).                                                                                                                                                                              |
| 8...9       | 8)failedvalue              | F16:10      | 1      | Sensor value to use if the sensor readings are out of range, or the sensor is disabled.                                                                                                                                                                                       |
| 10...11     | 9)min                      | F16:10      | 1      | Minimum sensor value, values below this are out of range.                                                                                                                                                                                                                     |
| 12...13     | 10)max                     | F16:10      | 1      | Maximum sensor value, values above this are out of range.                                                                                                                                                                                                                     |
| 14:7        | 11)kty83                   | B1          | 1      | Set to 1 to indicate that this sensor is a KTY83 temperature sensor.                                                                                                                                                                                                          |
| 14:6        | 12)setToDefault            | B1          | 1      | Set this bit to configure this sensor for default settings. All other settings in this packet will be ignored.                                                                                                                                                                |
| 14:5        | 13)calibrate               | B1          | 1      | Set this bit to perform a calibration for this sensor. This setting must be combined with `calibrateoffset` or `calibrategain` (but not both). This bit will never bet set when this packet comes from the EFI.                                                               |
| 14:4        | 14)calibrateoffset         | B1          | 1      | Set this bit to perform a zero calibration for this sensor. The offset of the sensor will be adjusted to make the sensor output match the calibration value. The `offset` field will be ignored in this case. This bit will never be set when this packet comes from the EFI. |
| 14:3        | 15)calibrategain           | B1          | 1      | Set this bit to perform a gain calibration for this sensor. The gain of the sensor will be adjusted to make the sensor output match the calibration value. The `gain` field will be ignored in this case. This bit will never be set when this packet comes from the EFI.     |
| 14:2        | 16)enableAutocorrect       | B1          | 1      | Set this bit to enable this sensor to be automatically corrected.                                                                                                                                                                                                             |
| 14:1        | 17)platinum1000rtd         | B1          | 1      | Set to 1 to indicate this sensor is a platinum RTD with a resistance of 1000 ohms at 0C. Offset should be set to 0, and the gain should be 1.0 if the TCR (Ohm/Ohm/C) of the sensor is 0.00385. Increase or decrease the gain to account for different TCR values.            |
| 14:0...15:0 | 18)reserved                | B9          | 1      | Reserved space for future expansion.                                                                                                                                                                                                                                          |
| 16...19     | 19)calibrationvalue        | F32         | 1      | The known sense value used with `calibrateoffset` or `calibrategain`.<br>Only included if calibrate is non-zero.                                                                                                                                                              |
[Sensors Configuration packet bytes]


## <a name="EFI_PKT_THROTTLECONFIGURATION"></a>Throttle Configuration

Configuration information specific to the throttle. There are two possible sources of throttle position sensing: The analog throttle sensor and the PWM input. If the EFI is configured to drive the throttle (`efiDrivesThrottle` is set) there are four possible sources of throttle command: The analog throttle, the PWM input, the user command, or the RPM controller. In addition, it is also possible to apply a throttle curve to convert the the commanded throttle to the actual throttle. Finally, if the EFI is configured to drive the throttle it is possible to *not* have a TPS sensor, in which case the throttle position is assumed to be equal the throttle output by the EFI.

- packet identifier: `EFI_PKT_THROTTLECONFIGURATION` : 4
- minimum data length: 8
- maximum data length: 78


| Bytes       | Name                  | [Enc](#Enc) | Repeat                                         | Description                                                                                                                                                                                                                                                                                                                                                            |
| ----------- | --------------------- | :---------: | :--------------------------------------------: | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0...1       | 1)closedPWMoutput     | U16         | 1                                              | PWM in microseconds for 0% throttle output (for `efiDrivesThrottle`).                                                                                                                                                                                                                                                                                                  |
| 2...3       | 2)openPWMoutput       | U16         | 1                                              | PWM in microseconds for 100% throttle output (for `efiDrivesThrottle`).                                                                                                                                                                                                                                                                                                |
| 4           | 3)inputfilterlpf      | U8          | 1                                              | Cutoff frequency for the low pass filter to apply to the throttle input, 0 will disable the filter. This filter always applies to TPS as well as command inputs. This filter supercedes the filter on the analog throttle sensor configuration.<br>Scaled by 10 from 0.0 to 25.5.                                                                                      |
| 5           | 4)outputRateLimit     | U8          | 1                                              | Rate limit on throttle output, in units of % throttle per second. 0 will disable the rate limiter.<br>Scaled by .2 from 0.0 to 1275.0.                                                                                                                                                                                                                                 |
| 6:7         | 5)enableAnalogCmd     | B1          | 1                                              | Set to enable the use of the analog throttle input as a throttle command (only valid if `efiDrivesThrottle` is set).                                                                                                                                                                                                                                                   |
| 6:6         | 6)enablePWMCmd        | B1          | 1                                              | Set to enable the use of the PWM input as a throttle command (only valid if `efiDrivesThrottle` is set).                                                                                                                                                                                                                                                               |
| 6:5         | 7)efiDrivesThrottle   | B1          | 1                                              | Set if the EFI is driving the throttle (through PWM out or CAN).                                                                                                                                                                                                                                                                                                       |
| 6:4         | 8)enableAnalogTPS     | B1          | 1                                              | Set to enable the use of the analog throttle input as a throttle position sensor. This cannot be set simultaneously with `enableAnalogCmd`.                                                                                                                                                                                                                            |
| 6:3         | 9)enablePWMTPS        | B1          | 1                                              | Set to enable the use of the PWM throttle input as a throttle position sensor. This cannot be set simultaneously with `enablePWMCmd`.                                                                                                                                                                                                                                  |
| 6:2         | 10)_enableCECANServo  | B1          | 1                                              | Deprecated for api 7.                                                                                                                                                                                                                                                                                                                                                  |
| 6:1         | 11)enableCECANTPS     | B1          | 1                                              | Set to enable the CAN servo position feedback as a throttle position sensor.                                                                                                                                                                                                                                                                                           |
| 6:0         | 12)enableMAPTPS       | B1          | 1                                              | Set to enable the manifold pressure (plus the MAP estimate table) as a throttle position sensor.                                                                                                                                                                                                                                                                       |
| 7...67      | 13)curve                           || 1                                              | The throttle curve relating throttle inputs to outputs. This can be disabled using the curve.enabled bit. The curve.indices are percent throttle input, and the curve.data are percent throttle output.                                                                                                                                                                |
| 7:7         | 13.1)enabled          | B1          | 1                                              | Table enable. If clear then this table is not used.                                                                                                                                                                                                                                                                                                                    |
| 7:6...7:0   | 13.2)num              | B7          | 1                                              | Number of entries in this table.                                                                                                                                                                                                                                                                                                                                       |
| 8...37      | 13.3)indices          | F16:10      | num, up to [EFI_TABLE_MAX_COLS](#efiTableSize) | List of independent variables.                                                                                                                                                                                                                                                                                                                                         |
| 38...67     | 13.4)data             | F16:10      | num, up to [EFI_TABLE_MAX_COLS](#efiTableSize) | List of dependent variables.                                                                                                                                                                                                                                                                                                                                           |
| 68          | 14)startThrottle      | U8          | 1                                              | Throttle percentage used to start the engine in percent. If there is no valid throttle command, this is used to set the throttle position.<br>Scaled by 2 from 0.0 to 127.5.<br>This field is optional. If it is not included then the value is assumed to be 15.                                                                                                      |
| 69          | 15)cecanAddress       | U8          | 1                                              | Address, from 1 to 254, that identifies a Currawong Engineering CAN servo used for the throttle control if `efiDrivesThrottle` is set. Use 0 to disable the CAN servo output. The CAN bus must be configured for 1Mbit operation to work with the Currawong Engineering CAN servo.<br>This field is optional. If it is not included then the value is assumed to be 0. |
| 70:7...70:5 | 16)reserved           | B3          | 1                                              | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                           |
| 70:4...70:0 | 17)tpsErrorThreshold  | B5          | 1                                              | Throttle position sensor error threshold in percent, used to determine if the TPS sensor has an error (if `efiDrivesThrottle` is set). Zero to disable.<br>Scaled by 1 from 0.0 to 31.0.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                           |
| 71          | 18)maxStartThrottle   | U8          | 1                                              | Maximum throttle percentage that can be used to start the engine. If the engine is cranked with throttle above this value spark and injector outputs will be suspended. Use 0% to disable the start prevention feature.<br>Scaled by 2 from 0.0 to 127.5.<br>This field is optional. If it is not included then the value is assumed to be 0.                          |
| 72...73     | 19)closedPWMinput     | U16         | 1                                              | PWM in microseconds for 0% throttle input (command or TPS), use zero to set `closedPWMinput` equal to `closedPWMoutput`.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                                                                           |
| 74...75     | 20)openPWMinput       | U16         | 1                                              | PWM in microseconds for 100% throttle input (command or TPS), use zero to set `closedPWMinput` equal to `closedPWMoutput`.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                                                                         |
| 76...77     | 21)tpsFeedbackIntGain | F16:10      | 1                                              | Integral feedback gain (/s) used to adjust throttle output to make the TPS value match the output command. The adjustment is only performed if this gain is non-zero, and if a TPS is enabled, and if `efiDrivesThrottle` is set.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                  |
[Throttle Configuration packet bytes]


## efiCrankWheelSyncStatus enumeration

States of crank wheel synchronization

| Name                   | Value | Description                                                   |
| ---------------------- | :---: | ------------------------------------------------------------- |
| `noCrankWheelSync`     | 0     | No syncrhonization information is available                   |
| `onegapCrankWheelSync` | 1     | One of the gaps has been discovered, no direction information |
| `twogapCrankWheelSync` | 2     | Both gaps have been discovered (or only one is configured)    |
| `fullCrankWheelSync`   | 3     | Big gap confirmed at the correct location                     |
[<a name="efiCrankWheelSyncStatus"></a>efiCrankWheelSyncStatus enumeration]



## efiCrankDirection enumeration

States of crank direction

| Name                    | Value | Description                                                |
| ----------------------- | :---: | ---------------------------------------------------------- |
| `noCrankDirection`      | 0     | Crank direction is unknown or cannot be known (single gap) |
| `normalCrankDirection`  | 1     | Crank direction is normal rotation                         |
| `reverseCrankDirection` | 2     | Crank direction is reverse rotation                        |
[<a name="efiCrankDirection"></a>efiCrankDirection enumeration]



## <a name="EFI_PKT_COOLINGCONFIGURATION"></a>Cooling configuration

Configuration information for the cooling system.

- packet identifier: `EFI_PKT_COOLINGCONFIGURATION` : 56
- data length: 28


| Bytes       | Name                  | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                   |
| ----------- | --------------------- | :---------: | :----: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0...1       | 1)reserved            | U16         | 1      | Reserved bytes in the packet.                                                                                                                                                                                                                 |
| 2           | 2)chtTooCold          | U8          | 1      | Head temperatures, in C, lower than this are an error (if the engine is running).<br>Scaled by 1 from 0.0 to 255.0.                                                                                                                           |
| 3           | 3)chtTooHot           | U8          | 1      | Head temperatures, in C, higher than this are an error.<br>Scaled by 1 from 0.0 to 255.0.                                                                                                                                                     |
| 4           | 4)chtDesired          | U8          | 1      | Desired operating head temperature, in C.<br>Scaled by 1 from 0.0 to 255.0.                                                                                                                                                                   |
| 5           | 5)derTime             | U8          | 1      | Time in seconds used to compute the CHT rate. Longer times will have less noise but more lag.<br>Scaled by 100 from 0.0 to 2.55.                                                                                                              |
| 6...7       | 6)trajGain            | F16:10      | 1      | Feedback trajectory gain from CHT error to CHT rate command. Must be positive. Use zero to disable the trajectory controller in favor of the classical controller.                                                                            |
| 8...9       | 7)forGain             | F16:10      | 1      | Feedforward gain from engine load (in percent) to cooling output in percent. Must be positive.                                                                                                                                                |
| 10...11     | 8)proGain             | F16:10      | 1      | Feedback gain from error (in C or C/s) to cooling output in percent. Must be positive.                                                                                                                                                        |
| 12...13     | 9)intGain             | F16:10      | 1      | Feedback gain from error (in C or C/s) integral to cooling output in percent. Must be positive.                                                                                                                                               |
| 14...15     | 10)derGain            | F16:10      | 1      | Feedback gain from error derivative to the cooling output. Must be positive.                                                                                                                                                                  |
| 16:7        | 11)enableSplitCooling | B1          | 1      | Set to enable split cooling in which CHT1 drives cooling output 1, CHT2 drives cooling output 2, and CHT3 drives cooling output 3.                                                                                                            |
| 16:6...17:0 | 12)reserved           | B15         | 1      | Reserved bits in the packet.                                                                                                                                                                                                                  |
| 18...19     | 13)closedPWM1         | U16         | 1      | PWM in microseconds for 0% cooling or cowl flap position for cooling output 1.                                                                                                                                                                |
| 20...21     | 14)openPWM1           | U16         | 1      | PWM in microseconds for 100% cooling or cowl flap position for cooling output 1.                                                                                                                                                              |
| 22...23     | 15)closedPWM2         | U16         | 1      | PWM in microseconds for 0% cooling or cowl flap position for cooling output 2.                                                                                                                                                                |
| 24...25     | 16)openPWM2           | U16         | 1      | PWM in microseconds for 100% cooling or cowl flap position for cooling output 2.                                                                                                                                                              |
| 26          | 17)cecanAddress1      | U8          | 1      | Address, from 1 to 254, that identifies the first cowl flap servo on the CAN bus. The CAN bus must be configured for 1Mbit operation to work with the Currawong Engineering CAN servo. Use address 0 to disable the first CAN servo output.   |
| 27          | 18)cecanAddress2      | U8          | 1      | Address, from 1 to 254, that identifies the second cowl flap servo on the CAN bus. The CAN bus must be configured for 1Mbit operation to work with the Currawong Engineering CAN servo. Use address 0 to disable the second CAN servo output. |
[Cooling configuration packet bytes]


## <a name="EFI_PKT_NEWENGINECONFIGURATION"></a>Engine Configuration

Configuration information for the core engine. This packet is used to set a number of options about how to sense the crank shaft position, and control the spark.

- packet identifier: `EFI_PKT_NEWENGINECONFIGURATION` : 57
- data length: 36


| Bytes       | Name                       | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                             |
| ----------- | -------------------------- | :---------: | :----: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0...1       | 1)displacement             | U16         | 1      | Engine displacement in cubic centimeters.<br>Scaled by 10 from 0.0 to 6553.5.                                                                                                                                                                                                           |
| 2:7         | 2)ignoreInputEnable        | B1          | 1      | Set to ignore the input line that enables/disabes the EFI. If set the input enable is always enabled. Note that the physical enable line must be high to trigger the CDI outputs. `ignoreInputEnable` is only useful for testing or if you are not using the CDI.                       |
| 2:6         | 3)preventNormalRotation    | B1          | 1      | Set to prevent normal direction engine running. This only applies if the crank sense system can determine direction.                                                                                                                                                                    |
| 2:5         | 4)preventReverseRotation   | B1          | 1      | Set to prevent reverse direction engine running. This only applies if the crank sense system can determine direction.                                                                                                                                                                   |
| 2:4         | 5)_enableCrank2            | B1          | 1      | Deprecated setting for enabling crank sensor 2, this setting will only be obeyed if api8 is clear.                                                                                                                                                                                      |
| 2:3         | 6)enableSpark3             | B1          | 1      |                                                                                                                                                                                                                                                                                         |
| 2:2         | 7)ignActiveHigh            | B1          | 1      | Set if the ignitions are triggered on a rising edge from the EFI, else ignition is triggered on the falling edge.                                                                                                                                                                       |
| 2:1         | 8)api8                     | B1          | 1      | This bit will be set for api8 or later packets.<br>Data are given constant value on encode 1.                                                                                                                                                                                           |
| 2:0         | 9)api7                     | B1          | 1      | This bit will be set for api7 or later packets.<br>Data are given constant value on encode 1.                                                                                                                                                                                           |
| 3           | 10)maxIgnDwellTime         | U8          | 1      | Maximum ignition dwell time in milliseconds.                                                                                                                                                                                                                                            |
| 4...5       | 11)minIgnActiveTime        | U16         | 1      | Minimum ignition active time in microseconds.                                                                                                                                                                                                                                           |
| 6...7       | 12)hardRevLimit            | U16         | 1      | Maximum engine speed, above which spark and injection are turned off. Zero to disable the rev limiter.<br>Scaled by 1 from 0.0 to 65535.0.                                                                                                                                              |
| 8...9       | 13)softRevLimit            | U16         | 1      | Engine speed above which spark and injection begin to turn off.<br>Scaled by 1 from 0.0 to 65535.0.                                                                                                                                                                                     |
| 10          | 14)rpmfilterlpf            | U8          | 1      | Cutoff frequency for the low pass filter to apply to the rpm, 0 will disable the filter.<br>Scaled by 10 from 0.0 to 25.5.                                                                                                                                                              |
| 11:7        | 15)reserved                | B1          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                            |
| 11:6...11:0 | 16)mapErrorThreshold       | B7          | 1      | Manifold pressure error threshold in kPa, used (along with the MAP estimate table) to determine if the manifold pressure has an error. Zero to disable.<br>Scaled by 2 from 0.0 to 63.5.                                                                                                |
| 12...22     | 17)crankSense1                          || 1      | Details of the crank sense configuration for crank sensor 1                                                                                                                                                                                                                             |
| 12:7        | 17.1)enableWheel           | B1          | 1      | Set to enable crank wheel sensing. The `totalCount` and `bigGapCount` must be set correctly. If they are not then enable will be cleared.                                                                                                                                               |
| 12:6        | 17.2)activeHigh            | B1          | 1      | Set if the crank sense occurs on the rising edge, else crank sense is on the falling edge.                                                                                                                                                                                              |
| 12:5        | 17.3)disableSensor         | B1          | 1      | Set to disable this sensor.                                                                                                                                                                                                                                                             |
| 12:4        | 17.4)preferThisSensor      | B1          | 1      | Set to prefer this sensor over the other one.                                                                                                                                                                                                                                           |
| 12:3...13:0 | 17.5)senseDelay            | B12         | 1      | Time delay from crank sensor signal to detection of the crank sense signal, in microseconds. This time is subtracted from the clock time of the crank sense.                                                                                                                            |
| 14...15     | 17.6)normalOffset          | U16         | 1      | Angle of the active sense event in degrees of crank rotation after TDC. For directional crank wheel sensors this is the offset for clockwise (normal) rotation.<br>Scaled by 65535/(360) from 0.0 to 360.0.                                                                             |
| 16...17     | 17.7)reverseOffset         | U16         | 1      | Angle of the active sense event in degrees of crank rotation after TDC, for directional crank wheels in the reversed rotation direction.<br>Scaled by 65535/(360) from 0.0 to 360.0.                                                                                                    |
| 18          | 17.8)totalCount            | U8          | 1      | The number of teeth on the crank wheel plus the missing teeth. This is the full count, as though no synchronization teeth were removed.                                                                                                                                                 |
| 19          | 17.9)bigGapCount           | U8          | 1      | The number of teeth missing in the big gap of the crank wheel. The end of the big gap is the crank angle synchronization point. There must always be a non-zero big gap count.                                                                                                          |
| 20          | 17.10)smallGapCount        | U8          | 1      | The number of teeth missing in the small gap of the crank wheel. The small gap count can be zero, in which case there is no small gap; and therefore no ability to detect direction of rotation. The small gap count must be less than the big gap count for a functioning crank wheel. |
| 21          | 17.11)normalIntraGapCount  | U8          | 1      | The number of teeth between the big gap and the small gap for normal rotation. Zero if there is no small gap.                                                                                                                                                                           |
| 22          | 17.12)reverseIntraGapCount | U8          | 1      | The number of teeth between the big gap and the small gap for reverse rotation. Zero if there is no small gap. The intra gap count for reversed rotation should be the same as `totalCount` - `bigGapCount` - `smallGapCount` - `normalIntraGapCount`.                                  |
| 23          | 18)reserved                | U8          | 1      | Reserved bytes in the packet.                                                                                                                                                                                                                                                           |
| 24...34     | 19)crankSense2                          || 1      | Details of the crank sense configuration for crank sensor 2                                                                                                                                                                                                                             |
| 24:7        | 19.1)enableWheel           | B1          | 1      | Set to enable crank wheel sensing. The `totalCount` and `bigGapCount` must be set correctly. If they are not then enable will be cleared.                                                                                                                                               |
| 24:6        | 19.2)activeHigh            | B1          | 1      | Set if the crank sense occurs on the rising edge, else crank sense is on the falling edge.                                                                                                                                                                                              |
| 24:5        | 19.3)disableSensor         | B1          | 1      | Set to disable this sensor.                                                                                                                                                                                                                                                             |
| 24:4        | 19.4)preferThisSensor      | B1          | 1      | Set to prefer this sensor over the other one.                                                                                                                                                                                                                                           |
| 24:3...25:0 | 19.5)senseDelay            | B12         | 1      | Time delay from crank sensor signal to detection of the crank sense signal, in microseconds. This time is subtracted from the clock time of the crank sense.                                                                                                                            |
| 26...27     | 19.6)normalOffset          | U16         | 1      | Angle of the active sense event in degrees of crank rotation after TDC. For directional crank wheel sensors this is the offset for clockwise (normal) rotation.<br>Scaled by 65535/(360) from 0.0 to 360.0.                                                                             |
| 28...29     | 19.7)reverseOffset         | U16         | 1      | Angle of the active sense event in degrees of crank rotation after TDC, for directional crank wheels in the reversed rotation direction.<br>Scaled by 65535/(360) from 0.0 to 360.0.                                                                                                    |
| 30          | 19.8)totalCount            | U8          | 1      | The number of teeth on the crank wheel plus the missing teeth. This is the full count, as though no synchronization teeth were removed.                                                                                                                                                 |
| 31          | 19.9)bigGapCount           | U8          | 1      | The number of teeth missing in the big gap of the crank wheel. The end of the big gap is the crank angle synchronization point. There must always be a non-zero big gap count.                                                                                                          |
| 32          | 19.10)smallGapCount        | U8          | 1      | The number of teeth missing in the small gap of the crank wheel. The small gap count can be zero, in which case there is no small gap; and therefore no ability to detect direction of rotation. The small gap count must be less than the big gap count for a functioning crank wheel. |
| 33          | 19.11)normalIntraGapCount  | U8          | 1      | The number of teeth between the big gap and the small gap for normal rotation. Zero if there is no small gap.                                                                                                                                                                           |
| 34          | 19.12)reverseIntraGapCount | U8          | 1      | The number of teeth between the big gap and the small gap for reverse rotation. Zero if there is no small gap. The intra gap count for reversed rotation should be the same as `totalCount` - `bigGapCount` - `smallGapCount` - `normalIntraGapCount`.                                  |
| 35          | 20)chtReference            | U8          | 1      | Reference head temperature for fuel correction, in C.<br>Scaled by 1 from 0.0 to 255.0.                                                                                                                                                                                                 |
[Engine Configuration packet bytes]


## <a name="EFI_PKT_INJECTORCONFIGURATION"></a>Injectors Configuration

Configuration information for the injector. The EFI supports three injector outputs. The injectors are operated so that the sum of all injectors is equal to the desired fuel output. The proportion of fuel that each injector outputs is controlled through the injector ratio tables. The EFI also supports injector skipping, in which short fuel injection pulses are accumulated into longer pulses for more precise control of the fuel delivery.

Injectors can be configured to operate independently, which means that the fuel multiplier that comes from temperature is determined independently for each injector. There are four temperature sensors: a dedicated MAT sensor, two dedicated CHT sensors, and a spare sensor that can be configurable as a second MAT or third CHT. If the spare temp sensor is configured as manifold temp the fuel multiplier for injector1 is based on CHT1 and MAT, the fuel multiplier for injector2 is based on CHT2 and spare temp, and the fuel multiplier for injector3 is the average of injector1 and 2. If the spare temp sensor is configured as cylinder temp the fuel multiplier for injector1 is based on CHT1 and MAT, the fule multiplier for injector2 is based on CHT2 and MAT, and the fuel multiplier for injector3 is based on spare temp and MAT.

- packet identifier: `EFI_PKT_INJECTORCONFIGURATION` : 6
- minimum data length: 16
- maximum data length: 25

Enumeration detailing the interpretation of injector phase

| Name                 | Value | Description                                                             |
| -------------------- | :---: | ----------------------------------------------------------------------- |
| `injectorPhaseEnd`   | 0     | The injector phasing describes the angle of the end of the injection    |
| `injectorPhaseMid`   | 1     | The injector phasing describes the angle of the middle of the injection |
| `injectorPhaseStart` | 2     | The injector phasing describes the angle of the start of the injection  |
[<a name="InjectorPhaseEdge"></a>InjectorPhaseEdge enumeration]





| Bytes       | Name                              | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                            |
| ----------- | --------------------------------- | :---------: | :----: | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7         | 1)enableInjector2                 | B1          | 1      | Set to enable the second injector.                                                                                                                                                                                                                                                                                     |
| 0:6         | 2)enableInjectorIndependence      | B1          | 1      | Set to allow injectors to independently compute fuel multipliers for temperature.                                                                                                                                                                                                                                      |
| 0:5         | 3)spareTempIsCHT                  | B1          | 1      | Set to assign the role of cylinder temperature 3 to the spare temp sensor.                                                                                                                                                                                                                                             |
| 0:4         | 4)spareTempIsMAT                  | B1          | 1      | Set to assign the role of manifold temperature 2 to the spare temp sensor.                                                                                                                                                                                                                                             |
| 0:3         | 5)enableInjector3                 | B1          | 1      | Set to enable the third injector.                                                                                                                                                                                                                                                                                      |
| 0:2         | 6)injectorsAreRedundant           | B1          | 1      | Set to allow injectors to operate redundantly, such that a fault on one injector causes the other injectors to pick up the fuel flow.                                                                                                                                                                                  |
| 0:1...0:0   | 7)[phaseEdge](#InjectorPhaseEdge) | B2          | 1      | The injector phase edge specifies how to apply the injector phase table, and what injector angle is reported in telemetry.                                                                                                                                                                                             |
| 1           | 8)fuelVolumePercentPerMAT         | U8          | 1      | Percent fuel volume correction per degree C of MAT difference from 15C. Should be positive (volume goes up as temp goes up).<br>Scaled by 200 from 0.0 to 1.275.                                                                                                                                                       |
| 2...3       | 9)multiplier                      | F16:10      | 1      | Overall fuel multiplier to apply to the output of the fuel table.                                                                                                                                                                                                                                                      |
| 4...5       | 10)nominalFuelPressure            | U16         | 1      | Nominal injector pressure in kilo-Pascals.<br>Scaled by 50 from 0.0 to 1310.7.                                                                                                                                                                                                                                         |
| 6...7       | 11)nominalFlowrate1               | U16         | 1      | First injector full-on flow rate at the nominal pressure at 15C fuel temperature in grams per minute.<br>Scaled by 65535/(1200) from 0.0 to 1200.0.                                                                                                                                                                    |
| 8...9       | 12)nominalFlowrate2               | U16         | 1      | Second injector full-on flow rate at the nominal pressure at 15C fuel temperature in grams per minute.<br>Scaled by 65535/(1200) from 0.0 to 1200.0.                                                                                                                                                                   |
| 10...11     | 13)minOpeningTime1                | U16         | 1      | Minimum opening time in microseconds for the first injector. Injector pulses less than this will result in erroneous fueling, so the EFI will perform injector skipping to keep the injector opening times longer than this value.                                                                                     |
| 12...13     | 14)minOpeningTime2                | U16         | 1      | Minimum opening time in microseconds for the second injector. Injector pulses less than this will result in erroneous fueling, so the EFI will perform injector skipping to keep the injector opening times longer than this value.                                                                                    |
| 14...15     | 15)fuelUsedCorrector              | F16:10      | 1      | Multiplier to apply to the accumulated fuel injected to correct for any fuel consumption reporting error. This value does not affect the engine operation, just the fuel reporting.                                                                                                                                    |
| 16...17     | 16)nominalFlowrate3               | U16         | 1      | Third injector full-on flow rate at the nominal pressure at 15C fuel temperature in grams per minute.<br>Scaled by 65535/(1200) from 0.0 to 1200.0.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                |
| 18...19     | 17)minOpeningTime3                | U16         | 1      | Minimum opening time in microseconds for the third injector. Injector pulses less than this will result in erroneous fueling, so the EFI will perform injector skipping to keep the injector opening times longer than this value.<br>This field is optional. If it is not included then the value is assumed to be 0. |
| 20:7...20:3 | 18)oilPrimingFreq                 | B5          | 1      | Frequency to run the oil injection pump for priming.<br>Scaled by 2 from 0.0 to 15.5.<br>This field is optional. If it is not included then the value is assumed to be 10.                                                                                                                                             |
| 20:2        | 19)spareTempIsOilT                | B1          | 1      | Set to assign the role of oil temperature to the spare temp sensor.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                                                                                |
| 20:1        | 20)injector2IsOilInj              | B1          | 1      | Set to enable the second injector output to be used for oil injection. This must not be set at the same time as enableInjector2.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                   |
| 20:0        | 21)injector3IsOilInj              | B1          | 1      | Set to enable the third injector output to be used for oil injection. This must not be set at the same time as enableInjector3.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                    |
| 21...22     | 22)oilForStarting                 | U16         | 1      | Oil to inject when the engine starts, in micrograms per cc of engine displacement.<br>Scaled by 1.0 from 0.0 to 65535.0.<br>This field is optional. If it is not included then the value is assumed to be 1000.0.                                                                                                      |
| 23...24     | 23)oilUsedCorrector               | F16:10      | 1      | Multiplier to apply to the accumulated oil injected to correct for any oil consumption reporting error. This value does not affect the oil injection operation, just the reporting.<br>Scaled by 100.<br>This field is optional. If it is not included then the value is assumed to be 1.0.                            |
[Injectors Configuration packet bytes]


## <a name="EFI_PKT_RPMCONTROLLER"></a>RPM Controller

Configuration information for the RPM controller. The RPM controller can only be used if the throttle configuration has `efiDrivesThrottle` set. The RPM controller functions by moving the throttle. This is not the same as the rev limiter, which functions by interrupting spark and injection.

The RPM controller can be commanded two different ways: either by directly commanding the controller with the [RPM command](#EFI_PKT_RPMCOMMAND) packet; or the RPM controller can be driven by the throttle command in order to achieve an RPM computed from the throttle command.

The RPM controller can run in two different feedback control modes. In the first mode (used if `trajGain` is zero) the feedback is based on RPM error. In the second mode the feedback is based on RPM rate error.

- packet identifier: `EFI_PKT_RPMCONTROLLER` : 7
- data length: 26


| Bytes     | Name                       | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                                                                                        |
| --------- | -------------------------- | :---------: | :----: | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0...1     | 1)minRPM                   | U16         | 1      | Minimum commandable engine speed. The RPM command will be limited to greater than or equal to this value.<br>Scaled by 1 from 0.0 to 65535.0.                                                                                                                                                                                                                                      |
| 2...3     | 2)maxRPM                   | U16         | 1      | Maximum commandable engine speed. The RPM command will be limited to less than or equal to this value. If the `maxRPM` is less than the `minRPM` the RPM controller settings will be set to defaults.<br>Scaled by 1 from 0.0 to 65535.0.                                                                                                                                          |
| 4:7       | 3)enableRPMfromThrottleCmd | B1          | 1      | Set to enable RPM command from throttle command. If this is clear the RPM governor is only enabled when receiving a direct RPM command.                                                                                                                                                                                                                                            |
| 4:6       | 4)enableLowRPMLimiter      | B1          | 1      | Set to override the throttle command (when the governor is *not* running) to prevent RPM falling below the minRPM.                                                                                                                                                                                                                                                                 |
| 4:5       | 5)enableHighRPMLimiter     | B1          | 1      | Set to override the throttle command (when the governor is *not* running) to prevent RPM exceeding the maxRPM.                                                                                                                                                                                                                                                                     |
| 4:4       | 6)reserved                 | B1          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                       |
| 4:3...4:0 | 7)updateRate               | B4          | 1      | Maximum frequency of the RPM control loop in Hertz. Slower reduces noise, faster improves responsiveness.<br>Scaled by 0.2 from 10.0 to 85.0.                                                                                                                                                                                                                                      |
| 5         | 8)throttleAtMinRPM         | U8          | 1      | The throttle in percent which is expected to yield the minimum RPM. This term is used to compute the RPM command from the throttle command when running in throttle mode. It is also used to compute the feedforward throttle term when running in direct RPM mode, and the throttle output in direct RPM mode if the engine is not yet running.<br>Scaled by 2 from 0.0 to 127.5. |
| 6...7     | 9)rpmToThrottlePower       | F16:10      | 1      | Power relationship between rpm ratio (rpmcmd-minRPM)/(maxRPM-minRPM) and throttle. This term is used to compute the RPM command from the throttle command when running in throttle mode. It is also used to compute the feedforward throttle term when running in direct RPM mode, and the throttle output in direct RPM mode if the engine is not yet running.                    |
| 8         | 10)rpmCmdfilterlpf         | U8          | 1      | Low pass filter cutoff frequency applied to the rpm command. Use zero to disable the filter.<br>Scaled by 10 from 0.0 to 25.5.                                                                                                                                                                                                                                                     |
| 9...10    | 11)maxRateCmd              | F16:10      | 1      | Maximum RPM rate command in RPM/s, zero to disable the rate limiter. Must be positive.                                                                                                                                                                                                                                                                                             |
| 11...12   | 12)trajGain                | F16:10      | 1      | Feedback trajectory gain from RPM error to RPM rate command. Must be positive. User zero to disable the trajectory controller in favor of the classical controller.                                                                                                                                                                                                                |
| 13...14   | 13)gainScaler              | F16:10      | 1      | Power applied to the inner loop gains. Positive numbers cause gains to increase as RPM increases. Negative numbers cause gains to decrease as RPM increases. 0 will cause gains to be constant with RPM.                                                                                                                                                                           |
| 15...16   | 14)forGain                 | F16:10      | 1      | Feedforward gain from RPM command to the throttle. Must be positive.                                                                                                                                                                                                                                                                                                               |
| 17...18   | 15)proGain                 | F16:10      | 1      | Feedback gain from error to the throttle. Must be positive.                                                                                                                                                                                                                                                                                                                        |
| 19...20   | 16)intGain                 | F16:10      | 1      | Feedback gain from error integral to the throttle. Must be positive.                                                                                                                                                                                                                                                                                                               |
| 21...22   | 17)derGain                 | F16:10      | 1      | Feedback gain from error derivative to the throttle. Must be positive.                                                                                                                                                                                                                                                                                                             |
| 23        | 18)derivativeTime          | U8          | 1      | Time in seconds used to compute the RPM rate. Longer times will have less noise but more lag.<br>Scaled by 255/(1.0) from 0.0 to 1.0.                                                                                                                                                                                                                                              |
| 24        | 19)outputfilterlpf         | U8          | 1      | Low pass filter cutoff frequency applied to the rpm controller output. Use zero to disable the filter.<br>Scaled by 10 from 0.0 to 25.5.                                                                                                                                                                                                                                           |
| 25        | 20)outputsoftband          | U8          | 1      | Percent throttle motion that falls into the 'softband', which increases the low pass filter strength. Use zero to disable the softband.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                          |
[RPM Controller packet bytes]


## <a name="EFI_PKT_MAINTENANCECONFIGURATION"></a>Maintenance schedule

Set or request the maintenance schedule. The maintenance schedule is a list of maintenance service actions. You can see if any maintenance item is pending by checking the `maintenanceNeeded` bit of the [errors packet](#EFI_PKT_TELEMETRYERRORS). Details of the maintenance status are provided in the [maintenance status](#EFI_PKT_MAINTENANCESTATUS) packet. If you use this packet to change the number of maintenance items, the maintenance status will also be updated to keep the status items in sync with the schedule.

- packet identifier: `EFI_PKT_MAINTENANCECONFIGURATION` : 50
- minimum data length: 4
- maximum data length: 956


| Bytes     | Name                                | [Enc](#Enc)                           | Repeat                                                      | Description                                                                                                                             |
| --------- | ----------------------------------- | :-----------------------------------: | :---------------------------------------------------------: | --------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7...0:5 | 1)reserved                          | B3                                    | 1                                                           | Reserved bits in the packet.                                                                                                            |
| 0:4...0:0 | 2)numItems                          | B5                                    | 1                                                           | The number of maintenance items.                                                                                                        |
| 1         | 3)hotTimeGain                       | U8                                    | 1                                                           | Hot time multiplier gain in multiplier per degree overtemp.<br>Scaled by 100 from 0.0 to 2.55.                                          |
| 2         | 4)highLoadThreshold                 | U8                                    | 1                                                           | Throttle position above which high load time begins accumulating.<br>Scaled by 2 from 0.0 to 127.5.                                     |
| 3         | 5)reserved                          | U8                                    | 1                                                           | Reserved bytes in the packet.                                                                                                           |
| 4...955   | 6)MaintenanceItemConfig                                                    || numItems, up to [maxMaintenanceItems](#MaxMaintenanceItems) | List of maintenance item                                                                                                                |
| 4:7...4:3 | 6.1)reserved                        | B5                                    | 1                                                           | Reserved bits in the packet.                                                                                                            |
| 4:2...4:0 | 6.2)[trigger](#MaintenanceTriggers) | B3                                    | 1                                                           | The trigger for this maintenance item.                                                                                                  |
| 5...7     | 6.3)interval                        | U24                                   | 1                                                           | The interval at which this maintenance should be done. For time triggers this is in seconds. Use zero to disable this maintenance item. |
| 8...71    | 6.4)description                     | Zero-terminated string up to 64 bytes                                                              || Brief description of the maintenance to be performed.                                                                                   |
[Maintenance schedule packet bytes]


## <a name="EFI_PKT_HARDWAREINFO"></a>Hardware Information

Hardware and manufacturing information. This data cannot be changed, only requested from the EFI.

- packet identifier: `EFI_PKT_HARDWAREINFO` : 34
- data length: 14


| Bytes       | Name                  | [Enc](#Enc) | Repeat | Description                                                                                                                                |
| ----------- | --------------------- | :---------: | :----: | ------------------------------------------------------------------------------------------------------------------------------------------ |
| 0...3       | 1)serial              | U32         | 1      | 32-bit serial number.                                                                                                                      |
| 4           | 2)mattempcurrentdev   | I8          | 1      | Deviation from the nominal current in Amps generated by the driver of the MAT sensor.<br>Scaled by 1000000.0 from -0.000127 to 0.000127.   |
| 5           | 3)sparetempcurrentdev | I8          | 1      | Deviation from the nominal current in Amps generated by the driver of the Spare sensor.<br>Scaled by 1000000.0 from -0.000127 to 0.000127. |
| 6           | 4)cht1tempcurrentdev  | I8          | 1      | Deviation from the nominal current in Amps generated by the driver of the CHT1 sensor.<br>Scaled by 1000000.0 from -0.000127 to 0.000127.  |
| 7           | 5)cht2tempcurrentdev  | I8          | 1      | Deviation from the nominal current in Amps generated by the driver of the CHT2 sensor.<br>Scaled by 1000000.0 from -0.000127 to 0.000127.  |
| 8:7...8:4   | 6)reserved            | B4          | 1      |                                                                                                                                            |
| 8:3...9:0   | 7)nomtempcurrent      | B12         | 1      | Nominal current from the temp sensor drivers in Amps.<br>Scaled by 1000000 from 0.0 to 0.004095.                                           |
| 10          | 8)revMajor            | U8          | 1      | Major hardware revision number.                                                                                                            |
| 11          | 9)revMinor            | U8          | 1      | Minor hardware revision number.                                                                                                            |
| 12...13     | 10)date                            || 1      | The manufacture date of the hardware                                                                                                       |
| 12:7...12:3 | 10.1)day              | B5          | 1      | The day of the month from 1 to 31.                                                                                                         |
| 12:2...13:7 | 10.2)month            | B4          | 1      | The month of the year from 1 (January) to 12 (December).                                                                                   |
| 13:6...13:0 | 10.3)year             | B7          | 1      | The year (AD) of the date.<br>Scaled by 1 from 2000.0 to 2127.0.                                                                           |
[Hardware Information packet bytes]


## <a name="EFI_PKT_SOFTWAREINFO"></a>Software Information

Software version information. This data cannot be changed, only requested.

- packet identifier: `EFI_PKT_SOFTWAREINFO` : 12
- minimum data length: 20
- maximum data length: 275


| Bytes       | Name             | [Enc](#Enc)                            | Repeat | Description                                                                  |
| ----------- | ---------------- | :------------------------------------: | :----: | ---------------------------------------------------------------------------- |
| 0...3       | 1)swHash         | U32                                    | 1      | Git hash number (short version) from the software repository for this build. |
| 4:7         | 2)released       | B1                                     | 1      | Set if software is released, otherwise this software is still being tested.  |
| 4:6...4:0   | 3)swMajorVersion | B7                                     | 1      | Major software revision number.                                              |
| 5           | 4)swMinorVersion | U8                                     | 1      | Minor software revision number.                                              |
| 6...7       | 5)reserved       | U16                                    | 1      | Reserved bytes in the packet.                                                |
| 8...9       | 6)swBuild        | U16                                    | 1      | Build number for this software. Larger build numbers are newer software.     |
| 10...11     | 7)date                                                   || 1      | The release date of the software                                             |
| 10:7...10:3 | 7.1)day          | B5                                     | 1      | The day of the month from 1 to 31.                                           |
| 10:2...11:7 | 7.2)month        | B4                                     | 1      | The month of the year from 1 (January) to 12 (December).                     |
| 11:6...11:0 | 7.3)year         | B7                                     | 1      | The year (AD) of the date.<br>Scaled by 1 from 2000.0 to 2127.0.             |
| 12...15     | 8)codecrc        | U32                                    | 1      | The crc for the software load.                                               |
| 16...18     | 9)size           | U24                                    | 1      | The size of the program load in bytes.                                       |
| 19...274    | 10)description   | Zero-terminated string up to 256 bytes         || Human readable description of the software release.                          |
[Software Information packet bytes]

<div class="page-break"></div>


---


# Health and usage monitoring (HUMS)

IntelliJect has a HUMS consisting of packets for [maintenance schedule](#EFI_PKT_MAINTENANCECONFIGURATION), [maintenance status](#EFI_PKT_MAINTENANCESTATUS), [engine wear packet](#EFI_PKT_ENGINEWEAR), and [digital logbook](#EFI_PKT_LOGENTRY). The goal of these packets is to replace the typical paper recording used for engine logging and maintenance, and to encourage the user to follow best practices regarding engine maintenance.

The process starts with engine wear monitoring. Engine wear is continuously accumulated by IntelliJect and reported as part of the telemetry. It includes Hobbs time, number of engine revolutions, hot time, high load time, number of starts, and peak CHT that was recorded. See the [engine wear packet](#EFI_PKT_ENGINEWEAR) for more details.

The [maintenance schedule](#EFI_PKT_MAINTENANCECONFIGURATION) is a configuration packet that specifies a list of service actions that should be taken in response to one of the variables in the engine wear. A common example would be changing the spark plugs after some period of engine time, or performing a hot section inspection if the CHT becomes too hot. IntelliJect compares the engine wear data to the maintenance schedule and status, and sets an [error bit](#EFI_PKT_TELEMETRYERRORS) if a maintenance item is due.

The [maintenance status](#EFI_PKT_MAINTENANCESTATUS) records the status of a service item in the maintenance schedule. When service is performed the `domaintenance` command is used to inform IntelliJect; which then clears the `maintenanceNeeded` error bit, updates the status with the date and time of the service, name of the service technician, and relevant variable from the engine wear. Maintenance status is *not* part of the configuration, and `domaintenance` can be commanded without unlocking the configuration. Maintenance status also includes an engine serial number, which can be set if the IntelliJect is unlocked.

The final HUMS packet is the [log entry](#EFI_PKT_LOGENTRY), which provides a simple list of log entries (one packet per entry). A log entry consists of a date and time, a user name, and a text description. Whenever maintenance is performed IntelliJect automatically adds a log entry with the details of the maintenance. A log entry can also be created manually and can be added without unlocking the configuration. It is possible to edit the log if the IntelliJect is unlocked.


## MaintenanceTriggers enumeration

Triggers for maintenance items

| Name                     | Value | Description                                                           |
| ------------------------ | :---: | --------------------------------------------------------------------- |
| Run time                 | 0     | Maintenance is triggered by engine run time                           |
| Revolutions              | 1     | Maintenance is triggered by number of revolutions                     |
| Hot time                 | 2     | Maintenance is triggered by engine time above CHT limit               |
| Load time                | 3     | Maintenance is triggered by engine time above the high load threshold |
| Starts                   | 4     | Maintenance is triggered by number of engine starts                   |
| CHT                      | 5     | Maintenance is triggered by cylinder head temperature                 |
| `numMaintenanceTriggers` | 6     |                                                                       |
[<a name="MaintenanceTriggers"></a>MaintenanceTriggers enumeration]



## MaxMaintenanceItems enumeration

Maximum number of maintenance items allowed in the schedule.

| Name                  | Value | Description                                                  |
| --------------------- | :---: | ------------------------------------------------------------ |
| `maxMaintenanceItems` | 14    | Maximum number of maintenance items allowed in the schedule. |
[<a name="MaxMaintenanceItems"></a>MaxMaintenanceItems enumeration]



## MaxNameCharacters enumeration

Maximum number of characters in a user name.

| Name                | Value | Description                                  |
| ------------------- | :---: | -------------------------------------------- |
| `maxNameCharacters` | 20    | Maximum number of characters in a user name. |
[<a name="MaxNameCharacters"></a>MaxNameCharacters enumeration]


<div class="page-break"></div>


---


# IntelliJect tables

Much of the configuration is done through two dimensional tables. Each table has row and column indices which can be selected from a list of [variables](#efiTableIndex). The most common choice of index variables are throttle and rpm.

For each table the EFI performs a bilinear interpolated lookup based on the current operating condition to compute that tables output. The lookup is *not* extrapolated beyond the limits of the table indices. Each table can be enabled or disabled.

Although tables support two dimensions it is acceptable to have only a single row, or a single column, or even just a single value. Tables with zero rows or zero columns are considered empty and are treated the same as tables which are disabled. If a table has only one row or one column then the index variable can be set to `null`. The maximum table size is 21 rows by 15 columns ([`EFI_TABLE_MAX_ROWS` x `EFI_TABLE_MAX_COLS`](#efiTableSize)).

In addition to the value of each cell in the table, each cell also supports an 8-bit metadata value. IntelliJect does not use the metadata, it is provided simply so that per-cell configuration information can be included in the table (for example, a specific metadata value may indicate that a cell value has been dyno calibrated).


## EFI Table List

List of tables used in the EFI.

| Name                        | Value | Description                                                                         |
| --------------------------- | :---: | ----------------------------------------------------------------------------------- |
| Fuel (μg/cc/rev)           | 0     | Fuel as micro-grams per revolution per cc of displacement.                          |
| Spark Advance (&deg;)          | 1     | Spark1 advance in degrees.                                                          |
| Spark2 Delay (&deg;)           | 2     | Delay of spark2 after spark1 in degrees.                                            |
| Charge Temp coefficient     | 3     | Fraction of CHT (versus MAT) to use in the determination of the charge temperature. |
| Fuel multiplier 1           | 4     | First fuel multiplier.                                                              |
| Injector1 Phase (&deg;)        | 5     | First injector angle in degrees.                                                    |
| Injector1 Trim (μs)        | 6     | First injector start time in microseconds.                                          |
| Min Throttle (%)            | 7     | First minimum throttle in percent.                                                  |
| Max Throttle (%)            | 8     | First maximum throttle in percent.                                                  |
| Spark adder 1 (&deg;)          | 9     | First spark adder in degrees.                                                       |
| Fuel multiplier 2           | 10    | Second fuel multiplier.                                                             |
| Fuel multiplier 3           | 11    | Third fuel multiplier.                                                              |
| MAP Estimate                | 12    | Ratio of manifold to baro pressure (for failed map sensor).                         |
| Shaft Power (W)             | 13    | Shaft power in Watts on standard day.                                               |
| Injector2 Ratio (%)         | 14    | Percentage of injector 2 versus injectors 1 and 3 in providing the total fuel flow. |
| Injector2 Phase (&deg;)        | 15    | Second injector angle in degrees.                                                   |
| Injector2 Trim (μs)        | 16    | Second injector start time in microseconds.                                         |
| Fuel multiplier 4           | 17    | Fourth fuel multiplier.                                                             |
| Ign Dwell (μs)             | 18    | Microseconds of time the ignition output is inactive between sparks.                |
| Spark adder 2 (&deg;)          | 19    | Second spark adder in degrees.                                                      |
| Fuel multiplier 5           | 20    | Fifth fuel multiplier.                                                              |
| Injector3 Ratio (%)         | 21    | Percentage of injector 3 versus injectors 1 and 2 in providing the total fuel flow. |
| Injector3 Phase (&deg;)        | 22    | Third injector angle in degrees.                                                    |
| Injector3 Trim (μs)        | 23    | Third injector start time in microseconds.                                          |
| Spark3 Delay (&deg;)           | 24    | Delay of spark3 after spark1 in degrees.                                            |
| Min Throttle2 (%)           | 25    | Second minimum throttle in percent.                                                 |
| Max Throttle2 (%)           | 26    | Second maximum throttle in percent.                                                 |
| Fuel multiplier 6           | 27    | Sixth fuel multiplier.                                                              |
| Fuel multiplier 7           | 28    | Seventh fuel multiplier.                                                            |
| Fuel to oil ratio           | 29    | Desired ratio of fuel to oil.                                                       |
| Oil pump duty cycle (%)     | 30    | Percentage duty cycle of the oil injection pump.                                    |
| Oil pump output (μg/cycle) | 31    | Output of the oil injection pump per pulse in micro-grams of oil.                   |
| Spark adder 3 (&deg;)          | 32    | Third spark adder in degrees.                                                       |
| User 1                      | 33    | First user table.                                                                   |
| User 2                      | 34    | Second user table.                                                                  |
| User 3                      | 35    | Third user table.                                                                   |
| User 4                      | 36    | Fourth user table.                                                                  |
[<a name="efiTableList"></a>EFI Table List enumeration]



## EFI Table Size

The maximum size of an EFI table

| Name                 | Value | Description                            |
| -------------------- | :---: | -------------------------------------- |
| `EFI_TABLE_MAX_ROWS` | 21    | Maximum number of rows in an EFI table |
| `EFI_TABLE_MAX_COLS` | 15    | Maximum number of cols in an EFI table |
[<a name="efiTableSize"></a>EFI Table Size enumeration]



## EFI Table Index Variable List

List of variables that can be used as row or column indexes in a table, or used as multipliers or dividers of table output

| Name                  | Value | Description                                                                           |
| --------------------- | :---: | ------------------------------------------------------------------------------------- |
| 1.0                   | 0     | The value of 1.0.                                                                     |
| Throttle (%)          | 1     | Position of the throttle in percent.                                                  |
| Load (%)              | 2     | Percent ratio of manifold pressure to barometric pressure.                            |
| RPM                   | 3     | Engine speed in revolutions per minute.                                               |
| Baro ratio            | 4     | Ratio of barometric air pressure to standard day pressure.                            |
| MAP ratio             | 5     | Ratio of manifold air pressure to standard day pressure.                              |
| Charge temp ratio     | 6     | Ratio of absolute charge temperature to absolute charge temperature reference.        |
| MAT ratio             | 7     | Ratio of manifold air absolute temperature to standard day absolute temperature.      |
| Density ratio         | 8     | Ratio of air density to standard day density.                                         |
| 1 - Density ratio     | 9     | 1.0 - the ratio of air density to standard day density.                               |
| MAT (C)               | 10    | Average manifold air temperature.                                                     |
| Charge temp (C)       | 11    | Charge temperature.                                                                   |
| CHT (C)               | 12    | Average cylinder head temperature.                                                    |
| CHT Cold (C)          | 13    | Coldest cylinder head temperature.                                                    |
| CHT Hot (C)           | 14    | Hottest cylinder head temperature.                                                    |
| MAP (kPa)             | 15    | Manifold air pressure.                                                                |
| Baro (kPa)            | 16    | Barometric air pressure.                                                              |
| Fuel pressure (kPa)   | 17    | Augmented fuel pressure (fuelp + baro - map).                                         |
| Raw fuel press. (kPa) | 18    | Gauge fuel pressure.                                                                  |
| Voltage (V)           | 19    | Voltage of the injector rail.                                                         |
| Revolutions           | 20    | Engine revolutions since start.                                                       |
| Time (s)              | 21    | Elapsed time since engine start.                                                      |
| Throttle rate (%/s)   | 22    | Rate of change of the throttle position.                                              |
| Pos throt. rate (%/s) | 23    | Rate of change of the throttle position, if the throttle rate is positive, else zero. |
| Neg throt. rate (%/s) | 24    | Rate of change of the throttle position, if the throttle rate is negative, else zero. |
| RPM rate (rpm/s)      | 25    | Rate of change of the engine speed.                                                   |
| Pos RPM rate (rpm/s)  | 26    | Rate of change of the engine speed, if the rpm rate is positive, else zero.           |
| Neg RPM rate (rpm/s)  | 27    | Rate of change of the engine speed, if the rpm rate is negative, else zero.           |
| Oil temperature (C)   | 28    | Oil temperature.                                                                      |
| Oil duty cycle (%)    | 29    | Duty cycle of the oil injection pump.                                                 |
| Oil on time (ms)      | 30    | The amount of time the oil injection pump is on per cycle.                            |
| User 1                | 31    | Output of the user 1 table                                                            |
| User 2                | 32    | Output of the user 2 table                                                            |
| User 3                | 33    | Output of the user 3 table                                                            |
| User 4                | 34    | Output of the user 4 table                                                            |
[<a name="efiTableIndex"></a>EFI Table Index Variable List enumeration]



## Fuel

The fuel table (`EFI_TABLE_FUEL`) gives the total amount of fuel to be injected, in units of micro-grams of fuel per revolution, per cubic centimeter of engine displacement. The output of the fuel table is the amount of fuel to inject, if all fuel multipliers are 1.0. The default indices are [throttle, rpm]


## Spark 1 advance

The spark 1 advance table (`EFI_TABLE_SPARK1ADV`) gives the amount of crank rotation, in degrees before top dead center, when the spark 1 output should be triggered, if all spark adders are 0. The default indices are [throttle, rpm].


## Spark 2 delay

The spark 2 delay table (`EFI_TABLE_SPARK2DELAY`) gives the amount of crank rotation in degrees after spark 1 output to trigger the spark 2 output. It is common for the spark 2 delay table to be disabled, or to be all zeros, in which case spark 2 will fire at the same time as spark 1. The default indices are [throttle, rpm].


## Charge temperature

The charge temperature table (`EFI_TABLE_CHARGETEMP`) gives the fraction of CHT versus MAT used to compute the charge temperature(k) = (k(CHT-MAT)+MAT); and the charge temperature reference(k) = (k(CHTref-15)+15). The charge temperature and charge temperature reference are used to compute the charge temperature ratio (ctratio) in one of two ways. If CHT reference is nonzero ctratio = (ChargeTemp + 273.15)/(ChargeTemp_ref + 273.15); if CHT reference is zero ctratio = (ChargeTemp + 273.15)/(288.15). The charge temperature models induction heating to determine the effective temperature of air trapped in the combustion chamber. The default indices are [throttle, rpm].


## First fuel multiplier

The first fuel multiplier (`EFI_TABLE_FM1`) is a user configurable fuel multiplier. The default indices are [throttle, rpm].


## Injector 1 phase

The injector 1 phase table (`EFI_TABLE_INJECTOR1PHASE`) gives the crank angle in degrees for the start, middle, or end of the injector 1 output. The injector 1 phase table has rows indexed by throttle or load and columns indexed by RPM.


## Injector 1 trim

The injector 1 trim table (`EFI_TABLE_INJECTOR1TRIM`) gives the amount of time in microseconds to add to the pulse sent to injector 1 to account for the difference in injector opening versus closing time. The injector trim should be positive if the injector opens slower than it closes and vice versa. The default indices are [fuel pressure, voltage].


## Minimum throttle

The minimum throttle table (`EFI_TABLE_MINTHROTTLE`) gives the minimum throttle output in percent. This table is only useful if the EFI is driving the throttle. This table provides a way to adjust the idle speed of the engine. The default indices are [CHTcold, rpm].


## Maximum throttle

The maximum throttle table (`EFI_TABLE_MAXTHROTTLE`) gives the maximum throttle output in percent. This table is only useful if the EFI is driving the throttle. This table provides a way to limit the power output of the engine. The default indices are [CHThot, rpm]


## Spark retard

The spark retard table (`EFI_TABLE_SPARKRETARD`) gives the reduction of the ignition advance in degrees. The spark retard table has rows indexed by throttle or load and columns indexed by the hottest head temperature. The retard table affects the advance of the all sparks equally. This table provides a way to manage engine knock. The default indices are [throttle, CHThot]


## Second fuel multiplier

The second fuel multiplier (`EFI_TABLE_FM2`) is a user configurable fuel multiplier. The default indices are [throttle, rpm]


## Third fuel multiplier

The third fuel multiplier (`EFI_TABLE_FM3`) is a user configurable fuel multiplier. The default indices are [throttle, rpm]


## Manifold pressure estimate

The manifold pressure table (`EFI_TABLE_MAP`) gives a way to estimate the manifold pressure when the manifold pressure sensor is failed or disabled. The table gives the ratio of the manifold pressure to the barometric pressure. The default indices are [throttle, rpm]. Note: these are the only indices that make sense for this table.


## Shaft power estimate

The shaft power table (`EFI_TABLE_SHAFTPOWER`) provides an estimate of the shaft power, in Watts, on a standard dry day. The shaft power reported in the telemetry is computed from this table, using a mechanical efficiency estimate of 85% as well as the air density ratio, and the inverse of the charge temperature ratio. The default indices are [throttle, rpm].


## Injector 2 ratio

The injector 2 ratio table (`EFI_TABLE_INJECTOR2RATIO`) gives the percent of the fuel that should be output by injector 2. If the table value is 0, all the fuel is output by injectors 1 and/or 3. If the table value is 100%, all the fuel is output by injector 2. The default indices are [throttle, rpm].


## Injector 2 phase

The injector 2 phase table (`EFI_TABLE_INJECTOR2PHASE`) is the same as the injector 1 phase table, but the output is applied to injector 2.


## Injector 2 trim

The injector 2 trim table (`EFI_TABLE_INJECTOR2TRIM`) is the same as the injector 1 trim table, but the output is applied to injector 2.


## Fourth fuel multiplier

The fourth fuel multiplier (`EFI_TABLE_FM4`) is a user configurable fuel multiplier. The default indices are [throttle, rpm].


## Ignition dwell

The ignition dwell table (`EFI_TABLE_IGNDWELL`) gives the amount of time the ignition output is in the inactive (non-spark) state between spark events. The dwell time is used to compute the amount of time the spark should be active, by subtracting the dwell time from the crank period. If the computed active time is less than the minimum active time from the engine configuration, the dwell will be reduced to honor the minimum active time. If the table is disabled the ignition active time will be the minimum active time. The default indices are [throttle, rpm].


## First spark adder

The first spark adder table (`EFI_TABLE_SA1`) is a user configurable spark adder. The default indices are [throttle, rpm].


## Fifth fuel multiplier

The fifth fuel multiplier (`EFI_TABLE_FM5`) is a user configurable fuel multiplier. The default indices are [throttle, rpm].


## Injector 3 ratio

The injector 3 ratio table (`EFI_TABLE_INJECTOR3RATIO`) gives the percent of the fuel that should be output by injector 3. If the table value is 0, all the fuel is output by injectors 1 and/or 2. If the table value is 100%, all the fuel is output by injector 3. The default indices are [throttle, rpm].


## Injector 3 phase

The injector 3 phase table (`EFI_TABLE_INJECTOR3PHASE`) is the same as the injector 1 phase table, but the output is applied to injector 3.


## Injector 3 trim

The injector 3 trim table (`EFI_TABLE_INJECTOR3TRIM`) is the same as the injector 1 trim table, but the output is applied to injector 3.


## Spark 3 delay

The spark 3 delay table (`EFI_TABLE_SPARK3DELAY`) gives the amount of crank rotation in degrees after spark 1 output to trigger the spark 3 output. The spark 3 delay table has rows indexed by throttle or load and columns indexed by RPM.


## Second minimum throttle

The second minimum throttle table (`EFI_TABLE_MINTHROTTLE2`) gives a second minimum throttle output in percent. This table is only useful if the EFI is driving the throttle. The actual minimum throttle is the larger of this table and the first minimum throttle table. The default indices are [CHTcold, density_ratio].


## Second maximum throttle

The second maximum throttle table (`EFI_TABLE_MAXTHROTTLE2`) gives a second maximum throttle output in percent. This table is only useful if the EFI is driving the throttle. The actual maximum throttle is the smaller of this table and the first maximum throttle table. The default indices are [CHThot, density_ratio]


## Sixth fuel multiplier

The sixth fuel multiplier (`EFI_TABLE_FM6`) is a user configurable fuel multiplier. The default indices are [throttle, rpm].


## Second spark adder

The second spark adder table (`EFI_TABLE_SA2`) is a user configurable spark adder. The default indices are [throttle, rpm].


## Fuel to oil raio

The fuel to oil ratio table (`EFI_TABLE_OILINJ_RATIO`) gives the desired ratio of fuel to oil. The default indices are [throttle, rpm].


## Oil injection duty cycle

The oil injection duty cycle table (`EFI_TABLE_OILINJ_DUTY`) gives the desired duty cycle in percent of the oil injection pump. The default indices are [oil temp, voltage]. If no oil temperature sensor is configured the MAT is used instead. If this table is disabled the duty cycle is set to 50%.


## Oil injection output per pulse

The oil injection output per pulse table (`EFI_TABLE_OILINJ_OUTPUT`) gives the amount of oil injected, in micrograms, per pulse of the pump. The default indices are [oil temp, oil duty]. If this table is disabled the oil injection will not run.


## Third spark adder

The third spark adder table (`EFI_TABLE_SA3`) is a user configurable spark adder. The default indices are [throttle, rpm].


## User 1

The first user table (`EFI_TABLE_USER1`) is a user configurable table whose output can be used as input to any other table. If this table is disabled its output is 0.


## User 2

The second user table (`EFI_TABLE_USER2`) is a user configurable table whose output can be used as input to any other table. If this table is disabled its output is 0.


## User 3

The third user table (`EFI_TABLE_USER3`) is a user configurable table whose output can be used as input to any other table. If this table is disabled its output is 0.


## User 4

The fourth user table (`EFI_TABLE_USER4`) is a user configurable table whose output can be used as input to any other table. If this table is disabled its output is 0.


## <a name="EFI_PKT_TABLE"></a>2D Table Packet

This packet is used to change or request a 2D table. It should not be used when communicating with 1.x IntelliJect. Send this packet with just the first byte to request a specific table.

- packet identifier: `EFI_PKT_TABLE` : 9
- minimum data length: 13
- maximum data length: 1408

Enumerated options for how to do table lookups.

| Name                       | Value | Description                                                          |
| -------------------------- | :---: | -------------------------------------------------------------------- |
| `no_tableLookupOptions`    | 0     | Standard table lookup: no extrapolation, simple interpolation.       |
| `angle_tableLookup`        | 1     | Table lookup uses angle rules for interpolation and output wrapping. |
| `extrapolated_tableLookup` | 2     | Extrapolation allowed for table lookup.                              |
| `extrapangle_tableLookup`  | 3     | Table lookup uses angle rules and extrapolation.                     |
[<a name="tableLookupOptions"></a>tableLookupOptions enumeration]





| Bytes       | Name                                 | [Enc](#Enc)                           | Repeat                                                                                                         | Description                                                                                                                                                |
| ----------- | ------------------------------------ | :-----------------------------------: | :------------------------------------------------------------------------------------------------------------: | ---------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7         | 1)enabled                            | B1                                    | 1                                                                                                              | Table enable. If clear this table is not used.                                                                                                             |
| 0:6         | 2)reserved                           | B1                                    | 1                                                                                                              | Reserved bits in the packet.                                                                                                                               |
| 0:5...0:0   | 3)[index](#efiTableList)             | B6                                    | 1                                                                                                              | Enumeration specifying to which table this packet applies.<br>Value is limited on encode to be less than or equal to EFI_TABLE_MAXINDEX.                   |
| 1:7         | 4)version2                           | B1                                    | 1                                                                                                              | This bit must be set for a 2.x table.<br>Data are given constant value on encode 1.                                                                        |
| 1:6         | 5)addone                             | B1                                    | 1                                                                                                              | If set the result of the table interpolation is added to 1.0 after applying the multiplier and divider.                                                    |
| 1:5...1:0   | 6)reserved                           | B6                                    | 1                                                                                                              | Reserved bits in the packet.                                                                                                                               |
| 2:7...2:6   | 7)reserved                           | B2                                    | 1                                                                                                              | Reserved bits in the packet.                                                                                                                               |
| 2:5...2:0   | 8)[rowindex](#efiTableIndex)         | B6                                    | 1                                                                                                              | Enumeration identifying what variable is used for the row index.<br>Value is limited on encode to be less than or equal to EFI_TBLIDX_MAXINDEX.            |
| 3:7...3:6   | 9)reserved                           | B2                                    | 1                                                                                                              | Reserved bits in the packet.                                                                                                                               |
| 3:5...3:0   | 10)[colindex](#efiTableIndex)        | B6                                    | 1                                                                                                              | Enumeration identifying what variable is used for the column index.<br>Value is limited on encode to be less than or equal to EFI_TBLIDX_MAXINDEX.         |
| 4:7...4:6   | 11)reserved                          | B2                                    | 1                                                                                                              | Reserved bits in the packet.                                                                                                                               |
| 4:5...4:0   | 12)[multiplierindex](#efiTableIndex) | B6                                    | 1                                                                                                              | Enumeration identifying what variable is used to multiply the table output.<br>Value is limited on encode to be less than or equal to EFI_TBLIDX_MAXINDEX. |
| 5:7...5:6   | 13)reserved                          | B2                                    | 1                                                                                                              | Reserved bits in the packet.                                                                                                                               |
| 5:5...5:0   | 14)[dividerindex](#efiTableIndex)    | B6                                    | 1                                                                                                              | Enumeration identifying what variable is used to divide the table output.<br>Value is limited on encode to be less than or equal to EFI_TBLIDX_MAXINDEX.   |
| 6...33      | 15)description                       | Zero-terminated string up to 28 bytes                                                                                                                 || User definable name for the table.                                                                                                                         |
| 34...37     | 16)reserved                          | U32                                   | 1                                                                                                              | Reserved bytes in the packet.                                                                                                                              |
| 38:7...38:5 | 17)reserved                          | B3                                    | 1                                                                                                              | Reserved bits in the packet.                                                                                                                               |
| 38:4...38:0 | 18)numrows                           | B5                                    | 1                                                                                                              | Number of rows used in this table.<br>Value is limited on encode to be less than or equal to EFI_TABLE_MAX_ROWS.                                           |
| 39:7...39:5 | 19)reserved                          | B3                                    | 1                                                                                                              | Reserved bits in the packet.                                                                                                                               |
| 39:4...39:0 | 20)numcols                           | B5                                    | 1                                                                                                              | Number of columns used in this table.<br>Value is limited on encode to be less than or equal to EFI_TABLE_MAX_COLS.                                        |
| 40...102    | 21)rowindices                        | F24:15                                | numrows, up to [EFI_TABLE_MAX_ROWS](#efiTableSize)                                                             | The number of rows used in this table.                                                                                                                     |
| 103...147   | 22)colindices                        | F24:15                                | numcols, up to [EFI_TABLE_MAX_COLS](#efiTableSize)                                                             | The number of columns used in this table.                                                                                                                  |
| 148...1092  | 23)data                              | F24:15                                | numrows &times; numcols, up to [EFI_TABLE_MAX_ROWS](#efiTableSize) &times; [EFI_TABLE_MAX_COLS](#efiTableSize) | The table data organized as row major.                                                                                                                     |
| 1093...1407 | 24)meta                              | U8                                    | numrows &times; numcols, up to [EFI_TABLE_MAX_ROWS](#efiTableSize) &times; [EFI_TABLE_MAX_COLS](#efiTableSize) | The table meta data organized as row major.                                                                                                                |
[2D Table Packet packet bytes]


## <a name="EFI_PKT_TABLEVALUE"></a>Table Cell Value

Change a single table value and meta-value. Note that the table value packet incudes a 'do not store' flag. When calibrating an engine this packet is used frequently, and the do not store flag can be used to suppress the non-volatile storage update. IntelliJect will respond by sending the entire table back.

- packet identifier: `EFI_PKT_TABLEVALUE` : 10
- minimum data length: 7
- maximum data length: 10


| Bytes     | Name                     | [Enc](#Enc) | Repeat | Description                                                                                                                                            |
| --------- | ------------------------ | :---------: | :----: | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 0:7       | 1)donotstore             | B1          | 1      | If set the table in memory will be updated, but not stored to non-volatile storage.                                                                    |
| 0:6       | 2)reserved               | B1          | 1      | Reserved bits in the packet.                                                                                                                           |
| 0:5...0:0 | 3)[index](#efiTableList) | B6          | 1      | Enumeration specifying to which table this packet applies.                                                                                             |
| 1:7       | 4)version2               | B1          | 1      | This bit must be set for a 2.x table.<br>Data are given constant value on encode 1.                                                                    |
| 1:6...1:5 | 5)reserved               | B2          | 1      | Reserved bits in the packet.                                                                                                                           |
| 1:4...1:0 | 6)row                    | B5          | 1      | The row number, starting at zero.                                                                                                                      |
| 2:7...2:5 | 7)reserved               | B3          | 1      | Reserved bits in the packet.                                                                                                                           |
| 2:4...2:0 | 8)col                    | B5          | 1      | The column number, starting at zero.                                                                                                                   |
| 3         | 9)reserved               | U8          | 1      | Reserved bytes in the packet.                                                                                                                          |
| 4...5     | 10)value                 | F16:10      | 1      | The value of this table at [row, col].                                                                                                                 |
| 6         | 11)meta                  | U8          | 1      | Meta data for this table cell.                                                                                                                         |
| 7...9     | 12)value                 | F24:15      | 1      | The value of this table at [row, col] at higher resolution.<br>This field overrides the previous field of the same name, if the packet is long enough. |
[Table Cell Value packet bytes]


## <a name="EFI_PKT_TABLE"></a>Table Enable

Change the table enable and the row or column index variables and the math index variables without changing the entire table. This packet is the same as the Table2D packet, but it excludes the row and column indices and the table data. IntelliJect will respond by sending the entire table back.

- packet identifier: `EFI_PKT_TABLE` : 9
- data length: 6


| Bytes     | Name                                 | [Enc](#Enc) | Repeat | Description                                                                                             |
| --------- | ------------------------------------ | :---------: | :----: | ------------------------------------------------------------------------------------------------------- |
| 0:7       | 1)enabled                            | B1          | 1      | Table enable. If clear this table is not used.                                                          |
| 0:6       | 2)reserved                           | B1          | 1      | Reserved bits in the packet.                                                                            |
| 0:5...0:0 | 3)[index](#efiTableList)             | B6          | 1      | Enumeration specifying to which table this packet applies.                                              |
| 1:7       | 4)version2                           | B1          | 1      | This bit must be set for a 2.x table.<br>Data are given constant value on encode 1.                     |
| 1:6       | 5)addone                             | B1          | 1      | If set the result of the table interpolation is added to 1.0 after applying the multiplier and divider. |
| 1:5...1:0 | 6)reserved                           | B6          | 1      | Reserved bits in the packet.                                                                            |
| 2:7...2:6 | 7)reserved                           | B2          | 1      | Reserved bits in the packet.                                                                            |
| 2:5...2:0 | 8)[rowindex](#efiTableIndex)         | B6          | 1      | Enumeration identifying what variable is used for the row index.                                        |
| 3:7...3:6 | 9)reserved                           | B2          | 1      | Reserved bits in the packet.                                                                            |
| 3:5...3:0 | 10)[colindex](#efiTableIndex)        | B6          | 1      | Enumeration identifying what variable is used for the column index.                                     |
| 4:7...4:6 | 11)reserved                          | B2          | 1      | Reserved bits in the packet.                                                                            |
| 4:5...4:0 | 12)[multiplierindex](#efiTableIndex) | B6          | 1      | Enumeration identifying what variable is used to multiply the table output.                             |
| 5:7...5:6 | 13)reserved                          | B2          | 1      | Reserved bits in the packet.                                                                            |
| 5:5...5:0 | 14)[dividerindex](#efiTableIndex)    | B6          | 1      | Enumeration identifying what variable is used to divide the table output.                               |
[Table Enable packet bytes]

<div class="page-break"></div>


---


# Telemetry Packets output by IntelliJect

These packets are sent automatically to report the system status. Telemetry packets (except summary packets) are 8 bytes so they can be transported in a single CAN frame. Telemetry packets fall into one of two categories: fast packets and slow packets. Fast packets are sent quickly because they contain rapidly changing information such as throttle position, engine speed, and rev count. Slow packets include status and sensor information.

It is possible to send the telemetry in *summary packets*, see the [communications configuration](#EFI_PKT_COMMCONFIGURATION) for more details. When using summary packets the normally short telemetry packets are concatenated into longer packets (one summary packet for fast telemetry, and one for slow telemetry). This saves packet overhead bandwidth and is easier to process at the receiving end. Summary packets are not sent on the CAN bus, unless the communications configuration has the option `forcePacketOverCAN` set.


## <a name="EFI_PKT_TELEMETRYTIME"></a>Telemetry Time

Time information for telemetry. The telemetry time packet is always output immediately before the telemetry fast packet, and before the slow telemetry packets. If fast and slow telemetry are output on the same epoch then only a single time packet is output.

- packet identifier: `EFI_PKT_TELEMETRYTIME` : 18
- data length: 8


| Bytes     | Name       | [Enc](#Enc) | Repeat | Description                                                                                  |
| --------- | ---------- | :---------: | :----: | -------------------------------------------------------------------------------------------- |
| 0...3     | 1)time     | U32         | 1      | Time in milliseconds since the system booted up.                                             |
| 4:7       | 2)fasttime | B1          | 1      | If set this time packet applies to fast telemetry data that immediately follows this packet. |
| 4:6       | 3)slowtime | B1          | 1      | If set this time packet applies to slow telemetry data that follows this packet.             |
| 4:5...4:4 | 4)reserved | B2          | 1      | Reserved bits for future expansion.                                                          |
| 4:3...7:0 | 5)revcount | B28         | 1      | Number of engine revolutions since the engine was last started.                              |
[Telemetry Time packet bytes]


## Throttle Source

Source of the throttle information, for throttle command or throttle position sensor

| Name                     | Value | Description                                                                                   |
| ------------------------ | :---: | --------------------------------------------------------------------------------------------- |
| `EFI_THROTTLE_START`     | 0     | Throttle command comes form starting throttle setting; throttle position sense is unknown.    |
| `EFI_THROTTLE_CMD`       | 1     | Throttle command comes from user command; throttle position sense is assumed to match output. |
| `EFI_THROTTLE_PWMIN`     | 2     | Throttle (command or sense) comes from PWM input                                              |
| `EFI_THROTTLE_ANALOGIN`  | 3     | Throttle (command or sense) comes from analog input                                           |
| `EFI_THROTTLE_RPMDIRECT` | 4     | Throttle command comes from the RPM governor with a direct user command                       |
| `EFI_THROTTLE_MAP`       | 5     | Throttle position sense comes from manifold pressure estimate                                 |
| `EFI_THROTTLE_CAN`       | 6     | Throttle position sense comes from CAN servo                                                  |
| `NUM_EFI_THROTTLES`      | 7     |                                                                                               |
[<a name="efiThrottleSource"></a>Throttle Source enumeration]



## <a name="EFI_PKT_TELEMETRYFAST"></a>Fast Telemetry

Fast telemetry, output at the fast telemetry rate.

- packet identifier: `EFI_PKT_TELEMETRYFAST` : 19
- data length: 8


| Bytes     | Name                                   | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                     |
| --------- | -------------------------------------- | :---------: | :----: | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0...1     | 1)rpm                                  | U16         | 1      | Engine speed in revolutions per minute.<br>Scaled by 4 from 0.0 to 16383.75.                                                                                                                    |
| 2:7       | 2)ioEnable                             | B1          | 1      | Global enable based on physical input.                                                                                                                                                          |
| 2:6       | 3)userEnable                           | B1          | 1      | User global enable.                                                                                                                                                                             |
| 2:5       | 4)spark1Enable                         | B1          | 1      | User enable for spark1.                                                                                                                                                                         |
| 2:4       | 5)spark2Enable                         | B1          | 1      | User enable for spark2.                                                                                                                                                                         |
| 2:3       | 6)spark3Enable                         | B1          | 1      | User enable for spark3.                                                                                                                                                                         |
| 2:2...2:0 | 7)[throttleCmdSrc](#efiThrottleSource) | B3          | 1      | Source of the throttle command information. If the EFI is driving the throttle this is the command source. If the EFI is not driving the throttle this is the source of throttle position data. |
| 3         | 8)throttleCmd                          | U8          | 1      | Throttle command (0 to 100%) going in.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                        |
| 4         | 9)throttlePos                          | U8          | 1      | Throttle position (0 to 100%) this may be different from the throttleCmd if a curve is applied, or if the governor is interpreting the throttle command.<br>Scaled by 2 from 0.0 to 127.5.      |
| 5         | 10)injector3Duty                       | U8          | 1      | The third injector duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                     |
| 6         | 11)injector1Duty                       | U8          | 1      | The first injector duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                     |
| 7         | 12)injector2Duty                       | U8          | 1      | The second injector duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                    |
[Fast Telemetry packet bytes]


## <a name="EFI_PKT_TELEMETRYFASTSUM"></a>Fast Telemetry Summary Packet

Summary packet that contains all the fast telemetry in a single packet.

- packet identifier: `EFI_PKT_TELEMETRYFASTSUM` : 35
- data length: 16


| Bytes       | Name                                     | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                     |
| ----------- | ---------------------------------------- | :---------: | :----: | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0...7       | 1)time                                                || 1      | Time information for the fast telemetry                                                                                                                                                         |
| 0...3       | 1.1)time                                 | U32         | 1      | Time in milliseconds since the system booted up.                                                                                                                                                |
| 4:7         | 1.2)fasttime                             | B1          | 1      | If set this time packet applies to fast telemetry data that immediately follows this packet.                                                                                                    |
| 4:6         | 1.3)slowtime                             | B1          | 1      | If set this time packet applies to slow telemetry data that follows this packet.                                                                                                                |
| 4:5...4:4   | 1.4)reserved                             | B2          | 1      | Reserved bits for future expansion.                                                                                                                                                             |
| 4:3...7:0   | 1.5)revcount                             | B28         | 1      | Number of engine revolutions since the engine was last started.                                                                                                                                 |
| 8...15      | 2)fast                                                || 1      | Fast EFI telemetry                                                                                                                                                                              |
| 8...9       | 2.1)rpm                                  | U16         | 1      | Engine speed in revolutions per minute.<br>Scaled by 4 from 0.0 to 16383.75.                                                                                                                    |
| 10:7        | 2.2)ioEnable                             | B1          | 1      | Global enable based on physical input.                                                                                                                                                          |
| 10:6        | 2.3)userEnable                           | B1          | 1      | User global enable.                                                                                                                                                                             |
| 10:5        | 2.4)spark1Enable                         | B1          | 1      | User enable for spark1.                                                                                                                                                                         |
| 10:4        | 2.5)spark2Enable                         | B1          | 1      | User enable for spark2.                                                                                                                                                                         |
| 10:3        | 2.6)spark3Enable                         | B1          | 1      | User enable for spark3.                                                                                                                                                                         |
| 10:2...10:0 | 2.7)[throttleCmdSrc](#efiThrottleSource) | B3          | 1      | Source of the throttle command information. If the EFI is driving the throttle this is the command source. If the EFI is not driving the throttle this is the source of throttle position data. |
| 11          | 2.8)throttleCmd                          | U8          | 1      | Throttle command (0 to 100%) going in.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                        |
| 12          | 2.9)throttlePos                          | U8          | 1      | Throttle position (0 to 100%) this may be different from the throttleCmd if a curve is applied, or if the governor is interpreting the throttle command.<br>Scaled by 2 from 0.0 to 127.5.      |
| 13          | 2.10)injector3Duty                       | U8          | 1      | The third injector duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                     |
| 14          | 2.11)injector1Duty                       | U8          | 1      | The first injector duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                     |
| 15          | 2.12)injector2Duty                       | U8          | 1      | The second injector duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                    |
[Fast Telemetry Summary Packet packet bytes]


## <a name="EFI_PKT_TELEMETRYSENSORS"></a>Telemetry Sensors

First set of sensor telemetry, output at slow telemetry rate.

- packet identifier: `EFI_PKT_TELEMETRYSENSORS` : 20
- data length: 8


| Bytes     | Name                                   | [Enc](#Enc) | Repeat | Description                                                                                                                                                             |
| --------- | -------------------------------------- | :---------: | :----: | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0         | 1)_cht                                 | U8          | 1      | Combined cylinder head temperature in C, used for table lookups. This is a deprecated field, prefer the field in Telemetry Sensors 4.<br>Scaled by 1 from 0.0 to 255.0. |
| 1:7...1:5 | 2)[throttlePosSrc](#efiThrottleSource) | B3          | 1      | Source of throttle position information used for table lookups.                                                                                                         |
| 1:4       | 3)spareTempIsCHT3                      | B1          | 1      | This bit is set if spare temperature is configured to act as the third CHT.                                                                                             |
| 1:3       | 4)spareTempIsMAT2                      | B1          | 1      | This bit is set if spare temperature is configured to act as the second MAT.                                                                                            |
| 1:2       | 5)spareTempIsOilT                      | B1          | 1      | This bit is set if spare temperature is configured to act as oil temperature.                                                                                           |
| 1:1...2:0 | 6)sparetemp                            | B10         | 1      | Spare temperature in C.<br>Scaled by 2 from -100.0 to 411.5.                                                                                                            |
| 3         | 7)mat                                  | I8          | 1      | Manifold air temperature in C.<br>Scaled by 1 from -127.0 to 127.0.                                                                                                     |
| 4:7...5:4 | 8)map                                  | B12         | 1      | Manifold air pressure in kiloPascals.<br>Scaled by 31.25 from 0.0 to 131.04.                                                                                            |
| 5:3...6:0 | 9)baro                                 | B12         | 1      | Baro air pressure in kiloPascals.<br>Scaled by 31.25 from 0.0 to 131.04.                                                                                                |
| 7         | 10)oat                                 | I8          | 1      | Outside air temperature (as measured by the digital barometer) in C.<br>Scaled by 1 from -127.0 to 127.0.                                                               |
[Telemetry Sensors packet bytes]


## <a name="EFI_PKT_TELEMETRYSENSORS2"></a>Telemetry Sensors 2

Second set of sensor telemetry, output at slow telemetry rate. The crank sense active signals will be set if the crank sense input is in the active state when this message is generated. This is useful only for measuring the crank sense timing, but not for determining anything when the engine is running.

- packet identifier: `EFI_PKT_TELEMETRYSENSORS2` : 21
- data length: 8


| Bytes     | Name                  | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                           |
| --------- | --------------------- | :---------: | :----: | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7...1:4 | 1)measuredCrank2Angle | B12         | 1      | Measured synchronization angle of crank sense 2, as determined by crank sense 1.<br>Scaled by 10 from 0.0 to 409.5.                                                                                                                                   |
| 1:3...1:0 | 2)devCrank2Angle      | B4          | 1      | Standard deviation of the measured synchronization angle of crank sense 2, as determined by crank sense 1.<br>Scaled by 2 from 0.0 to 7.5.                                                                                                            |
| 2:7...3:4 | 3)_density            | B12         | 1      | Estimated air density in kilogram per cubic meter, deprecated in api 10.<br>Scaled by 4095/(3.0) from 0.0 to 3.0.                                                                                                                                     |
| 3:3       | 4)expectSensors4      | B1          | 1      | If this bit is set the receiver can expect that sensors4 data will be sent from the EFI, otherwise the EFI is older and the deprecated fields in TelemetrySensors and TelemetrySensors2 should be used.<br>Data are given constant value on encode 1. |
| 3:2       | 5)api8                | B1          | 1      | Set if this packet comes from API 8 or later.<br>Data are given constant value on encode 1.                                                                                                                                                           |
| 3:1       | 6)crankSense1Active   | B1          | 1      | Set if the first crank sense signal is in the active state.                                                                                                                                                                                           |
| 3:0       | 7)crankSense2Active   | B1          | 1      | Set if the second crank sense signal is in the active state.                                                                                                                                                                                          |
| 4         | 8)cooling2            | U8          | 1      | Second cooling output in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                   |
| 5         | 9)tpsError            | I8          | 1      | Error in percent between throttle output and TPS measurement.<br>Scaled by 10 from -12.7 to 12.7.                                                                                                                                                     |
| 6         | 10)analogTPS          | U8          | 1      | Analog throttle position sensor in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                         |
| 7         | 11)pwmTPS             | U8          | 1      | PWM throttle position sensor in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                            |
[Telemetry Sensors 2 packet bytes]


## <a name="EFI_PKT_TELEMETRYSENSORS3"></a>Telemetry Sensors 3

Third set of sensor telemetry, output at slow telemetry rate.

- packet identifier: `EFI_PKT_TELEMETRYSENSORS3` : 38
- data length: 8


| Bytes     | Name           | [Enc](#Enc) | Repeat | Description                                                                                         |
| --------- | -------------- | :---------: | :----: | --------------------------------------------------------------------------------------------------- |
| 0:7...1:7 | 1)inputVolts   | B9          | 1      | Input voltage in Volts.<br>Scaled by 10 from 0.0 to 51.1.                                           |
| 1:6...1:0 | 2)inputCurrent | B7          | 1      | Input current in Amps.<br>Scaled by 40 from 0.0 to 3.175.                                           |
| 2         | 3)volts12      | U8          | 1      | 12V rail voltage in Volts.<br>Scaled by 25 from 7.0 to 17.2.                                        |
| 3         | 4)current12    | U8          | 1      | 12V rail current in Amps.<br>Scaled by 40 from 0.0 to 6.375.                                        |
| 4:7...5:4 | 5)baroDigital  | B12         | 1      | Baro air pressure in kiloPascals from the digital barometer.<br>Scaled by 31.25 from 0.0 to 131.04. |
| 5:3...6:0 | 6)baroAnalog   | B12         | 1      | Baro air pressure in kiloPascals from the analog barometer.<br>Scaled by 31.25 from 0.0 to 131.04.  |
| 7         | 7)reserved     | U8          | 1      | Reserved bytes in the packet.                                                                       |
[Telemetry Sensors 3 packet bytes]


## <a name="EFI_PKT_TELEMETRYSENSORS4"></a>Telemetry Sensors 4

Fourth set of sensor telemetry, output at slow telemetry rate.

- packet identifier: `EFI_PKT_TELEMETRYSENSORS4` : 48
- data length: 8


| Bytes     | Name                                   | [Enc](#Enc) | Repeat | Description                                                                                                                                                   |
| --------- | -------------------------------------- | :---------: | :----: | ------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7...1:6 | 1)cht                                  | B10         | 1      | Average cylinder head temperature in C.<br>Scaled by 2 from -100.0 to 411.5.                                                                                  |
| 1:5...2:4 | 2)cht1                                 | B10         | 1      | First cylinder head temperature in C.<br>Scaled by 2 from -100.0 to 411.5.                                                                                    |
| 2:3...3:2 | 3)cht2                                 | B10         | 1      | Second cylinder head temperature in C.<br>Scaled by 2 from -100.0 to 411.5.                                                                                   |
| 3:1...3:0 | 4)[crankdirection](#efiCrankDirection) | B2          | 1      | The direction of rotation measured by the crank wheel(s) or by the once-per-rev sensors.                                                                      |
| 4         | 5)chargeTempCoef                       | U8          | 1      | Charge temperature coefficient from 0 to 1 that determines how much CHT is used in the computation of charge temperature.<br>Scaled by 200 from 0.0 to 1.275. |
| 5         | 6)_fmHeadTemp2                         | U8          | 1      | Deprecated in API 10, do not use.<br>Scaled by 100 from 0.0 to 2.55.<br>Data are given constant value on encode 1.                                            |
| 6...7     | 7)fuelp                                | F16:10      | 1      | Fuel pressure in kiloPascals.                                                                                                                                 |
[Telemetry Sensors 4 packet bytes]


## <a name="EFI_PKT_TELEMETRYFUEL"></a>Telemetry Fuel

Fueling telemetry, output at the slow telemetry rate

- packet identifier: `EFI_PKT_TELEMETRYFUEL` : 22
- data length: 8


| Bytes | Name              | [Enc](#Enc) | Repeat | Description                                                                                                |
| ----- | ----------------- | :---------: | :----: | ---------------------------------------------------------------------------------------------------------- |
| 0...1 | 1)fuelFlowRate    | F16:10      | 1      | Fuel flow rate in grams per minute.                                                                        |
| 2...3 | 2)fuelConsumption | F16:10      | 1      | Fuel consumption in grams since the system turned on.                                                      |
| 4     | 3)fmInjector1     | U8          | 1      | Total fuel multiplier for injector 1, including the main fuel multiplier.<br>Scaled by 50 from 0.0 to 5.1. |
| 5     | 4)fmInjector2     | U8          | 1      | Total fuel multiplier for injector 2, including the main fuel multiplier.<br>Scaled by 50 from 0.0 to 5.1. |
| 6     | 5)fmInjector3     | U8          | 1      | Total fuel multiplier for injector 3, including the main fuel multiplier.<br>Scaled by 50 from 0.0 to 5.1. |
| 7     | 6)reserved        | U8          | 1      | Reserved bytes in the packet.                                                                              |
[Telemetry Fuel packet bytes]


## <a name="EFI_PKT_TELEMETRYINJECTOR"></a>Telemetry Injector

Operational details for injectors 1 and 2

- packet identifier: `EFI_PKT_TELEMETRYINJECTOR` : 25
- data length: 8


| Bytes     | Name                  | [Enc](#Enc) | Repeat | Description                                                                                                                                         |
| --------- | --------------------- | :---------: | :----: | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7       | 1)reserved            | B1          | 1      | Reserved bits in the packet.                                                                                                                        |
| 0:6       | 2)injector1usesense1  | B1          | 1      | Set if injector 1 is being triggered by sense 1.                                                                                                    |
| 0:5       | 3)injector1usesense2  | B1          | 1      | Set if injector 1 is being triggered by sense 2.                                                                                                    |
| 0:4...2:4 | 4)injector1Time       | B17         | 1      | First injector on time for each engine cycle in microseconds. This does not include the injector trim time.<br>Scaled by 1.0 from 0.0 to 131071.0.  |
| 2:3...3:2 | 5)injection1Angle     | B10         | 1      | Crank angle of the first injection.<br>Scaled by 2 from 0.0 to 511.5.                                                                               |
| 3:1...3:0 | 6)reserved            | B2          | 1      | Reserved bits in the packet.                                                                                                                        |
| 4:7       | 7)reserved            | B1          | 1      | Reserved bits in the packet.                                                                                                                        |
| 4:6       | 8)injector2usesense1  | B1          | 1      | Set if injector 2 is being triggered by sense 1.                                                                                                    |
| 4:5       | 9)injector2usesense2  | B1          | 1      | Set if injector 2 is being triggered by sense 2.                                                                                                    |
| 4:4...6:4 | 10)injector2Time      | B17         | 1      | Second injector on time for each engine cycle in microseconds. This does not include the injector trim time.<br>Scaled by 1.0 from 0.0 to 131071.0. |
| 6:3...7:2 | 11)injection2Angle    | B10         | 1      | Crank angle of the second injection.<br>Scaled by 2 from 0.0 to 511.5.                                                                              |
| 7:1       | 12)injector3usesense1 | B1          | 1      | Set if injector 3 is being triggered by sense 1.                                                                                                    |
| 7:0       | 13)injector3usesense2 | B1          | 1      | Set if injector 3 is being triggered by sense 2.                                                                                                    |
[Telemetry Injector packet bytes]


## <a name="EFI_PKT_TELEMETRYEXTENDEDOUTPUTS"></a>Telemetry Extended Outputs

Operational details for the third injector and second and third spark

- packet identifier: `EFI_PKT_TELEMETRYEXTENDEDOUTPUTS` : 59
- data length: 8


| Bytes     | Name              | [Enc](#Enc) | Repeat | Description                                                                                                                                        |
| --------- | ----------------- | :---------: | :----: | -------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7...1:6 | 1)sparkAdvance2   | B10         | 1      | Spark advance in degrees for the second spark output.<br>Scaled by 2 from 0.0 to 511.5.                                                            |
| 1:5...2:4 | 2)sparkAdvance3   | B10         | 1      | Spark advance in degrees for the third spark output.<br>Scaled by 2 from 0.0 to 511.5.                                                             |
| 2:3...3:2 | 3)injection3Angle | B10         | 1      | Crank angle of the third injection.<br>Scaled by 2 from 0.0 to 511.5.                                                                              |
| 3:1       | 4)reserved        | B1          | 1      | Reserved bits in the packet.                                                                                                                       |
| 3:0...5:0 | 5)injector3Time   | B17         | 1      | third injector on time for each engine cycle in microseconds. This does not include the injector trim time.<br>Scaled by 1.0 from 0.0 to 131071.0. |
| 6         | 6)_fmChargeTemp3  | U8          | 1      | Deprecated in API 10, do not use.<br>Scaled by 100 from 0.0 to 2.55.<br>Data are given constant value on encode 1.                                 |
| 7         | 7)_fmHeadTemp3    | U8          | 1      | Deprecated in API 10, do not use.<br>Scaled by 100 from 0.0 to 2.55.<br>Data are given constant value on encode 1.                                 |
[Telemetry Extended Outputs packet bytes]


## <a name="EFI_PKT_TELEMETRYSLOW"></a>Telemetry Slow

Miscellanious telemetry, output at the slow telemetry rate. For any slow output epoch this is always the last packet. Therefore you should trigger display of slow packet data on receipt of this packet (or the slow summary packet).

- packet identifier: `EFI_PKT_TELEMETRYSLOW` : 23
- data length: 8


| Bytes     | Name                         | [Enc](#Enc) | Repeat | Description                                                                                                    |
| --------- | ---------------------------- | :---------: | :----: | -------------------------------------------------------------------------------------------------------------- |
| 0         | 1)sparkAdvance1              | I8          | 1      | Spark advance in degrees for the first spark output.<br>Scaled by 2 from -63.5 to 63.5.                        |
| 1:7...1:6 | 2)reserved                   | B2          | 1      | Reserved bits in the packet.                                                                                   |
| 1:5       | 3)spark1usesense1            | B1          | 1      | Set if spark 1 is being triggered by sense 1.                                                                  |
| 1:4       | 4)spark1usesense2            | B1          | 1      | Set if spark 1 is being triggered by sense 2.                                                                  |
| 1:3       | 5)spark2usesense1            | B1          | 1      | Set if spark 2 is being triggered by sense 1.                                                                  |
| 1:2       | 6)spark2usesense2            | B1          | 1      | Set if spark 2 is being triggered by sense 2.                                                                  |
| 1:1       | 7)spark3usesense1            | B1          | 1      | Set if spark 3 is being triggered by sense 1.                                                                  |
| 1:0       | 8)spark3usesense2            | B1          | 1      | Set if spark 3 is being triggered by sense 2.                                                                  |
| 2...3     | 9)power                      | F16:10      | 1      | Estimated engine shaft power in Watts.                                                                         |
| 4:7       | 10)rpmControllerFromUser     | B1          | 1      | Set if the RPM controller is running and gettings its command from the user.                                   |
| 4:6       | 11)rpmControllerFromThrottle | B1          | 1      | Set if the RPM controller is running and getting its command from the throttle input, or the throttle limiter. |
| 4:5...5:0 | 12)rpmcmd                    | B14         | 1      | Engine speed command in revolutions per minute.<br>Scaled by 1 from 0.0 to 16383.0.                            |
| 6         | 13)pumpduty                  | U8          | 1      | Fuel pump duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                             |
| 7         | 14)cooling1                  | U8          | 1      | First cooling output in percent.<br>Scaled by 2 from 0.0 to 127.5.                                             |
[Telemetry Slow packet bytes]


## <a name="EFI_PKT_TELEMETRYSLOW"></a>Telemetry Slow (api=7, deprecated)

Miscellanious telemetry, output at the slow telemetry rate. For any slow output epoch this is always the last packet. Therefore you should trigger display of slow packet data on receipt of this packet (or the slow summary packet).

- packet identifier: `EFI_PKT_TELEMETRYSLOW` : 23
- data length: 8


| Bytes     | Name                        | [Enc](#Enc) | Repeat | Description                                                                                                    |
| --------- | --------------------------- | :---------: | :----: | -------------------------------------------------------------------------------------------------------------- |
| 0         | 1)sparkAdvance1             | I8          | 1      | Spark advance in degrees for the first spark output.<br>Scaled by 2 from -63.5 to 63.5.                        |
| 1         | 2)_sparkAdvance2            | I8          | 1      | Spark advance in degrees for the second spark output.<br>Scaled by 2 from -63.5 to 63.5.                       |
| 2...3     | 3)power                     | F16:10      | 1      | Estimated engine shaft power in Watts.                                                                         |
| 4:7       | 4)rpmControllerFromUser     | B1          | 1      | Set if the RPM controller is running and gettings its command from the user.                                   |
| 4:6       | 5)rpmControllerFromThrottle | B1          | 1      | Set if the RPM controller is running and getting its command from the throttle input, or the throttle limiter. |
| 4:5...5:0 | 6)rpmcmd                    | B14         | 1      | Engine speed command in revolutions per minute.<br>Scaled by 1 from 0.0 to 16383.0.                            |
| 6         | 7)pumpduty                  | U8          | 1      | Fuel pump duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                             |
| 7         | 8)cooling1                  | U8          | 1      | First cooling output in percent.<br>Scaled by 2 from 0.0 to 127.5.                                             |
[Telemetry Slow (api=7, deprecated) packet bytes]


## <a name="EFI_PKT_TELEMETRYCPU"></a>Telemetry CPU

Run time system information including CPU loading

- packet identifier: `EFI_PKT_TELEMETRYCPU` : 24
- data length: 8


| Bytes     | Name               | [Enc](#Enc) | Repeat | Description                                                                                                                                                                               |
| --------- | ------------------ | :---------: | :----: | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0         | 1)cpuLoad          | U8          | 1      | Total cpu loading in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                           |
| 1         | 2)intLoad          | U8          | 1      | Percentage of CPU time spent in interrupts.<br>Scaled by 10 from 0.0 to 25.5.                                                                                                             |
| 2         | 3)stack            | U8          | 1      | Percentage of available stack space that has been used.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                 |
| 3         | 4)cpuTemp          | I8          | 1      | CPU temperature in C.<br>Scaled by 1 from -127.0 to 127.0.                                                                                                                                |
| 4:7       | 5)safeModeActive   | B1          | 1      | If set the system booted in safe mode due to too many resets. No settings data were loaded from non-volatile storage in this mode. The only way to exit safe mode is to reset the system. |
| 4:6       | 6)passwordRequired | B1          | 1      | If set the EFI requires a password to be unlocked.                                                                                                                                        |
| 4:5...4:4 | 7)unlockLevel      | B2          | 1      | EFI unlock level. 0 indicates the EFI is locked, '1' indicates the EFI is partially unlocked, and '2' indicates it is password unlocked.                                                  |
| 4:3       | 8)sdcardPresent    | B1          | 1      | If set an SD card is detected and configured for data recording, and the SD card telemetry packet should be expected.                                                                     |
| 4:2       | 9)gcuPresent       | B1          | 1      | If set a GCU (generator control unit) is detected on the CNA bus and the GCU telemetry packet should be expected.                                                                         |
| 4:1...4:0 | 10)reserved        | B2          | 1      | reserved bits.                                                                                                                                                                            |
| 5         | 11)watchdog        | U8          | 1      | Maximum interval of time, in microseconds, between watchdog service events.<br>Scaled by 0.1 from 0.0 to 2550.0.                                                                          |
| 6:7...6:5 | 12)reserved        | B3          | 1      | reserved bits.                                                                                                                                                                            |
| 6:4...6:0 | 13)api             | B5          | 1      | Application programming interface number. Changes to the ICD will increment this number.<br>Data are given constant value on encode geteficommsApi().                                     |
| 7         | 14)interrupt       | U8          | 1      | Maximum amount time spent in an interrupt, or with interrupts blocked, in microseconds.                                                                                                   |
[Telemetry CPU packet bytes]


## Dynamic and Sticky Errors

Error information, output at the slow telemetry rate. There are two types of error information: dynamic errors (reported by `EFI_PKT_TELEMETRYERRORS`) and sticky errors (reported by `EFI_PKT_STICKY_ERRORS`). Dynamic errors indicate the current error condition, and will automatically clear when the error condition is resolved. Sticky errors are only cleared by sending `EFI_PKT_STICKY_ERRORS` with bits cleared where desired.

This packet supports multiple identifiers.

- packet identifier: <a name="EFI_PKT_STICKY_ERRORS"></a>`EFI_PKT_STICKY_ERRORS` : 13
- packet identifier: <a name="EFI_PKT_TELEMETRYERRORS"></a>`EFI_PKT_TELEMETRYERRORS` : 26
- data length: 8

Enumerated list of errors that can be reported by the crank sensor

| Name                       | Value | Description                                                                                                          |
| -------------------------- | :---: | -------------------------------------------------------------------------------------------------------------------- |
| `EFI_CRANK_NOERROR`        | 0     | No errors in crank sense                                                                                             |
| `EFI_CRANK_STARTSYNCERROR` | 1     | Crank wheel synchronization error during start. This could be a transient                                            |
| `EFI_CRANK_MISSEDEDGE`     | 2     | Crank sense missed an edge (i.e. two edges of the same type (rising or falling) in a row)                            |
| `EFI_CRANK_OVERCAPTURE`    | 3     | Crank sense hardware registered an overcapture, which occurs when software does not service the hardware fast enough |
| `EFI_CRANK_TOOFAST`        | 4     | Crank sense active edge was earlier than expected                                                                    |
| `EFI_CRANK_RUNSYNCERROR`   | 5     | Crank wheel synchronization was lost after the engine was running                                                    |
| `EFI_CRANK_TOOLATE`        | 6     | Crank sense active edge was later than expected                                                                      |
| `EFI_CRANK_REVERSED`       | 7     | Crank is rotating in a direction that is not allowed                                                                 |
[<a name="efiCrankError"></a>efiCrankError enumeration]





| Bytes     | Name                        | [Enc](#Enc) | Repeat | Description                                                                                                                                                                             |
| --------- | --------------------------- | :---------: | :----: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7       | 1)inputvoltagelow           | B1          | 1      | Set if input voltage is too low.                                                                                                                                                        |
| 0:6       | 2)inputvoltagehigh          | B1          | 1      | Set if input voltage is too high.                                                                                                                                                       |
| 0:5       | 3)matsensorunderrng         | B1          | 1      | Set if manifold air temperature sensor is out of range low.                                                                                                                             |
| 0:4       | 4)matsensoroverrng          | B1          | 1      | Set if manifold air temperature sensor is out of range high.                                                                                                                            |
| 0:3       | 5)cht1sensorunderrng        | B1          | 1      | Set if first cylinder head temperature sensor is out of range low.                                                                                                                      |
| 0:2       | 6)cht1sensoroverrng         | B1          | 1      | Set if first cylinder head temperature sensor is out of range high.                                                                                                                     |
| 0:1       | 7)cht2sensorunderrng        | B1          | 1      | Set if second cylinder head temperature sensor is out of range low.                                                                                                                     |
| 0:0       | 8)cht2sensoroverrng         | B1          | 1      | Set if second cylinder head temperature sensor is out of range high.                                                                                                                    |
| 1:7       | 9)mapsensorunderrng         | B1          | 1      | Set if manifold air pressure sensor is out of range low.                                                                                                                                |
| 1:6       | 10)mapsensoroverrng         | B1          | 1      | Set if manifold air pressure sensor is out of range high.                                                                                                                               |
| 1:5       | 11)fuelpsensorunderrng      | B1          | 1      | Set if fuel pressure sensor is out of range low.                                                                                                                                        |
| 1:4       | 12)fuelpsensoroverrng       | B1          | 1      | Set if fuel pressure sensor is out of range high.                                                                                                                                       |
| 1:3       | 13)analogtpssensorunderrng  | B1          | 1      | Set if analog TPS sensor is out of range low.                                                                                                                                           |
| 1:2       | 14)analogtpssensoroverrng   | B1          | 1      | Set if analog TPS is out of range high.                                                                                                                                                 |
| 1:1       | 15)cputemplow               | B1          | 1      | Set if CPU temperature is too low.                                                                                                                                                      |
| 1:0       | 16)cputemphigh              | B1          | 1      | Set if CPU temperature is too high.                                                                                                                                                     |
| 2:7       | 17)sparetempsensorunderrng  | B1          | 1      | Set if spare temperature sensor is out of range low.                                                                                                                                    |
| 2:6       | 18)sparetempsensoroverrng   | B1          | 1      | Set if spare temperature sensor is out of range high.                                                                                                                                   |
| 2:5       | 19)barosensorunderrng       | B1          | 1      | Set if barometric air pressure sensor is out of range low.                                                                                                                              |
| 2:4       | 20)barosensoroverrng        | B1          | 1      | Set if barometric air pressure sensor is out of range high.                                                                                                                             |
| 2:3...2:1 | 21)crank1error              | B3          | 1      | Enumerated errors from crank sense1, clear if no errors.                                                                                                                                |
| 2:0...3:6 | 22)crank2error              | B3          | 1      | Enumerated errors from crank sense2, clear if no errors.                                                                                                                                |
| 3:5       | 23)cht1low                  | B1          | 1      | Set if the first cylinder head temperature is too low, the head is overcooled.                                                                                                          |
| 3:4       | 24)cht1high                 | B1          | 1      | Set if the first cylinder head temperature is too high, the head is overheating.                                                                                                        |
| 3:3       | 25)cht2low                  | B1          | 1      | Set if the second cylinder head temperature is too low, the head is overcooled.                                                                                                         |
| 3:2       | 26)cht2high                 | B1          | 1      | Set if the second cylinder head temperature is too high, the head is overheating.                                                                                                       |
| 3:1       | 27)fuelplow                 | B1          | 1      | Set if the fuel pressure is too low.                                                                                                                                                    |
| 3:0       | 28)fuelphigh                | B1          | 1      | Set if the fuel pressure is too high.                                                                                                                                                   |
| 4:7       | 29)hardoverspeed            | B1          | 1      | Set if the engine RPM exceeded the hard rev limit.                                                                                                                                      |
| 4:6       | 30)softoverspeed            | B1          | 1      | Set if the engine RPM exceeded the soft rev limit.                                                                                                                                      |
| 4:5       | 31)parameterhash            | B1          | 1      | Set if the nonvolatile parameters did not hash to the expected value.                                                                                                                   |
| 4:4       | 32)voltage12low             | B1          | 1      | Set if 12V voltage is too low.                                                                                                                                                          |
| 4:3       | 33)voltage12high            | B1          | 1      | Set if 12V voltage is too high.                                                                                                                                                         |
| 4:2       | 34)inputcurrentlow          | B1          | 1      | Set if the input current is too low.                                                                                                                                                    |
| 4:1       | 35)inputcurrenthigh         | B1          | 1      | Set if the input current is too high.                                                                                                                                                   |
| 4:0       | 36)current12low             | B1          | 1      | Set if the current of the 12 volt rail is too low.                                                                                                                                      |
| 5:7       | 37)current12high            | B1          | 1      | Set if the current of the 12 volt rail too high.                                                                                                                                        |
| 5:6       | 38)power6fault              | B1          | 1      | Set if the 6 volt power supply is faulted.                                                                                                                                              |
| 5:5       | 39)injector1fault           | B1          | 1      | Set if the first injector has a fault.                                                                                                                                                  |
| 5:4       | 40)injector2fault           | B1          | 1      | Set if the second injector has a fault.                                                                                                                                                 |
| 5:3       | 41)userStorageError         | B1          | 1      | Set if there is a user storage error.                                                                                                                                                   |
| 5:2       | 42)analogbarosensorunderrng | B1          | 1      | Set if the analog barometric air pressure sensor is out of range low.                                                                                                                   |
| 5:1       | 43)analogbarosensoroverrng  | B1          | 1      | Set if the analog barometric air pressure sensor is out of range high.                                                                                                                  |
| 5:0       | 44)oatsensorunderrng        | B1          | 1      | Set if barometric air pressure sensor is out of range low.                                                                                                                              |
| 6:7       | 45)oatsensoroverrng         | B1          | 1      | Set if barometric air pressure sensor is out of range high.                                                                                                                             |
| 6:6       | 46)canThrottleMissing       | B1          | 1      | Set if the throttle is configured to use a CAN servo, but the CAN servo has not been detected on the bus.                                                                               |
| 6:5       | 47)canThrottleError         | B1          | 1      | Set if the CAN throttle servo has an error bit set.                                                                                                                                     |
| 6:4       | 48)canCooling1Missing       | B1          | 1      | Set if the cooling output 1 is configured to use a CAN servo, but the CAN servo has not been detected on the bus.                                                                       |
| 6:3       | 49)canCooling1Error         | B1          | 1      | Set if the CAN cooling 1 servo has an error bit set.                                                                                                                                    |
| 6:2       | 50)maxstartthrottle         | B1          | 1      | Set if throttle was greater than the maximum start throttle while the engine was cranking for start. In that case spark and injector outputs are suspended.                             |
| 6:1       | 51)maplow                   | B1          | 1      | Set if manifold air pressure sensor is low compared to the estimate.                                                                                                                    |
| 6:0       | 52)maphigh                  | B1          | 1      | Set if manifold air pressure sensor is high compared to the estimate.                                                                                                                   |
| 7:7       | 53)tpsmissing               | B1          | 1      | Set if there is no throttle position source, but the system is configured for one.                                                                                                      |
| 7:6       | 54)tpserror                 | B1          | 1      | Set if there is a problem with the throttle position sensor: either it is invalid (none connected to analog or PWM), or it disagrees with the throttle output (if efi drives throttle). |
| 7:5       | 55)sdcarderror              | B1          | 1      | Set if an error occurred with the SD card recording. This error will only assert after the SD card has been successfully detected.                                                      |
| 7:4       | 56)reserved1                | B1          | 1      | Reserved for future expansion.                                                                                                                                                          |
| 7:3       | 57)canCooling2Missing       | B1          | 1      | Set if the cooling output 2 is configured to use a CAN servo, but the CAN servo has not been detected on the bus.                                                                       |
| 7:2       | 58)canCooling2Error         | B1          | 1      | Set if the CAN cooling 2 servo has an error bit set.                                                                                                                                    |
| 7:1       | 59)maintenanceNeeded        | B1          | 1      | If set a maintenance item has been triggered, see the maintenance packet for details.                                                                                                   |
| 7:0       | 60)testModeActive           | B1          | 1      | If set the EFI is in test mode or is autocalibrating the throttle servo, refer to packet EFI_PKT_TESTMODE.                                                                              |
[Dynamic and Sticky Errors packet bytes]


## <a name="EFI_PKT_ENGINEWEAR"></a>Engine Wear

Information about engine wear, output at the slow telemetry rate.

- packet identifier: `EFI_PKT_ENGINEWEAR` : 8
- data length: 16


| Bytes       | Name           | [Enc](#Enc) | Repeat | Description                                                                                                             |
| ----------- | -------------- | :---------: | :----: | ----------------------------------------------------------------------------------------------------------------------- |
| 0...3       | 1)hobbs        | U32         | 1      | Total engine run time in seconds.                                                                                       |
| 4...7       | 2)revcount     | U32         | 1      | Total number of revolutions the engine has made.                                                                        |
| 8:7...10:4  | 3)hotTime      | B20         | 1      | Seconds the engine has spent running above the CHT limit.<br>Scaled by 0.1 from 0.0 to 10485750.0.                      |
| 10:3...12:0 | 4)highLoadTime | B20         | 1      | Seconds the engine has spent running at high load (throttle greater than 80%).<br>Scaled by 0.1 from 0.0 to 10485750.0. |
| 13          | 5)peakCHT      | U8          | 1      | Highest head temperature observed while the engine was running.<br>Scaled by 0.5 from 0.0 to 510.0.                     |
| 14:7...14:4 | 6)reserved     | B4          | 1      | Reserved bits in the packet.                                                                                            |
| 14:3...15:0 | 7)numStarts    | B12         | 1      | Number of times the engine has started.                                                                                 |
[Engine Wear packet bytes]


## <a name="EFI_PKT_ENGINEWEAR"></a>Short Engine Wear

Short information about engine wear, output at the slow telemetry rate over the CAN bus.

- packet identifier: `EFI_PKT_ENGINEWEAR` : 8
- data length: 8


| Bytes | Name       | [Enc](#Enc) | Repeat | Description                                      |
| ----- | ---------- | :---------: | :----: | ------------------------------------------------ |
| 0...3 | 1)hobbs    | U32         | 1      | Total engine run time in seconds.                |
| 4...7 | 2)revcount | U32         | 1      | Total number of revolutions the engine has made. |
[Short Engine Wear packet bytes]


## <a name="EFI_PKT_ENGINEWEAREXT"></a>Extended Engine Wear

Engine wear information beyond just hobbs and revcount, output at the slow telemetry rate over the CAN bus. This packet, in combination with `EngineWearShort` is equivalent to the full `EngineWear` packet.

- packet identifier: `EFI_PKT_ENGINEWEAREXT` : 49
- data length: 8


| Bytes     | Name           | [Enc](#Enc) | Repeat | Description                                                                                                             |
| --------- | -------------- | :---------: | :----: | ----------------------------------------------------------------------------------------------------------------------- |
| 0:7...2:4 | 1)hotTime      | B20         | 1      | Seconds the engine has spent running above the CHT limit.<br>Scaled by 0.1 from 0.0 to 10485750.0.                      |
| 2:3...4:0 | 2)highLoadTime | B20         | 1      | Seconds the engine has spent running at high load (throttle greater than 80%).<br>Scaled by 0.1 from 0.0 to 10485750.0. |
| 5         | 3)peakCHT      | U8          | 1      | Highest head temperature observed while the engine was running.<br>Scaled by 0.5 from 0.0 to 510.0.                     |
| 6:7...6:4 | 4)reserved     | B4          | 1      | Reserved bits in the packet.                                                                                            |
| 6:3...7:0 | 5)numStarts    | B12         | 1      | Number of times the engine has started.                                                                                 |
[Extended Engine Wear packet bytes]


## <a name="EFI_PKT_TELEMETRYCOMMS"></a>Telemetry about communications

Information about communications performance and configuration, output at the slow telemetry rate.

- packet identifier: `EFI_PKT_TELEMETRYCOMMS` : 40
- data length: 8

Enumeration of CAN bus errors

| Name            | Value | Description                                             |
| --------------- | :---: | ------------------------------------------------------- |
| No error        | 0     | No error                                                |
| Stuff           | 1     | Bit stuff error (more than 5 consecutive bits the same) |
| Form            | 2     | Form error                                              |
| Acknowledgement | 3     | Frame acknowledgement error                             |
| Bit recessive   | 4     | Bit recessive error                                     |
| Bit dominant    | 5     | Bit dominant error                                      |
| CRC             | 6     | CRC error                                               |
| Software        | 7     | Software error                                          |
[<a name="efiCANerr"></a>CAN errors enumeration]





| Bytes     | Name                         | [Enc](#Enc) | Repeat | Description                                                                                                                                                                      |
| --------- | ---------------------------- | :---------: | :----: | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0         | 1)canREC                     | U8          | 1      | The receive error counter from the CAN bus hardware.                                                                                                                             |
| 1         | 2)canTEC                     | U8          | 1      | The least significant 8 bits of the transmit error counter from the CAN bus hardware.                                                                                            |
| 2:7...2:5 | 3)[lastCANerr](#efiCANerr)   | B3          | 1      | Cause of the last CAN error.                                                                                                                                                     |
| 2:4       | 4)canBOF                     | B1          | 1      | This bit is set when the CAN hardware enters the bus-off state. The bus-off state is entered on canTEC overflow, greater than 255.                                               |
| 2:3       | 5)canEPVF                    | B1          | 1      | This bit is set when the CAN error passive limit has been reached (canREC or canTEC greater than 127).                                                                           |
| 2:2       | 6)canEWGF                    | B1          | 1      | This bit is set when the CAN warning limit has been reached (canREC or canTEC greater than 95).                                                                                  |
| 2:1       | 7)canDisableNativeInput      | B1          | 1      | This bit echos the CAN configuration bit `disableNativeInput`. If set then native CAN packets cannot be sent to this EFI (except possibly using the broadcast ID).               |
| 2:0       | 8)reserved                   | B1          | 1      | Reserved bits in the packet.                                                                                                                                                     |
| 3:7       | 9)canDisableNativeOutput     | B1          | 1      | This bit echos the CAN configuration bit `disableNativeOutput`. If set then native CAN packets will not be sent by this EFI.                                                     |
| 3:6       | 10)canDisableNativeBroadcast | B1          | 1      | This bit echos the CAN configuration bit `disableNativeBroadcast`. If set then native CAN packets cannot be sent to this EFI using the broadcast ID.                             |
| 3:5       | 11)canForcePacketOverCAN     | B1          | 1      | This bit echos the CAN configuration bit `forcePacketOverCAN`. If set any CAN packets sent to this EFI must use the packet-over-CAN schema, otherwise they will be ignored.      |
| 3:4       | 12)canCmdAndTmOnly           | B1          | 1      | This bit echos the CAN configuration bit `cmdAndTmOnly`. If set then only commands and telemetry are supported on the CAN bus (configuration request or change will be ignored). |
| 3:3       | 13)canTxFull                 | B1          | 1      | Set if the transmit buffer of the CAN interface is full.                                                                                                                         |
| 3:2...3:0 | 14)canPacketFailureCnt       | B3          | 1      | Number of packets received over the CAN bus which were invalid (bad CRC) since the last time this packet was output.                                                             |
| 4:7       | 15)uartTxFull                | B1          | 1      | Set if the transmit buffer of the UART interface is full.                                                                                                                        |
| 4:6...4:4 | 16)uartPacketFailureCnt      | B3          | 1      | Number of packets received over the UART which were invalid (bad CRC) since the last time this packet was output.                                                                |
| 4:3       | 17)usbTxFull                 | B1          | 1      | Set if the transmit buffer of the USB interface is full.                                                                                                                         |
| 4:2...4:0 | 18)usbPacketFailureCnt       | B3          | 1      | Number of packets received over the USB which were invalid (bad CRC) since the last time this packet was output.                                                                 |
| 5         | 19)txCANPacketCounter        | U8          | 1      | Free running counter of packets transmitted over CAN (not counting this packet).                                                                                                 |
| 6         | 20)txPacketCounter           | U8          | 1      | Free running counter of packets transmitted over the UART or USB (not counting this packet).                                                                                     |
| 7         | 21)rxPacketCounter           | U8          | 1      | Free running counter for received packets (from any source).                                                                                                                     |
[Telemetry about communications packet bytes]


## SD Status

SD Card status

| Name            | Value | Description                                                                             |
| --------------- | :---: | --------------------------------------------------------------------------------------- |
| SD bad card     | 0     | The SD card has not been detected; it may be missing, or the hardware may be bad.       |
| SD bad boot     | 1     | The SD card does not have a valid boot sector, it needs formatting.                     |
| SD bad volume   | 2     | The SD card does not have a valid volume sector, it needs formatting.                   |
| SD bad root     | 3     | The SD card does not have the expected root directory layout, it needs directory setup. |
| SD formatting   | 4     | The SD card is being formatted. This status is always followed by `EFI_SD_SETTINGUP`.   |
| SD setting up   | 5     | The SD card root directory is being setup.                                              |
| SD ready        | 6     | The SD card is ready to record.                                                         |
| SD transferring | 7     | The SD card is reporting recorded data.                                                 |
[<a name="efiSDStatus"></a>SD Status enumeration]



## <a name="EFI_PKT_TELEMETRYSDCARD"></a>SD card telemetry

Telemetry about the SD card. This packet is only sent if the SD card is present. Presence of the SD card is always visible in the [CPU telemetry](#EFI_PKT_TELEMETRYCPU) packet.

- packet identifier: `EFI_PKT_TELEMETRYSDCARD` : 45
- data length: 8


| Bytes     | Name                       | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                                                                                                |
| --------- | -------------------------- | :---------: | :----: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 0:7...0:5 | 1)[sdstatus](#efiSDStatus) | B3          | 1      | Status of the SD card.                                                                                                                                                                                                                                                                                                                                                                     |
| 0:4...0:0 | 2)errorCount               | B5          | 1      | The number of SD card errors experienced since bootup.                                                                                                                                                                                                                                                                                                                                     |
| 1...3     | 3)sequence                 | U24         | 1      | If `sdstatus` is `EFI_SD_READY` this is the number of 512-byte blocks that have been written to the card since the system booted up. If `sdstatus` is `EFI_SD_SETTINGUP` this gives the progress of the directory setup in tenths of a percent. If `sdstatus` is `EFI_SD_TRANSFER` this gives the progress of the transfer in tenths of a percent. In all other states this value is zero. |
| 4...7     | 4)numDataBlocks            | U32         | 1      | The total number of data blocks that have been written to the card, this does not include the journal blocks.                                                                                                                                                                                                                                                                              |
[SD card telemetry packet bytes]


## <a name="EFI_PKT_TELEMETRYOILINJ"></a>Oil Injection Telemetry

Telemetry about the oil injection system. The oil injection system must be enabled in the injector configuration. If oil injection is not enabled this packet will not be sent.

- packet identifier: `EFI_PKT_TELEMETRYOILINJ` : 60
- data length: 8


| Bytes | Name             | [Enc](#Enc) | Repeat | Description                                                                                                     |
| ----- | ---------------- | :---------: | :----: | --------------------------------------------------------------------------------------------------------------- |
| 0...1 | 1)oilFlowRate    | F16:10      | 1      | Oil flow rate in grams per minute.                                                                              |
| 2...3 | 2)oilConsumption | F16:10      | 1      | Oil consumption in grams since the system turned on.                                                            |
| 4     | 3)oilDuty        | U8          | 1      | Oil injector pump duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                      |
| 5     | 4)oilFreq        | U8          | 1      | Oil injector pump frequency in Hz.<br>Scaled by 10 from 0.0 to 25.5.                                            |
| 6     | 5)oilPrimeLeft   | U8          | 1      | Number of oil injection priming pulses remaining, zero if priming is off.<br>Scaled by 0.05 from 0.0 to 5100.0. |
| 7     | 6)reserved       | U8          | 1      | Reserved bytes in the packet.                                                                                   |
[Oil Injection Telemetry packet bytes]


## <a name="EFI_PKT_TELEMETRYGCU"></a>GCU Telemetry

Telemetry about the Generator Control Unit (GCU). The GCU is external hardware that sends status information on the CAN bus. This packet is only sent by IntelliJect if the GCU data are detected on the CAN bus. Detection of the GCU is always visible in the [CPU telemetry](#EFI_PKT_TELEMETRYCPU) packet.

- packet identifier: `EFI_PKT_TELEMETRYGCU` : 58
- data length: 8

| Name          | Value | Description |
| ------------- | :---: | ----------- |
| Northwest GCU | 0     |             |
| Millswood PMU | 1     |             |
| P4F Generator | 2     |             |
[<a name="gcuSource"></a>Source of GCU information enumeration]





| Bytes     | Name                   | [Enc](#Enc) | Repeat | Description                                                                   |
| --------- | ---------------------- | :---------: | :----: | ----------------------------------------------------------------------------- |
| 0:7...0:6 | 1)[source](#gcuSource) | B2          | 1      | Enumeration describing the source of the GCU information.                     |
| 0:5...1:0 | 2)volts28              | B14         | 1      | Voltage of the 28 V rail.<br>Scaled by 100 from 0.0 to 163.83.                |
| 2:7...3:4 | 3)volts12              | B12         | 1      | Voltage of the 12 V rail.<br>Scaled by 100 from 0.0 to 40.95.                 |
| 3:3...4:0 | 4)volts6               | B12         | 1      | Voltage of the 6 V rail.<br>Scaled by 100 from 0.0 to 40.95.                  |
| 5         | 5)gcuTemp              | U8          | 1      | Internal temperature of the GCU in Celsius.<br>Scaled by 2 from 0.0 to 127.5. |
| 6:7       | 6)cranking             | B1          | 1      | Set if the GCU is cranking the engine for start.                              |
| 6:6       | 7)generating           | B1          | 1      | Set if the GCU is making electricity from the generator.                      |
| 6:5       | 8)charging             | B1          | 1      | Set if the GCU is charging the battery.                                       |
| 6:4       | 9)startAvailable       | B1          | 1      | Set if the GCU starter is available.                                          |
| 6:3...7:0 | 10)current28           | B12         | 1      | Current in Amps of the 28 V rail.<br>Scaled by 100 from 0.0 to 40.95.         |
[GCU Telemetry packet bytes]


## <a name="EFI_PKT_TELEMETRYSLOWSUM"></a>Slow Telemetry Summary Packet

Summary packet that contains all the slow telemetry in a single packet. The summary packet is only used on non-CAN interfaces (unless `forcePacketOverCAN` is set), and only if it is enabled in the communications configuration. Since each ordinary telemetry packet is only 8 bytes long, combining multiple telmetry packets into a single summary packet can significantly reduce packet overhead.

- packet identifier: `EFI_PKT_TELEMETRYSLOWSUM` : 36
- data length: 128


| Bytes         | Name                                      | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                                                                                                |
| ------------- | ----------------------------------------- | :---------: | :----: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 0...7         | 1)time                                                 || 1      | Time information for the slow telemetry                                                                                                                                                                                                                                                                                                                                                    |
| 0...3         | 1.1)time                                  | U32         | 1      | Time in milliseconds since the system booted up.                                                                                                                                                                                                                                                                                                                                           |
| 4:7           | 1.2)fasttime                              | B1          | 1      | If set this time packet applies to fast telemetry data that immediately follows this packet.                                                                                                                                                                                                                                                                                               |
| 4:6           | 1.3)slowtime                              | B1          | 1      | If set this time packet applies to slow telemetry data that follows this packet.                                                                                                                                                                                                                                                                                                           |
| 4:5...4:4     | 1.4)reserved                              | B2          | 1      | Reserved bits for future expansion.                                                                                                                                                                                                                                                                                                                                                        |
| 4:3...7:0     | 1.5)revcount                              | B28         | 1      | Number of engine revolutions since the engine was last started.                                                                                                                                                                                                                                                                                                                            |
| 8...15        | 2)sensors                                              || 1      | First set of sensor information                                                                                                                                                                                                                                                                                                                                                            |
| 8             | 2.1)_cht                                  | U8          | 1      | Combined cylinder head temperature in C, used for table lookups. This is a deprecated field, prefer the field in Telemetry Sensors 4.<br>Scaled by 1 from 0.0 to 255.0.                                                                                                                                                                                                                    |
| 9:7...9:5     | 2.2)[throttlePosSrc](#efiThrottleSource)  | B3          | 1      | Source of throttle position information used for table lookups.                                                                                                                                                                                                                                                                                                                            |
| 9:4           | 2.3)spareTempIsCHT3                       | B1          | 1      | This bit is set if spare temperature is configured to act as the third CHT.                                                                                                                                                                                                                                                                                                                |
| 9:3           | 2.4)spareTempIsMAT2                       | B1          | 1      | This bit is set if spare temperature is configured to act as the second MAT.                                                                                                                                                                                                                                                                                                               |
| 9:2           | 2.5)spareTempIsOilT                       | B1          | 1      | This bit is set if spare temperature is configured to act as oil temperature.                                                                                                                                                                                                                                                                                                              |
| 9:1...10:0    | 2.6)sparetemp                             | B10         | 1      | Spare temperature in C.<br>Scaled by 2 from -100.0 to 411.5.                                                                                                                                                                                                                                                                                                                               |
| 11            | 2.7)mat                                   | I8          | 1      | Manifold air temperature in C.<br>Scaled by 1 from -127.0 to 127.0.                                                                                                                                                                                                                                                                                                                        |
| 12:7...13:4   | 2.8)map                                   | B12         | 1      | Manifold air pressure in kiloPascals.<br>Scaled by 31.25 from 0.0 to 131.04.                                                                                                                                                                                                                                                                                                               |
| 13:3...14:0   | 2.9)baro                                  | B12         | 1      | Baro air pressure in kiloPascals.<br>Scaled by 31.25 from 0.0 to 131.04.                                                                                                                                                                                                                                                                                                                   |
| 15            | 2.10)oat                                  | I8          | 1      | Outside air temperature (as measured by the digital barometer) in C.<br>Scaled by 1 from -127.0 to 127.0.                                                                                                                                                                                                                                                                                  |
| 16...23       | 3)sensors2                                             || 1      | Second set of sensor information                                                                                                                                                                                                                                                                                                                                                           |
| 16:7...17:4   | 3.1)measuredCrank2Angle                   | B12         | 1      | Measured synchronization angle of crank sense 2, as determined by crank sense 1.<br>Scaled by 10 from 0.0 to 409.5.                                                                                                                                                                                                                                                                        |
| 17:3...17:0   | 3.2)devCrank2Angle                        | B4          | 1      | Standard deviation of the measured synchronization angle of crank sense 2, as determined by crank sense 1.<br>Scaled by 2 from 0.0 to 7.5.                                                                                                                                                                                                                                                 |
| 18:7...19:4   | 3.3)_density                              | B12         | 1      | Estimated air density in kilogram per cubic meter, deprecated in api 10.<br>Scaled by 4095/(3.0) from 0.0 to 3.0.                                                                                                                                                                                                                                                                          |
| 19:3          | 3.4)expectSensors4                        | B1          | 1      | If this bit is set the receiver can expect that sensors4 data will be sent from the EFI, otherwise the EFI is older and the deprecated fields in TelemetrySensors and TelemetrySensors2 should be used.<br>Data are given constant value on encode 1.                                                                                                                                      |
| 19:2          | 3.5)api8                                  | B1          | 1      | Set if this packet comes from API 8 or later.<br>Data are given constant value on encode 1.                                                                                                                                                                                                                                                                                                |
| 19:1          | 3.6)crankSense1Active                     | B1          | 1      | Set if the first crank sense signal is in the active state.                                                                                                                                                                                                                                                                                                                                |
| 19:0          | 3.7)crankSense2Active                     | B1          | 1      | Set if the second crank sense signal is in the active state.                                                                                                                                                                                                                                                                                                                               |
| 20            | 3.8)cooling2                              | U8          | 1      | Second cooling output in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                                                                                                                                        |
| 21            | 3.9)tpsError                              | I8          | 1      | Error in percent between throttle output and TPS measurement.<br>Scaled by 10 from -12.7 to 12.7.                                                                                                                                                                                                                                                                                          |
| 22            | 3.10)analogTPS                            | U8          | 1      | Analog throttle position sensor in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                                                                                                                              |
| 23            | 3.11)pwmTPS                               | U8          | 1      | PWM throttle position sensor in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                                                                                                                                 |
| 24...31       | 4)fuel                                                 || 1      | Fuel flow and fuel used information                                                                                                                                                                                                                                                                                                                                                        |
| 24...25       | 4.1)fuelFlowRate                          | F16:10      | 1      | Fuel flow rate in grams per minute.                                                                                                                                                                                                                                                                                                                                                        |
| 26...27       | 4.2)fuelConsumption                       | F16:10      | 1      | Fuel consumption in grams since the system turned on.                                                                                                                                                                                                                                                                                                                                      |
| 28            | 4.3)fmInjector1                           | U8          | 1      | Total fuel multiplier for injector 1, including the main fuel multiplier.<br>Scaled by 50 from 0.0 to 5.1.                                                                                                                                                                                                                                                                                 |
| 29            | 4.4)fmInjector2                           | U8          | 1      | Total fuel multiplier for injector 2, including the main fuel multiplier.<br>Scaled by 50 from 0.0 to 5.1.                                                                                                                                                                                                                                                                                 |
| 30            | 4.5)fmInjector3                           | U8          | 1      | Total fuel multiplier for injector 3, including the main fuel multiplier.<br>Scaled by 50 from 0.0 to 5.1.                                                                                                                                                                                                                                                                                 |
| 31            | 4.6)reserved                              | U8          | 1      | Reserved bytes in the packet.                                                                                                                                                                                                                                                                                                                                                              |
| 32...39       | 5)injector                                             || 1      | Injector(s) information                                                                                                                                                                                                                                                                                                                                                                    |
| 32:7          | 5.1)reserved                              | B1          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                               |
| 32:6          | 5.2)injector1usesense1                    | B1          | 1      | Set if injector 1 is being triggered by sense 1.                                                                                                                                                                                                                                                                                                                                           |
| 32:5          | 5.3)injector1usesense2                    | B1          | 1      | Set if injector 1 is being triggered by sense 2.                                                                                                                                                                                                                                                                                                                                           |
| 32:4...34:4   | 5.4)injector1Time                         | B17         | 1      | First injector on time for each engine cycle in microseconds. This does not include the injector trim time.<br>Scaled by 1.0 from 0.0 to 131071.0.                                                                                                                                                                                                                                         |
| 34:3...35:2   | 5.5)injection1Angle                       | B10         | 1      | Crank angle of the first injection.<br>Scaled by 2 from 0.0 to 511.5.                                                                                                                                                                                                                                                                                                                      |
| 35:1...35:0   | 5.6)reserved                              | B2          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                               |
| 36:7          | 5.7)reserved                              | B1          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                               |
| 36:6          | 5.8)injector2usesense1                    | B1          | 1      | Set if injector 2 is being triggered by sense 1.                                                                                                                                                                                                                                                                                                                                           |
| 36:5          | 5.9)injector2usesense2                    | B1          | 1      | Set if injector 2 is being triggered by sense 2.                                                                                                                                                                                                                                                                                                                                           |
| 36:4...38:4   | 5.10)injector2Time                        | B17         | 1      | Second injector on time for each engine cycle in microseconds. This does not include the injector trim time.<br>Scaled by 1.0 from 0.0 to 131071.0.                                                                                                                                                                                                                                        |
| 38:3...39:2   | 5.11)injection2Angle                      | B10         | 1      | Crank angle of the second injection.<br>Scaled by 2 from 0.0 to 511.5.                                                                                                                                                                                                                                                                                                                     |
| 39:1          | 5.12)injector3usesense1                   | B1          | 1      | Set if injector 3 is being triggered by sense 1.                                                                                                                                                                                                                                                                                                                                           |
| 39:0          | 5.13)injector3usesense2                   | B1          | 1      | Set if injector 3 is being triggered by sense 2.                                                                                                                                                                                                                                                                                                                                           |
| 40...47       | 6)slow                                                 || 1      | Slow telemetry data                                                                                                                                                                                                                                                                                                                                                                        |
| 40            | 6.1)sparkAdvance1                         | I8          | 1      | Spark advance in degrees for the first spark output.<br>Scaled by 2 from -63.5 to 63.5.                                                                                                                                                                                                                                                                                                    |
| 41:7...41:6   | 6.2)reserved                              | B2          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                               |
| 41:5          | 6.3)spark1usesense1                       | B1          | 1      | Set if spark 1 is being triggered by sense 1.                                                                                                                                                                                                                                                                                                                                              |
| 41:4          | 6.4)spark1usesense2                       | B1          | 1      | Set if spark 1 is being triggered by sense 2.                                                                                                                                                                                                                                                                                                                                              |
| 41:3          | 6.5)spark2usesense1                       | B1          | 1      | Set if spark 2 is being triggered by sense 1.                                                                                                                                                                                                                                                                                                                                              |
| 41:2          | 6.6)spark2usesense2                       | B1          | 1      | Set if spark 2 is being triggered by sense 2.                                                                                                                                                                                                                                                                                                                                              |
| 41:1          | 6.7)spark3usesense1                       | B1          | 1      | Set if spark 3 is being triggered by sense 1.                                                                                                                                                                                                                                                                                                                                              |
| 41:0          | 6.8)spark3usesense2                       | B1          | 1      | Set if spark 3 is being triggered by sense 2.                                                                                                                                                                                                                                                                                                                                              |
| 42...43       | 6.9)power                                 | F16:10      | 1      | Estimated engine shaft power in Watts.                                                                                                                                                                                                                                                                                                                                                     |
| 44:7          | 6.10)rpmControllerFromUser                | B1          | 1      | Set if the RPM controller is running and gettings its command from the user.                                                                                                                                                                                                                                                                                                               |
| 44:6          | 6.11)rpmControllerFromThrottle            | B1          | 1      | Set if the RPM controller is running and getting its command from the throttle input, or the throttle limiter.                                                                                                                                                                                                                                                                             |
| 44:5...45:0   | 6.12)rpmcmd                               | B14         | 1      | Engine speed command in revolutions per minute.<br>Scaled by 1 from 0.0 to 16383.0.                                                                                                                                                                                                                                                                                                        |
| 46            | 6.13)pumpduty                             | U8          | 1      | Fuel pump duty cycle in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                                                                                                                                         |
| 47            | 6.14)cooling1                             | U8          | 1      | First cooling output in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                                                                                                                                         |
| 48...55       | 7)cpu                                                  || 1      | Run time CPU information                                                                                                                                                                                                                                                                                                                                                                   |
| 48            | 7.1)cpuLoad                               | U8          | 1      | Total cpu loading in percent.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                                                                                                                                            |
| 49            | 7.2)intLoad                               | U8          | 1      | Percentage of CPU time spent in interrupts.<br>Scaled by 10 from 0.0 to 25.5.                                                                                                                                                                                                                                                                                                              |
| 50            | 7.3)stack                                 | U8          | 1      | Percentage of available stack space that has been used.<br>Scaled by 2 from 0.0 to 127.5.                                                                                                                                                                                                                                                                                                  |
| 51            | 7.4)cpuTemp                               | I8          | 1      | CPU temperature in C.<br>Scaled by 1 from -127.0 to 127.0.                                                                                                                                                                                                                                                                                                                                 |
| 52:7          | 7.5)safeModeActive                        | B1          | 1      | If set the system booted in safe mode due to too many resets. No settings data were loaded from non-volatile storage in this mode. The only way to exit safe mode is to reset the system.                                                                                                                                                                                                  |
| 52:6          | 7.6)passwordRequired                      | B1          | 1      | If set the EFI requires a password to be unlocked.                                                                                                                                                                                                                                                                                                                                         |
| 52:5...52:4   | 7.7)unlockLevel                           | B2          | 1      | EFI unlock level. 0 indicates the EFI is locked, '1' indicates the EFI is partially unlocked, and '2' indicates it is password unlocked.                                                                                                                                                                                                                                                   |
| 52:3          | 7.8)sdcardPresent                         | B1          | 1      | If set an SD card is detected and configured for data recording, and the SD card telemetry packet should be expected.                                                                                                                                                                                                                                                                      |
| 52:2          | 7.9)gcuPresent                            | B1          | 1      | If set a GCU (generator control unit) is detected on the CNA bus and the GCU telemetry packet should be expected.                                                                                                                                                                                                                                                                          |
| 52:1...52:0   | 7.10)reserved                             | B2          | 1      | reserved bits.                                                                                                                                                                                                                                                                                                                                                                             |
| 53            | 7.11)watchdog                             | U8          | 1      | Maximum interval of time, in microseconds, between watchdog service events.<br>Scaled by 0.1 from 0.0 to 2550.0.                                                                                                                                                                                                                                                                           |
| 54:7...54:5   | 7.12)reserved                             | B3          | 1      | reserved bits.                                                                                                                                                                                                                                                                                                                                                                             |
| 54:4...54:0   | 7.13)api                                  | B5          | 1      | Application programming interface number. Changes to the ICD will increment this number.<br>Data are given constant value on encode geteficommsApi().                                                                                                                                                                                                                                      |
| 55            | 7.14)interrupt                            | U8          | 1      | Maximum amount time spent in an interrupt, or with interrupts blocked, in microseconds.                                                                                                                                                                                                                                                                                                    |
| 56...63       | 8)sensors3                                             || 1      | Third set of sensor information                                                                                                                                                                                                                                                                                                                                                            |
| 56:7...57:7   | 8.1)inputVolts                            | B9          | 1      | Input voltage in Volts.<br>Scaled by 10 from 0.0 to 51.1.                                                                                                                                                                                                                                                                                                                                  |
| 57:6...57:0   | 8.2)inputCurrent                          | B7          | 1      | Input current in Amps.<br>Scaled by 40 from 0.0 to 3.175.                                                                                                                                                                                                                                                                                                                                  |
| 58            | 8.3)volts12                               | U8          | 1      | 12V rail voltage in Volts.<br>Scaled by 25 from 7.0 to 17.2.                                                                                                                                                                                                                                                                                                                               |
| 59            | 8.4)current12                             | U8          | 1      | 12V rail current in Amps.<br>Scaled by 40 from 0.0 to 6.375.                                                                                                                                                                                                                                                                                                                               |
| 60:7...61:4   | 8.5)baroDigital                           | B12         | 1      | Baro air pressure in kiloPascals from the digital barometer.<br>Scaled by 31.25 from 0.0 to 131.04.                                                                                                                                                                                                                                                                                        |
| 61:3...62:0   | 8.6)baroAnalog                            | B12         | 1      | Baro air pressure in kiloPascals from the analog barometer.<br>Scaled by 31.25 from 0.0 to 131.04.                                                                                                                                                                                                                                                                                         |
| 63            | 8.7)reserved                              | U8          | 1      | Reserved bytes in the packet.                                                                                                                                                                                                                                                                                                                                                              |
| 64...71       | 9)dynamicError                                         || 1      | Dynamic engine errors                                                                                                                                                                                                                                                                                                                                                                      |
| 64:7          | 9.1)inputvoltagelow                       | B1          | 1      | Set if input voltage is too low.                                                                                                                                                                                                                                                                                                                                                           |
| 64:6          | 9.2)inputvoltagehigh                      | B1          | 1      | Set if input voltage is too high.                                                                                                                                                                                                                                                                                                                                                          |
| 64:5          | 9.3)matsensorunderrng                     | B1          | 1      | Set if manifold air temperature sensor is out of range low.                                                                                                                                                                                                                                                                                                                                |
| 64:4          | 9.4)matsensoroverrng                      | B1          | 1      | Set if manifold air temperature sensor is out of range high.                                                                                                                                                                                                                                                                                                                               |
| 64:3          | 9.5)cht1sensorunderrng                    | B1          | 1      | Set if first cylinder head temperature sensor is out of range low.                                                                                                                                                                                                                                                                                                                         |
| 64:2          | 9.6)cht1sensoroverrng                     | B1          | 1      | Set if first cylinder head temperature sensor is out of range high.                                                                                                                                                                                                                                                                                                                        |
| 64:1          | 9.7)cht2sensorunderrng                    | B1          | 1      | Set if second cylinder head temperature sensor is out of range low.                                                                                                                                                                                                                                                                                                                        |
| 64:0          | 9.8)cht2sensoroverrng                     | B1          | 1      | Set if second cylinder head temperature sensor is out of range high.                                                                                                                                                                                                                                                                                                                       |
| 65:7          | 9.9)mapsensorunderrng                     | B1          | 1      | Set if manifold air pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                                   |
| 65:6          | 9.10)mapsensoroverrng                     | B1          | 1      | Set if manifold air pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                                  |
| 65:5          | 9.11)fuelpsensorunderrng                  | B1          | 1      | Set if fuel pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                                           |
| 65:4          | 9.12)fuelpsensoroverrng                   | B1          | 1      | Set if fuel pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                                          |
| 65:3          | 9.13)analogtpssensorunderrng              | B1          | 1      | Set if analog TPS sensor is out of range low.                                                                                                                                                                                                                                                                                                                                              |
| 65:2          | 9.14)analogtpssensoroverrng               | B1          | 1      | Set if analog TPS is out of range high.                                                                                                                                                                                                                                                                                                                                                    |
| 65:1          | 9.15)cputemplow                           | B1          | 1      | Set if CPU temperature is too low.                                                                                                                                                                                                                                                                                                                                                         |
| 65:0          | 9.16)cputemphigh                          | B1          | 1      | Set if CPU temperature is too high.                                                                                                                                                                                                                                                                                                                                                        |
| 66:7          | 9.17)sparetempsensorunderrng              | B1          | 1      | Set if spare temperature sensor is out of range low.                                                                                                                                                                                                                                                                                                                                       |
| 66:6          | 9.18)sparetempsensoroverrng               | B1          | 1      | Set if spare temperature sensor is out of range high.                                                                                                                                                                                                                                                                                                                                      |
| 66:5          | 9.19)barosensorunderrng                   | B1          | 1      | Set if barometric air pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                                 |
| 66:4          | 9.20)barosensoroverrng                    | B1          | 1      | Set if barometric air pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                                |
| 66:3...66:1   | 9.21)crank1error                          | B3          | 1      | Enumerated errors from crank sense1, clear if no errors.                                                                                                                                                                                                                                                                                                                                   |
| 66:0...67:6   | 9.22)crank2error                          | B3          | 1      | Enumerated errors from crank sense2, clear if no errors.                                                                                                                                                                                                                                                                                                                                   |
| 67:5          | 9.23)cht1low                              | B1          | 1      | Set if the first cylinder head temperature is too low, the head is overcooled.                                                                                                                                                                                                                                                                                                             |
| 67:4          | 9.24)cht1high                             | B1          | 1      | Set if the first cylinder head temperature is too high, the head is overheating.                                                                                                                                                                                                                                                                                                           |
| 67:3          | 9.25)cht2low                              | B1          | 1      | Set if the second cylinder head temperature is too low, the head is overcooled.                                                                                                                                                                                                                                                                                                            |
| 67:2          | 9.26)cht2high                             | B1          | 1      | Set if the second cylinder head temperature is too high, the head is overheating.                                                                                                                                                                                                                                                                                                          |
| 67:1          | 9.27)fuelplow                             | B1          | 1      | Set if the fuel pressure is too low.                                                                                                                                                                                                                                                                                                                                                       |
| 67:0          | 9.28)fuelphigh                            | B1          | 1      | Set if the fuel pressure is too high.                                                                                                                                                                                                                                                                                                                                                      |
| 68:7          | 9.29)hardoverspeed                        | B1          | 1      | Set if the engine RPM exceeded the hard rev limit.                                                                                                                                                                                                                                                                                                                                         |
| 68:6          | 9.30)softoverspeed                        | B1          | 1      | Set if the engine RPM exceeded the soft rev limit.                                                                                                                                                                                                                                                                                                                                         |
| 68:5          | 9.31)parameterhash                        | B1          | 1      | Set if the nonvolatile parameters did not hash to the expected value.                                                                                                                                                                                                                                                                                                                      |
| 68:4          | 9.32)voltage12low                         | B1          | 1      | Set if 12V voltage is too low.                                                                                                                                                                                                                                                                                                                                                             |
| 68:3          | 9.33)voltage12high                        | B1          | 1      | Set if 12V voltage is too high.                                                                                                                                                                                                                                                                                                                                                            |
| 68:2          | 9.34)inputcurrentlow                      | B1          | 1      | Set if the input current is too low.                                                                                                                                                                                                                                                                                                                                                       |
| 68:1          | 9.35)inputcurrenthigh                     | B1          | 1      | Set if the input current is too high.                                                                                                                                                                                                                                                                                                                                                      |
| 68:0          | 9.36)current12low                         | B1          | 1      | Set if the current of the 12 volt rail is too low.                                                                                                                                                                                                                                                                                                                                         |
| 69:7          | 9.37)current12high                        | B1          | 1      | Set if the current of the 12 volt rail too high.                                                                                                                                                                                                                                                                                                                                           |
| 69:6          | 9.38)power6fault                          | B1          | 1      | Set if the 6 volt power supply is faulted.                                                                                                                                                                                                                                                                                                                                                 |
| 69:5          | 9.39)injector1fault                       | B1          | 1      | Set if the first injector has a fault.                                                                                                                                                                                                                                                                                                                                                     |
| 69:4          | 9.40)injector2fault                       | B1          | 1      | Set if the second injector has a fault.                                                                                                                                                                                                                                                                                                                                                    |
| 69:3          | 9.41)userStorageError                     | B1          | 1      | Set if there is a user storage error.                                                                                                                                                                                                                                                                                                                                                      |
| 69:2          | 9.42)analogbarosensorunderrng             | B1          | 1      | Set if the analog barometric air pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                      |
| 69:1          | 9.43)analogbarosensoroverrng              | B1          | 1      | Set if the analog barometric air pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                     |
| 69:0          | 9.44)oatsensorunderrng                    | B1          | 1      | Set if barometric air pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                                 |
| 70:7          | 9.45)oatsensoroverrng                     | B1          | 1      | Set if barometric air pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                                |
| 70:6          | 9.46)canThrottleMissing                   | B1          | 1      | Set if the throttle is configured to use a CAN servo, but the CAN servo has not been detected on the bus.                                                                                                                                                                                                                                                                                  |
| 70:5          | 9.47)canThrottleError                     | B1          | 1      | Set if the CAN throttle servo has an error bit set.                                                                                                                                                                                                                                                                                                                                        |
| 70:4          | 9.48)canCooling1Missing                   | B1          | 1      | Set if the cooling output 1 is configured to use a CAN servo, but the CAN servo has not been detected on the bus.                                                                                                                                                                                                                                                                          |
| 70:3          | 9.49)canCooling1Error                     | B1          | 1      | Set if the CAN cooling 1 servo has an error bit set.                                                                                                                                                                                                                                                                                                                                       |
| 70:2          | 9.50)maxstartthrottle                     | B1          | 1      | Set if throttle was greater than the maximum start throttle while the engine was cranking for start. In that case spark and injector outputs are suspended.                                                                                                                                                                                                                                |
| 70:1          | 9.51)maplow                               | B1          | 1      | Set if manifold air pressure sensor is low compared to the estimate.                                                                                                                                                                                                                                                                                                                       |
| 70:0          | 9.52)maphigh                              | B1          | 1      | Set if manifold air pressure sensor is high compared to the estimate.                                                                                                                                                                                                                                                                                                                      |
| 71:7          | 9.53)tpsmissing                           | B1          | 1      | Set if there is no throttle position source, but the system is configured for one.                                                                                                                                                                                                                                                                                                         |
| 71:6          | 9.54)tpserror                             | B1          | 1      | Set if there is a problem with the throttle position sensor: either it is invalid (none connected to analog or PWM), or it disagrees with the throttle output (if efi drives throttle).                                                                                                                                                                                                    |
| 71:5          | 9.55)sdcarderror                          | B1          | 1      | Set if an error occurred with the SD card recording. This error will only assert after the SD card has been successfully detected.                                                                                                                                                                                                                                                         |
| 71:4          | 9.56)reserved1                            | B1          | 1      | Reserved for future expansion.                                                                                                                                                                                                                                                                                                                                                             |
| 71:3          | 9.57)canCooling2Missing                   | B1          | 1      | Set if the cooling output 2 is configured to use a CAN servo, but the CAN servo has not been detected on the bus.                                                                                                                                                                                                                                                                          |
| 71:2          | 9.58)canCooling2Error                     | B1          | 1      | Set if the CAN cooling 2 servo has an error bit set.                                                                                                                                                                                                                                                                                                                                       |
| 71:1          | 9.59)maintenanceNeeded                    | B1          | 1      | If set a maintenance item has been triggered, see the maintenance packet for details.                                                                                                                                                                                                                                                                                                      |
| 71:0          | 9.60)testModeActive                       | B1          | 1      | If set the EFI is in test mode or is autocalibrating the throttle servo, refer to packet EFI_PKT_TESTMODE.                                                                                                                                                                                                                                                                                 |
| 72...79       | 10)stickyError                                         || 1      | Sticky engine errors                                                                                                                                                                                                                                                                                                                                                                       |
| 72:7          | 10.1)inputvoltagelow                      | B1          | 1      | Set if input voltage is too low.                                                                                                                                                                                                                                                                                                                                                           |
| 72:6          | 10.2)inputvoltagehigh                     | B1          | 1      | Set if input voltage is too high.                                                                                                                                                                                                                                                                                                                                                          |
| 72:5          | 10.3)matsensorunderrng                    | B1          | 1      | Set if manifold air temperature sensor is out of range low.                                                                                                                                                                                                                                                                                                                                |
| 72:4          | 10.4)matsensoroverrng                     | B1          | 1      | Set if manifold air temperature sensor is out of range high.                                                                                                                                                                                                                                                                                                                               |
| 72:3          | 10.5)cht1sensorunderrng                   | B1          | 1      | Set if first cylinder head temperature sensor is out of range low.                                                                                                                                                                                                                                                                                                                         |
| 72:2          | 10.6)cht1sensoroverrng                    | B1          | 1      | Set if first cylinder head temperature sensor is out of range high.                                                                                                                                                                                                                                                                                                                        |
| 72:1          | 10.7)cht2sensorunderrng                   | B1          | 1      | Set if second cylinder head temperature sensor is out of range low.                                                                                                                                                                                                                                                                                                                        |
| 72:0          | 10.8)cht2sensoroverrng                    | B1          | 1      | Set if second cylinder head temperature sensor is out of range high.                                                                                                                                                                                                                                                                                                                       |
| 73:7          | 10.9)mapsensorunderrng                    | B1          | 1      | Set if manifold air pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                                   |
| 73:6          | 10.10)mapsensoroverrng                    | B1          | 1      | Set if manifold air pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                                  |
| 73:5          | 10.11)fuelpsensorunderrng                 | B1          | 1      | Set if fuel pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                                           |
| 73:4          | 10.12)fuelpsensoroverrng                  | B1          | 1      | Set if fuel pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                                          |
| 73:3          | 10.13)analogtpssensorunderrng             | B1          | 1      | Set if analog TPS sensor is out of range low.                                                                                                                                                                                                                                                                                                                                              |
| 73:2          | 10.14)analogtpssensoroverrng              | B1          | 1      | Set if analog TPS is out of range high.                                                                                                                                                                                                                                                                                                                                                    |
| 73:1          | 10.15)cputemplow                          | B1          | 1      | Set if CPU temperature is too low.                                                                                                                                                                                                                                                                                                                                                         |
| 73:0          | 10.16)cputemphigh                         | B1          | 1      | Set if CPU temperature is too high.                                                                                                                                                                                                                                                                                                                                                        |
| 74:7          | 10.17)sparetempsensorunderrng             | B1          | 1      | Set if spare temperature sensor is out of range low.                                                                                                                                                                                                                                                                                                                                       |
| 74:6          | 10.18)sparetempsensoroverrng              | B1          | 1      | Set if spare temperature sensor is out of range high.                                                                                                                                                                                                                                                                                                                                      |
| 74:5          | 10.19)barosensorunderrng                  | B1          | 1      | Set if barometric air pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                                 |
| 74:4          | 10.20)barosensoroverrng                   | B1          | 1      | Set if barometric air pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                                |
| 74:3...74:1   | 10.21)crank1error                         | B3          | 1      | Enumerated errors from crank sense1, clear if no errors.                                                                                                                                                                                                                                                                                                                                   |
| 74:0...75:6   | 10.22)crank2error                         | B3          | 1      | Enumerated errors from crank sense2, clear if no errors.                                                                                                                                                                                                                                                                                                                                   |
| 75:5          | 10.23)cht1low                             | B1          | 1      | Set if the first cylinder head temperature is too low, the head is overcooled.                                                                                                                                                                                                                                                                                                             |
| 75:4          | 10.24)cht1high                            | B1          | 1      | Set if the first cylinder head temperature is too high, the head is overheating.                                                                                                                                                                                                                                                                                                           |
| 75:3          | 10.25)cht2low                             | B1          | 1      | Set if the second cylinder head temperature is too low, the head is overcooled.                                                                                                                                                                                                                                                                                                            |
| 75:2          | 10.26)cht2high                            | B1          | 1      | Set if the second cylinder head temperature is too high, the head is overheating.                                                                                                                                                                                                                                                                                                          |
| 75:1          | 10.27)fuelplow                            | B1          | 1      | Set if the fuel pressure is too low.                                                                                                                                                                                                                                                                                                                                                       |
| 75:0          | 10.28)fuelphigh                           | B1          | 1      | Set if the fuel pressure is too high.                                                                                                                                                                                                                                                                                                                                                      |
| 76:7          | 10.29)hardoverspeed                       | B1          | 1      | Set if the engine RPM exceeded the hard rev limit.                                                                                                                                                                                                                                                                                                                                         |
| 76:6          | 10.30)softoverspeed                       | B1          | 1      | Set if the engine RPM exceeded the soft rev limit.                                                                                                                                                                                                                                                                                                                                         |
| 76:5          | 10.31)parameterhash                       | B1          | 1      | Set if the nonvolatile parameters did not hash to the expected value.                                                                                                                                                                                                                                                                                                                      |
| 76:4          | 10.32)voltage12low                        | B1          | 1      | Set if 12V voltage is too low.                                                                                                                                                                                                                                                                                                                                                             |
| 76:3          | 10.33)voltage12high                       | B1          | 1      | Set if 12V voltage is too high.                                                                                                                                                                                                                                                                                                                                                            |
| 76:2          | 10.34)inputcurrentlow                     | B1          | 1      | Set if the input current is too low.                                                                                                                                                                                                                                                                                                                                                       |
| 76:1          | 10.35)inputcurrenthigh                    | B1          | 1      | Set if the input current is too high.                                                                                                                                                                                                                                                                                                                                                      |
| 76:0          | 10.36)current12low                        | B1          | 1      | Set if the current of the 12 volt rail is too low.                                                                                                                                                                                                                                                                                                                                         |
| 77:7          | 10.37)current12high                       | B1          | 1      | Set if the current of the 12 volt rail too high.                                                                                                                                                                                                                                                                                                                                           |
| 77:6          | 10.38)power6fault                         | B1          | 1      | Set if the 6 volt power supply is faulted.                                                                                                                                                                                                                                                                                                                                                 |
| 77:5          | 10.39)injector1fault                      | B1          | 1      | Set if the first injector has a fault.                                                                                                                                                                                                                                                                                                                                                     |
| 77:4          | 10.40)injector2fault                      | B1          | 1      | Set if the second injector has a fault.                                                                                                                                                                                                                                                                                                                                                    |
| 77:3          | 10.41)userStorageError                    | B1          | 1      | Set if there is a user storage error.                                                                                                                                                                                                                                                                                                                                                      |
| 77:2          | 10.42)analogbarosensorunderrng            | B1          | 1      | Set if the analog barometric air pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                      |
| 77:1          | 10.43)analogbarosensoroverrng             | B1          | 1      | Set if the analog barometric air pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                     |
| 77:0          | 10.44)oatsensorunderrng                   | B1          | 1      | Set if barometric air pressure sensor is out of range low.                                                                                                                                                                                                                                                                                                                                 |
| 78:7          | 10.45)oatsensoroverrng                    | B1          | 1      | Set if barometric air pressure sensor is out of range high.                                                                                                                                                                                                                                                                                                                                |
| 78:6          | 10.46)canThrottleMissing                  | B1          | 1      | Set if the throttle is configured to use a CAN servo, but the CAN servo has not been detected on the bus.                                                                                                                                                                                                                                                                                  |
| 78:5          | 10.47)canThrottleError                    | B1          | 1      | Set if the CAN throttle servo has an error bit set.                                                                                                                                                                                                                                                                                                                                        |
| 78:4          | 10.48)canCooling1Missing                  | B1          | 1      | Set if the cooling output 1 is configured to use a CAN servo, but the CAN servo has not been detected on the bus.                                                                                                                                                                                                                                                                          |
| 78:3          | 10.49)canCooling1Error                    | B1          | 1      | Set if the CAN cooling 1 servo has an error bit set.                                                                                                                                                                                                                                                                                                                                       |
| 78:2          | 10.50)maxstartthrottle                    | B1          | 1      | Set if throttle was greater than the maximum start throttle while the engine was cranking for start. In that case spark and injector outputs are suspended.                                                                                                                                                                                                                                |
| 78:1          | 10.51)maplow                              | B1          | 1      | Set if manifold air pressure sensor is low compared to the estimate.                                                                                                                                                                                                                                                                                                                       |
| 78:0          | 10.52)maphigh                             | B1          | 1      | Set if manifold air pressure sensor is high compared to the estimate.                                                                                                                                                                                                                                                                                                                      |
| 79:7          | 10.53)tpsmissing                          | B1          | 1      | Set if there is no throttle position source, but the system is configured for one.                                                                                                                                                                                                                                                                                                         |
| 79:6          | 10.54)tpserror                            | B1          | 1      | Set if there is a problem with the throttle position sensor: either it is invalid (none connected to analog or PWM), or it disagrees with the throttle output (if efi drives throttle).                                                                                                                                                                                                    |
| 79:5          | 10.55)sdcarderror                         | B1          | 1      | Set if an error occurred with the SD card recording. This error will only assert after the SD card has been successfully detected.                                                                                                                                                                                                                                                         |
| 79:4          | 10.56)reserved1                           | B1          | 1      | Reserved for future expansion.                                                                                                                                                                                                                                                                                                                                                             |
| 79:3          | 10.57)canCooling2Missing                  | B1          | 1      | Set if the cooling output 2 is configured to use a CAN servo, but the CAN servo has not been detected on the bus.                                                                                                                                                                                                                                                                          |
| 79:2          | 10.58)canCooling2Error                    | B1          | 1      | Set if the CAN cooling 2 servo has an error bit set.                                                                                                                                                                                                                                                                                                                                       |
| 79:1          | 10.59)maintenanceNeeded                   | B1          | 1      | If set a maintenance item has been triggered, see the maintenance packet for details.                                                                                                                                                                                                                                                                                                      |
| 79:0          | 10.60)testModeActive                      | B1          | 1      | If set the EFI is in test mode or is autocalibrating the throttle servo, refer to packet EFI_PKT_TESTMODE.                                                                                                                                                                                                                                                                                 |
| 80...95       | 11)wear                                                || 1      | Engine wear information                                                                                                                                                                                                                                                                                                                                                                    |
| 80...83       | 11.1)hobbs                                | U32         | 1      | Total engine run time in seconds.                                                                                                                                                                                                                                                                                                                                                          |
| 84...87       | 11.2)revcount                             | U32         | 1      | Total number of revolutions the engine has made.                                                                                                                                                                                                                                                                                                                                           |
| 88:7...90:4   | 11.3)hotTime                              | B20         | 1      | Seconds the engine has spent running above the CHT limit.<br>Scaled by 0.1 from 0.0 to 10485750.0.                                                                                                                                                                                                                                                                                         |
| 90:3...92:0   | 11.4)highLoadTime                         | B20         | 1      | Seconds the engine has spent running at high load (throttle greater than 80%).<br>Scaled by 0.1 from 0.0 to 10485750.0.                                                                                                                                                                                                                                                                    |
| 93            | 11.5)peakCHT                              | U8          | 1      | Highest head temperature observed while the engine was running.<br>Scaled by 0.5 from 0.0 to 510.0.                                                                                                                                                                                                                                                                                        |
| 94:7...94:4   | 11.6)reserved                             | B4          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                               |
| 94:3...95:0   | 11.7)numStarts                            | B12         | 1      | Number of times the engine has started.                                                                                                                                                                                                                                                                                                                                                    |
| 96...103      | 12)comms                                               || 1      | Communications error and performance information                                                                                                                                                                                                                                                                                                                                           |
| 96            | 12.1)canREC                               | U8          | 1      | The receive error counter from the CAN bus hardware.                                                                                                                                                                                                                                                                                                                                       |
| 97            | 12.2)canTEC                               | U8          | 1      | The least significant 8 bits of the transmit error counter from the CAN bus hardware.                                                                                                                                                                                                                                                                                                      |
| 98:7...98:5   | 12.3)[lastCANerr](#efiCANerr)             | B3          | 1      | Cause of the last CAN error.                                                                                                                                                                                                                                                                                                                                                               |
| 98:4          | 12.4)canBOF                               | B1          | 1      | This bit is set when the CAN hardware enters the bus-off state. The bus-off state is entered on canTEC overflow, greater than 255.                                                                                                                                                                                                                                                         |
| 98:3          | 12.5)canEPVF                              | B1          | 1      | This bit is set when the CAN error passive limit has been reached (canREC or canTEC greater than 127).                                                                                                                                                                                                                                                                                     |
| 98:2          | 12.6)canEWGF                              | B1          | 1      | This bit is set when the CAN warning limit has been reached (canREC or canTEC greater than 95).                                                                                                                                                                                                                                                                                            |
| 98:1          | 12.7)canDisableNativeInput                | B1          | 1      | This bit echos the CAN configuration bit `disableNativeInput`. If set then native CAN packets cannot be sent to this EFI (except possibly using the broadcast ID).                                                                                                                                                                                                                         |
| 98:0          | 12.8)reserved                             | B1          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                               |
| 99:7          | 12.9)canDisableNativeOutput               | B1          | 1      | This bit echos the CAN configuration bit `disableNativeOutput`. If set then native CAN packets will not be sent by this EFI.                                                                                                                                                                                                                                                               |
| 99:6          | 12.10)canDisableNativeBroadcast           | B1          | 1      | This bit echos the CAN configuration bit `disableNativeBroadcast`. If set then native CAN packets cannot be sent to this EFI using the broadcast ID.                                                                                                                                                                                                                                       |
| 99:5          | 12.11)canForcePacketOverCAN               | B1          | 1      | This bit echos the CAN configuration bit `forcePacketOverCAN`. If set any CAN packets sent to this EFI must use the packet-over-CAN schema, otherwise they will be ignored.                                                                                                                                                                                                                |
| 99:4          | 12.12)canCmdAndTmOnly                     | B1          | 1      | This bit echos the CAN configuration bit `cmdAndTmOnly`. If set then only commands and telemetry are supported on the CAN bus (configuration request or change will be ignored).                                                                                                                                                                                                           |
| 99:3          | 12.13)canTxFull                           | B1          | 1      | Set if the transmit buffer of the CAN interface is full.                                                                                                                                                                                                                                                                                                                                   |
| 99:2...99:0   | 12.14)canPacketFailureCnt                 | B3          | 1      | Number of packets received over the CAN bus which were invalid (bad CRC) since the last time this packet was output.                                                                                                                                                                                                                                                                       |
| 100:7         | 12.15)uartTxFull                          | B1          | 1      | Set if the transmit buffer of the UART interface is full.                                                                                                                                                                                                                                                                                                                                  |
| 100:6...100:4 | 12.16)uartPacketFailureCnt                | B3          | 1      | Number of packets received over the UART which were invalid (bad CRC) since the last time this packet was output.                                                                                                                                                                                                                                                                          |
| 100:3         | 12.17)usbTxFull                           | B1          | 1      | Set if the transmit buffer of the USB interface is full.                                                                                                                                                                                                                                                                                                                                   |
| 100:2...100:0 | 12.18)usbPacketFailureCnt                 | B3          | 1      | Number of packets received over the USB which were invalid (bad CRC) since the last time this packet was output.                                                                                                                                                                                                                                                                           |
| 101           | 12.19)txCANPacketCounter                  | U8          | 1      | Free running counter of packets transmitted over CAN (not counting this packet).                                                                                                                                                                                                                                                                                                           |
| 102           | 12.20)txPacketCounter                     | U8          | 1      | Free running counter of packets transmitted over the UART or USB (not counting this packet).                                                                                                                                                                                                                                                                                               |
| 103           | 12.21)rxPacketCounter                     | U8          | 1      | Free running counter for received packets (from any source).                                                                                                                                                                                                                                                                                                                               |
| 104...111     | 13)sensors4                                            || 1      | Fourth set of sensor information                                                                                                                                                                                                                                                                                                                                                           |
| 104:7...105:6 | 13.1)cht                                  | B10         | 1      | Average cylinder head temperature in C.<br>Scaled by 2 from -100.0 to 411.5.                                                                                                                                                                                                                                                                                                               |
| 105:5...106:4 | 13.2)cht1                                 | B10         | 1      | First cylinder head temperature in C.<br>Scaled by 2 from -100.0 to 411.5.                                                                                                                                                                                                                                                                                                                 |
| 106:3...107:2 | 13.3)cht2                                 | B10         | 1      | Second cylinder head temperature in C.<br>Scaled by 2 from -100.0 to 411.5.                                                                                                                                                                                                                                                                                                                |
| 107:1...107:0 | 13.4)[crankdirection](#efiCrankDirection) | B2          | 1      | The direction of rotation measured by the crank wheel(s) or by the once-per-rev sensors.                                                                                                                                                                                                                                                                                                   |
| 108           | 13.5)chargeTempCoef                       | U8          | 1      | Charge temperature coefficient from 0 to 1 that determines how much CHT is used in the computation of charge temperature.<br>Scaled by 200 from 0.0 to 1.275.                                                                                                                                                                                                                              |
| 109           | 13.6)_fmHeadTemp2                         | U8          | 1      | Deprecated in API 10, do not use.<br>Scaled by 100 from 0.0 to 2.55.<br>Data are given constant value on encode 1.                                                                                                                                                                                                                                                                         |
| 110...111     | 13.7)fuelp                                | F16:10      | 1      | Fuel pressure in kiloPascals.                                                                                                                                                                                                                                                                                                                                                              |
| 112...119     | 14)sdcard                                              || 1      | SD Card telemetry information                                                                                                                                                                                                                                                                                                                                                              |
| 112:7...112:5 | 14.1)[sdstatus](#efiSDStatus)             | B3          | 1      | Status of the SD card.                                                                                                                                                                                                                                                                                                                                                                     |
| 112:4...112:0 | 14.2)errorCount                           | B5          | 1      | The number of SD card errors experienced since bootup.                                                                                                                                                                                                                                                                                                                                     |
| 113...115     | 14.3)sequence                             | U24         | 1      | If `sdstatus` is `EFI_SD_READY` this is the number of 512-byte blocks that have been written to the card since the system booted up. If `sdstatus` is `EFI_SD_SETTINGUP` this gives the progress of the directory setup in tenths of a percent. If `sdstatus` is `EFI_SD_TRANSFER` this gives the progress of the transfer in tenths of a percent. In all other states this value is zero. |
| 116...119     | 14.4)numDataBlocks                        | U32         | 1      | The total number of data blocks that have been written to the card, this does not include the journal blocks.                                                                                                                                                                                                                                                                              |
| 120...127     | 15)extendedoutputs                                     || 1      | Telemetry for injector3, spark2, and spark3                                                                                                                                                                                                                                                                                                                                                |
| 120:7...121:6 | 15.1)sparkAdvance2                        | B10         | 1      | Spark advance in degrees for the second spark output.<br>Scaled by 2 from 0.0 to 511.5.                                                                                                                                                                                                                                                                                                    |
| 121:5...122:4 | 15.2)sparkAdvance3                        | B10         | 1      | Spark advance in degrees for the third spark output.<br>Scaled by 2 from 0.0 to 511.5.                                                                                                                                                                                                                                                                                                     |
| 122:3...123:2 | 15.3)injection3Angle                      | B10         | 1      | Crank angle of the third injection.<br>Scaled by 2 from 0.0 to 511.5.                                                                                                                                                                                                                                                                                                                      |
| 123:1         | 15.4)reserved                             | B1          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                               |
| 123:0...125:0 | 15.5)injector3Time                        | B17         | 1      | third injector on time for each engine cycle in microseconds. This does not include the injector trim time.<br>Scaled by 1.0 from 0.0 to 131071.0.                                                                                                                                                                                                                                         |
| 126           | 15.6)_fmChargeTemp3                       | U8          | 1      | Deprecated in API 10, do not use.<br>Scaled by 100 from 0.0 to 2.55.<br>Data are given constant value on encode 1.                                                                                                                                                                                                                                                                         |
| 127           | 15.7)_fmHeadTemp3                         | U8          | 1      | Deprecated in API 10, do not use.<br>Scaled by 100 from 0.0 to 2.55.<br>Data are given constant value on encode 1.                                                                                                                                                                                                                                                                         |
[Slow Telemetry Summary Packet packet bytes]

<div class="page-break"></div>


---


# Special Packets for IntelliJect


## <a name="EFI_PKT_RESET"></a>Reset Report

This packet reports the cause of a reset; it is spontaneously sent each time the EFI resets, on the first connection from a user. In addition the reset packet can be requested by sending this packet with zero length. The firmware will handle any unexpected exception or interrupt by immediately performing a software reset. In that case the `exceptionReset` bit will be set, and the exception field will indicate which unhandled interrupt or exception triggered the reset. The register values reported by this packet are only valid for an exception reset. For other reset causes the register values will be zero.

Resets which are not caused by power are counted in the `errorresetcnt`. If the error counter reaches 20 the software will enter a safe mode in which it does not load any settings from storage. 10 seconds after the system boots up the `errorresetcnt` is zeroed. This makes it possible to recover the system from a reset loop caused by bad settings data.

- packet identifier: `EFI_PKT_RESET` : 30
- minimum data length: 24
- maximum data length: 35

Enumeration for exceptions or interrupts on the chip. This enumeration will appear in the exception field of a reset report packet if the reset is caused by an exception or unhandled interrupted

| Name                         | Value | Description                                                                  |
| ---------------------------- | :---: | ---------------------------------------------------------------------------- |
| `EFI_EXC_NMI`                | 2     | Non maskable interrupt, Clock Security System                                |
| `EFI_EXC_HARD_FAULT`         | 3     | All class of fault                                                           |
| `EFI_EXC_MEM_MANAGE`         | 4     | Memory management                                                            |
| `EFI_EXC_BUS_FAULT`          | 5     | Pre-fetch fault, memory access fault                                         |
| `EFI_EXC_USAGE_FAULT`        | 6     | Undefined instruction or illegal state                                       |
| `EFI_EXC_SV_CALL`            | 11    | System Service call via SWI instruction                                      |
| `EFI_EXC_DEBUG_MONITOR`      | 12    | Debug Monitor                                                                |
| `EFI_EXC_PEND_SV`            | 14    | Pendable request for system service                                          |
| `EFI_EXC_SYSTICK`            | 15    | System tick timer                                                            |
| `EFI_INT_WWDG`               | 16    | Window Watchdog interrupt                                                    |
| `EFI_INT_PVD`                | 17    | PVD through EXTI line detection interrupt                                    |
| `EFI_INT_TAMP_STAMP`         | 18    | Tamper and TimeStamp interrupts through the EXTI line                        |
| `EFI_INT_RTC_WKUP`           | 19    | RTC Wakeup interrupt through the EXTI line                                   |
| `EFI_INT_FLASH`              | 20    | Flash global interrupt                                                       |
| `EFI_INT_RCC`                | 21    | RCC global interrupt                                                         |
| `EFI_INT_EXTI0`              | 22    | EXTI Line0 interrupt                                                         |
| `EFI_INT_EXTI1`              | 23    | EXTI Line1 interrupt                                                         |
| `EFI_INT_EXTI2`              | 24    | EXTI Line2 interrupt                                                         |
| `EFI_INT_EXTI3`              | 25    | EXTI Line3 interrupt                                                         |
| `EFI_INT_EXTI4`              | 26    | EXTI Line4 interrupt                                                         |
| `EFI_INT_DMA1_Stream0`       | 27    | DMA1 Stream0 global interrupt                                                |
| `EFI_INT_DMA1_Stream1`       | 28    | DMA1 Stream1 global interrupt                                                |
| `EFI_INT_DMA1_Stream2`       | 29    | DMA1 Stream2 global interrupt                                                |
| `EFI_INT_DMA1_Stream3`       | 30    | DMA1 Stream3 global interrupt                                                |
| `EFI_INT_DMA1_Stream4`       | 31    | DMA1 Stream4 global interrupt                                                |
| `EFI_INT_DMA1_Stream5`       | 32    | DMA1 Stream5 global interrupt                                                |
| `EFI_INT_DMA1_Stream6`       | 33    | DMA1 Stream6 global interrupt                                                |
| `EFI_INT_ADC`                | 34    | ADC1 global interrupt                                                        |
| `EFI_INT_CAN1_TX`            | 35    | CAN1 TX interrupt                                                            |
| `EFI_INT_CAN1_RX0`           | 36    | CAN1 RX0 interrupt                                                           |
| `EFI_INT_CAN1_RX1`           | 37    | CAN1 RX1 interrupt                                                           |
| `EFI_INT_CAN1_SCE`           | 38    | CAN1 SCE interrupt                                                           |
| `EFI_INT_EXTI9_5`            | 39    | EXTI Line[9:5] interrupts                                                    |
| `EFI_INT_TIM1_BRK_TIM9`      | 40    | TIM1 Break interrupt and TIM9 global interrupt                               |
| `EFI_INT_TIM1_UP_TIM10`      | 41    | TIM1 update interrupt and TIM10 global interrupt                             |
| `EFI_INT_TIM_TRG_COM_TIM11`  | 42    | TIM1 Trigger and Commutation interrupts and TIM11 global interrupt           |
| `EFI_INT_TIM1_CC`            | 43    | TIM1 Capture Compare interrupt                                               |
| `EFI_INT_TIM2`               | 44    | TIM2 global interrupt                                                        |
| `EFI_INT_TIM3`               | 45    | TIM3 global interrupt                                                        |
| `EFI_INT_TIM4`               | 46    | TIM4 global interrupt                                                        |
| `EFI_INT_I2C1_EVT`           | 47    | I2C1 global event interrupt                                                  |
| `EFI_INT_I2C1_ERR`           | 48    | I2C1 global error interrupt                                                  |
| `EFI_INT_I2C2_EVT`           | 49    | I2C2 global event interrupt                                                  |
| `EFI_INT_I2C2_ERR`           | 50    | I2C2 global error interrupt                                                  |
| `EFI_INT_SPI1`               | 51    | SPI1 global interrupt                                                        |
| `EFI_INT_SPI2`               | 52    | SPI2 global interrupt                                                        |
| `EFI_INT_USART1`             | 53    | USART1 global interrupt                                                      |
| `EFI_INT_USART2`             | 54    | USART2 global interrupt                                                      |
| `EFI_INT_USART3`             | 55    | USART3 global interrupt                                                      |
| `EFI_INT_EXTI15_10`          | 56    | EXTI Line[15:10] interrupts                                                  |
| `EFI_INT_EXTI17_RTC_Alarm`   | 57    | EXTI Line 17 interrupt / RTC Alarms (A and B) through EXTI line interrupt    |
| `EFI_INT_EXTI18_OTG_FS_WKUP` | 58    | EXTI Line 18 interrupt / USB On-The-Go FS Wakeup through EXTI line interrupt |
| `EFI_INT_TIM8_BRK_TIM12`     | 59    | TIM8 Break interrupt TIM12 global interrupt                                  |
| `EFI_INT_TIM8_UP_TIM13`      | 60    | TIM8 Update interrupt TIM13 global interrupt                                 |
| `EFI_INT_TIM8_TRG_COM_TIM14` | 61    | TIM8 Trigger and Commutation interrupt TIM14 global interrupt                |
| `EFI_INT_TIM8_CC`            | 62    | TIM8 Cap/Com interrupt                                                       |
| `EFI_INT_DMA1_Stream7`       | 63    | DMA1 global interrupt Channel 7                                              |
| `EFI_INT_FSMC`               | 64    | FSMC global interrupt                                                        |
| `EFI_INT_SDIO`               | 65    | SDIO global interrupt                                                        |
| `EFI_INT_TIM5`               | 66    | TIM5 global interrupt                                                        |
| `EFI_INT_SPI3`               | 67    | SPI3 global interrupt                                                        |
| `EFI_INT_TIM6`               | 70    | TIM6 global interrupt                                                        |
| `EFI_INT_TIM7`               | 71    | TIM7 global interrupt                                                        |
| `EFI_INT_DMA2_Stream0`       | 72    | DMA2 Stream0 global interrupt                                                |
| `EFI_INT_DMA2_Stream1`       | 73    | DMA2 Stream1 global interrupt                                                |
| `EFI_INT_DMA2_Stream2`       | 74    | DMA2 Stream2 global interrupt                                                |
| `EFI_INT_DMA2_Stream3`       | 75    | DMA2 Stream3 global interrupt                                                |
| `EFI_INT_DMA2_Stream4`       | 76    | DMA2 Stream4 global interrupt                                                |
| `EFI_INT_DFSDM1_FLT0`        | 77    | SD filter0 global interrupt                                                  |
| `EFI_INT_DFSDM1_FLT1`        | 78    | SD filter1 global interrupt                                                  |
| `EFI_INT_CAN2_TX`            | 79    | CAN2 TX interrupt                                                            |
| `EFI_INT_CAN2_RX0`           | 80    | BXCAN2 RX0 interrupt                                                         |
| `EFI_INT_CAN2_RX1`           | 81    | BXCAN2 RX1 interrupt                                                         |
| `EFI_INT_CAN2_SCE`           | 82    | CAN2 SCE interrupt                                                           |
| `EFI_INT_OTG_FS`             | 83    | USB On The Go FS global interrupt                                            |
| `EFI_INT_DMA2_Stream5`       | 84    | DMA2 Stream5 global interrupt                                                |
| `EFI_INT_DMA2_Stream6`       | 85    | DMA2 Stream6 global interrupt                                                |
| `EFI_INT_DMA2_Stream7`       | 86    | DMA2 Stream7 global interrupt                                                |
| `EFI_INT_USART6`             | 87    | USART6 global interrupt                                                      |
| `EFI_INT_I2C3_EV`            | 88    | I2C3 event interrupt                                                         |
| `EFI_INT_I2C3_ER`            | 89    | I2C3 error interrupt                                                         |
| `EFI_INT_RNG`                | 96    | RNG global interrupt                                                         |
| `EFI_INT_FPU`                | 97    | FPU global interrupt                                                         |
| `EFI_INT_SPI4`               | 98    | SPI4 global interrupt                                                        |
| `EFI_INT_SPI5`               | 99    | SPI5 global interrupt                                                        |
| `EFI_INT_Quad_SPI`           | 108   | Quad SPI global interrupt                                                    |
| `EFI_INT_I2CFMP1_EV`         | 109   | I2CFMP1 event interrupt                                                      |
| `EFI_INT_I2CFMP1_ER`         | 110   | I2CFMP1 error interrupt                                                      |
[<a name="efiExceptionId"></a>Exception ID enumeration]





| Bytes     | Name                    | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                                                                                                            |
| --------- | ----------------------- | :---------: | :----: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 0:7       | 1)lowPowerReset         | B1          | 1      | Reset caused by the low power management.                                                                                                                                                                                                                                                                                                                                                              |
| 0:6       | 2)windowWatchdogReset   | B1          | 1      | Reset caused by the window watchdog.                                                                                                                                                                                                                                                                                                                                                                   |
| 0:5       | 3)watchdogReset         | B1          | 1      | Reset caused by the independent watchdog.                                                                                                                                                                                                                                                                                                                                                              |
| 0:4       | 4)softwareReset         | B1          | 1      | Reset caused by software.                                                                                                                                                                                                                                                                                                                                                                              |
| 0:3       | 5)powerReset            | B1          | 1      | Reset caused by power on.                                                                                                                                                                                                                                                                                                                                                                              |
| 0:2       | 6)pinReset              | B1          | 1      | Reset caused by the reset pin.                                                                                                                                                                                                                                                                                                                                                                         |
| 0:1       | 7)borReset              | B1          | 1      | Brown out reset.                                                                                                                                                                                                                                                                                                                                                                                       |
| 0:0       | 8)exceptionReset        | B1          | 1      | Reset caused by unhandled exception or interrupt.                                                                                                                                                                                                                                                                                                                                                      |
| 1         | 9)exception             | U8          | 1      | The number of the interrupt or exception which caused this reset, valid only if `exceptionReset` is set, otherwise zero.                                                                                                                                                                                                                                                                               |
| 2:7       | 10)first                | B1          | 1      | If set, this is the first report generated since the system reset. Subsequent requests for the reset report will have this bit clear.                                                                                                                                                                                                                                                                  |
| 2:6       | 11)safeModeActive       | B1          | 1      | If set the system booted in safe mode because `errorresetcnt` was too large. No settings data were loaded from non-volatile storage in this mode. The only way to exit safe mode is to reset the system.                                                                                                                                                                                               |
| 2:5       | 12)quickRestartActive   | B1          | 1      | If set the system booted in quick restart mode because the engine was running when the reset occurred.                                                                                                                                                                                                                                                                                                 |
| 2:4       | 13)specialWatchdogReset | B1          | 1      | Reset caused by special watchdog handling. This is an exception that fires before the true watchdog in an attempt to learn the program counter of the stuck code.                                                                                                                                                                                                                                      |
| 2:3...2:0 | 14)reserved             | B4          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                                                                           |
| 3         | 15)count                | U8          | 1      | Volatile reset count information. Each time the system boots up, it increments this counter. If the system resets by losing power long enough to clear the RAM then this counter will reset to zero. After the counter reaches 255 it will roll over to zero.                                                                                                                                          |
| 4...7     | 16)programCounter       | U32         | 1      | The program counter of the instruction that triggered the exception.                                                                                                                                                                                                                                                                                                                                   |
| 8...11    | 17)linkRegister         | U32         | 1      | The link register at the time of the exception.                                                                                                                                                                                                                                                                                                                                                        |
| 12...15   | 18)xpsr                 | U32         | 1      | The program status register at the time of the exception.                                                                                                                                                                                                                                                                                                                                              |
| 16...19   | 19)fpscr                | U32         | 1      | The floating point status and control register at the time of the exception.                                                                                                                                                                                                                                                                                                                           |
| 20...23   | 20)cfsr                 | U32         | 1      | The configurable fault status register at the time of the exception.                                                                                                                                                                                                                                                                                                                                   |
| 24...27   | 21)mmfar                | U32         | 1      | The memory management fault address register at the time of the exception.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                                                                                                                                                         |
| 28...31   | 22)bfar                 | U32         | 1      | The bus fault address register at the time of the exception.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                                                                                                                                                                       |
| 32        | 23)errorresetcnt        | U8          | 1      | The error reset count is incremented once for each time the system resets due to an error which is not power related. If the error counter reaches 20 the software will enter a safe mode in which it does not load any settings from storage. 10 seconds after the system boots up the `errorresetcnt` is zeroed.<br>This field is optional. If it is not included then the value is assumed to be 0. |
| 33...34   | 24)stickyresetcount     | U16         | 1      | Non-volatile reset count information. Each time the system boots up, it increments this counter.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                                                                                                                                   |
[Reset Report packet bytes]


## <a name="EFI_PKT_NACK"></a>Not Acknowledge Packet

This packet is sent by the EFI to indicate a not-acknowledge response. The nack always occurs in reponse to a packet from the user. There are six different nack cases: 1) The input packet was not recognized. 2) The input packet attempted to change locked configuration data, 3) The input packet attempted to read hidden configuration data, 4) The input packet attempted a command that cannot be performed while the engine is running, or 5) The input packet was `EFI_PKT_REQUEST` but one of the request bits was reserved, or 6) The input packet required the system be enabled when it was disabled, or vice versa.

- packet identifier: `EFI_PKT_NACK` : 27
- minimum data length: 2
- maximum data length: 4

Enumeration describing the cause of the nack

| Name                      | Value | Description                                                                   |
| ------------------------- | :---: | ----------------------------------------------------------------------------- |
| `nackPktNotRecognized`    | 0     | Nack due to unrecognized input packet.                                        |
| `nackDataAreLocked`       | 1     | Nack due to attempt to change locked data.                                    |
| `nackDataAreHidden`       | 2     | Nack due to attempt to read hidden configuration data.                        |
| `nackRunningIncorrect`    | 3     | Nack due to engine running when it needs to be off, or vice versa.            |
| `nackReqBitNotRecognized` | 4     | Nack due to use of a reserved bit in the request packet.                      |
| `nackLogIsFull`           | 5     | Nack due to log being full.                                                   |
| `nackEnableIncorrect`     | 6     | Nack due to system being disabled when it needs to be enabled, or vice versa. |
[<a name="efiNackCause"></a>Causes of a Nack enumeration]





| Bytes     | Name                     | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                                              |
| --------- | ------------------------ | :---------: | :----: | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0         | 1)type                   | U8          | 1      | The type of the packet that triggered the NACK response.                                                                                                                                                                                                                                                                                 |
| 1:7...1:5 | 2)[cause](#efiNackCause) | B3          | 1      | The reason for the nack response.                                                                                                                                                                                                                                                                                                        |
| 1:4...1:0 | 3)index                  | B5          | 1      | The index information from the first data byte of the packet that triggered the NACK response. This is only relevant for those packets that have indices.                                                                                                                                                                                |
| 2         | 4)requestbit             | U8          | 1      | If the cause of the nack was `nackReqBitNotRecognized` this field gives the bit number, from 95 (most significant) to 0 (least), of the request bit that was not understood.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                         |
| 3:7...3:6 | 5)reserved               | B2          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                             |
| 3:5...3:0 | 6)index                  | B6          | 1      | The index information from the first data byte of the packet that triggered the NACK response. This is only relevant for those packets that have indices.<br>This field is optional. If it is not included then the value is assumed to be 0.<br>This field overrides the previous field of the same name, if the packet is long enough. |
[Not Acknowledge Packet packet bytes]


## <a name="EFI_PKT_OSCILLOSCOPE_SETUP"></a>Oscilloscope setup packet

This packet is used to configure and enable the high speed oscilloscope. Triggering scope mode when a previous scope capture is underway will cause the previous capture to be discarded. If you ask for continuous scope output, but lack the neccessary communications bandwidth, the scope output will be intermittent. A single scope capture may take multiple packets to completely report. Send this packet with zero length to request the current oscilloscope setup.

- packet identifier: `EFI_PKT_OSCILLOSCOPE_SETUP` : 41
- data length: 12

Enumeration for scope signals in the EFI. The first signals match exactly the enumerations of the EFI sensors.

| Name                     | Value                       | Description                                                                             |
| ------------------------ | :-------------------------: | --------------------------------------------------------------------------------------- |
| Input Voltage            | EFI_SCOPE_FIRSTANALOG       | Input voltage sensor, units of Volts                                                    |
| MAT                      | EFI_SCOPE_FIRSTANALOG + 1   | Manifold air temperature sensor, units of Celsius                                       |
| CHT1                     | EFI_SCOPE_FIRSTANALOG + 2   | First cylinder head temperature sensor, units of Celsius                                |
| CHT2                     | EFI_SCOPE_FIRSTANALOG + 3   | Second cylinder head temperature sensor, units of Celsius                               |
| MAP                      | EFI_SCOPE_FIRSTANALOG + 4   | Manifold pressure sensor, units of kilo-Pascals                                         |
| Fuel pressure            | EFI_SCOPE_FIRSTANALOG + 5   | Fuel pressure sensor, units of kilo-Pascals                                             |
| Analog TPS               | EFI_SCOPE_FIRSTANALOG + 6   | Analog throttle sensor, units of percent                                                |
| CPU Temp                 | EFI_SCOPE_FIRSTANALOG + 7   | Temperature of the CPU, units of Celsius                                                |
| Spare Temp               | EFI_SCOPE_FIRSTANALOG + 8   | Spare temperature sensor, units of Celsius                                              |
| Reserved                 | EFI_SCOPE_FIRSTANALOG + 9   |                                                                                         |
| Reserved                 | EFI_SCOPE_FIRSTANALOG + 10  |                                                                                         |
| Current                  | EFI_SCOPE_FIRSTANALOG + 11  | Input current sensor, units of Amps                                                     |
| 12 Volts                 | EFI_SCOPE_FIRSTANALOG + 12  | 12 Volt rail voltage sensor, units of Volts                                             |
| 12 Current               | EFI_SCOPE_FIRSTANALOG + 13  | 12 Volt rail current sensor, units of Amps                                              |
| Barometer                | EFI_SCOPE_FIRSTANALOG + 14  | Analog barometric pressure sensor, units of kilo-Pascals                                |
| Reserved                 | EFI_SCOPE_FIRSTANALOG + 15  |                                                                                         |
| IO enable                | EFI_SCOPE_FIRSTDIGITAL      | IO enable scope signal                                                                  |
| Crank sense 1            | EFI_SCOPE_FIRSTDIGITAL + 1  | First crank sense scope signal                                                          |
| Crank sense 2            | EFI_SCOPE_FIRSTDIGITAL + 2  | Second crank sense scope signal                                                         |
| Throttle out             | EFI_SCOPE_FIRSTDIGITAL + 3  | Throttle PWM output scope signal                                                        |
| Throttle in              | EFI_SCOPE_FIRSTDIGITAL + 4  | Throttle PWM input scope signal                                                         |
| Cooling out              | EFI_SCOPE_FIRSTDIGITAL + 5  | Cooling PWM output scope signal                                                         |
| Edge error               | EFI_SCOPE_FIRSTDIGITAL + 6  | An output edge error was detected                                                       |
| Spark 1                  | EFI_SCOPE_FIRSTDIGITAL + 7  | First spark output scope signal                                                         |
| Spark 2                  | EFI_SCOPE_FIRSTDIGITAL + 8  | Second spark output scope signal                                                        |
| Spark 3                  | EFI_SCOPE_FIRSTDIGITAL + 9  | Third spark output scope signal                                                         |
| Injector 1               | EFI_SCOPE_FIRSTDIGITAL + 10 | First injector output scope signal                                                      |
| Injector 2               | EFI_SCOPE_FIRSTDIGITAL + 11 | Second injector output scope signal                                                     |
| Injector 3               | EFI_SCOPE_FIRSTDIGITAL + 12 | Third injector output scope signal                                                      |
| Crank 1 error            | EFI_SCOPE_FIRSTDIGITAL + 13 | Error flag on first crank sense signal                                                  |
| Crank 2 error            | EFI_SCOPE_FIRSTDIGITAL + 14 | Error flag on second crank sense signal                                                 |
| Asynchronous             | EFI_SCOPE_FIRSTDIGITAL + 15 | Flag on whether this sample was asynchronous (bit is set) or synchronous (bit is clear) |
[<a name="efiScopeEnum"></a>EFI oscilloscope signal List enumeration]





| Bytes     | Name                              | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                                                               |
| --------- | --------------------------------- | :---------: | :----: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7       | 1)enable                          | B1          | 1      | Set to enable the oscilloscope, for packets going to the EFI. For packets from the EFI this reports the running status of the scope.                                                                                                                                                                                                                      |
| 0:6       | 2)single                          | B1          | 1      | Set for a single oscilloscope reporting window. If this is set the oscilloscope will automatically disable after a single capture.                                                                                                                                                                                                                        |
| 0:5...1:6 | 3)reserved                        | B8          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                              |
| 1:5...3:0 | 4)timewindow                      | B22         | 1      | Time window the scope will report in microseconds.                                                                                                                                                                                                                                                                                                        |
| 4         | 5)reserved                        | U8          | 1      | Reserved bytes in the packet.                                                                                                                                                                                                                                                                                                                             |
| 5:7       | 6)triggerenable                   | B1          | 1      | Set to enable triggering, if clear then the scope is always triggered (i.e. always generating data).                                                                                                                                                                                                                                                      |
| 5:6       | 7)triggerfalling                  | B1          | 1      | Set to trigger on the falling edge, else the trigger is on the rising edge.                                                                                                                                                                                                                                                                               |
| 5:5...5:0 | 8)[triggerchannel](#efiScopeEnum) | B6          | 1      | Scope channel used to trigger oscilloscope output, if `triggerenable` is set.                                                                                                                                                                                                                                                                             |
| 6...7     | 9)triggerlevel                    | U16         | 1      | Level of the trigger channel that causes an oscilloscope trigger. For analog sensor signals this in raw ADC units, with 0 the lowest value from the ADC and 65535 the largest value from the ADC. For digital signals this is either 0 or 1.                                                                                                              |
| 8...11    | 10)channels                       | U32         | 1      | A bitfield flag indicating which channels should be captured for the scope. The channel numbers follow `efiScopeEnum` enumeration, with channel zero being the least significant bit. Digital channels (`EFI_SCOPE_IOENABLE` and higher) are always reported in the data packet, but will not generate data packets unless they are set in this bitfield. |
[Oscilloscope setup packet packet bytes]


## <a name="EFI_PKT_OSCILLOSCOPE_DATA"></a>Oscilloscope data packet

This packet is used to report the oscilliscope data. If the scope is setup for continuous output, but the communications hardware lacks the neccessary bandwidth, the scope output will be intermittent. A single scope capture will usually take multiple packets to completely report; however the exact number of packets is not known until the scope capture is complete. The EFI will begin sending scope data as soon as possible, and the transfer is complete when the `lastPacket` flag is set.

The oscilloscope data packet can contain multiple samples of data. Since the number of signals in each sample depends on the oscilloscope configuration the data are encoded as a repeating array of 16-bit words. The first word is always the time offset, which gives the time of the sample in microseconds, relative to the `reftime` of the packet. The second word of the sample is always the digital sample word, with the least significant bit corresponding to the `EFI_SCOPE_IOENABLE` digital channel. The remaining words in the sample provide the analog scope signals; the number of analog signals are given by `numAnalogPerSample`, which will equal the number of analog channels that were selected in the oscilloscope configuration packet. The total number of words in the packet will always be a multiple of `2 + numAnalogPerSample`. The analog signals are transmitted in their raw 16-bit ADC form. You must use the sensor configurations to convert the sensors to engineering units data.

- packet identifier: `EFI_PKT_OSCILLOSCOPE_DATA` : 42
- minimum data length: 10
- maximum data length: 1016

Enumeration for data words.

| Name                         | Value | Description                                                   |
| ---------------------------- | :---: | ------------------------------------------------------------- |
| `EFI_SCOPE_WORD_TIME`        | 0     | The first word in a series is the time offset in microseconds |
| `EFI_SCOPE_WORD_DIGITALS`    | 1     | The second word in a series is the digital sample field       |
| `EFI_SCOPE_WORD_FIRSTANALOG` | 2     | The third word in a series is the first analog sample field   |
| `EFI_SCOPE_MAX_WORDS`        | 500   | A maximum of 500 words in a packet                            |
[<a name="efiScopeDataWords"></a>EFI oscilloscope data word list enumeration]





| Bytes     | Name                  | [Enc](#Enc) | Repeat                                                    | Description                                                                                                                                            |
| --------- | --------------------- | :---------: | :-------------------------------------------------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 0:7       | 1)lastPacket          | B1          | 1                                                         | If set this packet is the last packet in a series of scope packets that report a single scope window of data.                                          |
| 0:6       | 2)triggerTimeIncluded | B1          | 1                                                         | If set this packet contains the time reference of the triggering event.                                                                                |
| 0:5...0:4 | 3)reserved            | B2          | 1                                                         | Reserved bits in the packet.                                                                                                                           |
| 0:3...1:0 | 4)packetCount         | B12         | 1                                                         | The number of this packet in the series of scope packets that report a single scope window of data. This value will be zero on the first scope packet. |
| 2:7...2:3 | 5)numAnalogPerSample  | B5          | 1                                                         | The number of analog channels that are included per sample in this packet.                                                                             |
| 2:2...2:1 | 6)reserved            | B2          | 1                                                         | Reserved bits in the packet.                                                                                                                           |
| 2:0...3:0 | 7)numWords            | B9          | 1                                                         | Number of words in this packet. The number of samples in the packet is equal to `numWords / (2 + numAnalogPerSample)`.                                 |
| 4...9     | 8)refTime             | U48         | 1                                                         | Time in microseconds since the system booted up. This is the reference time used for subsequent samples in the packet.                                 |
| 10...15   | 9)triggerTime         | U48         | 1                                                         | Time in microseconds since the system booted up when the trigger event occurred.<br>Only included if triggerTimeIncluded is non-zero.                  |
| 16...1015 | 10)words              | U16         | numWords, up to [EFI_SCOPE_MAX_WORDS](#efiScopeDataWords) | The words of the samples. The interpretation of the sample words depends on the oscilloscope configuration.                                            |
[Oscilloscope data packet packet bytes]


## <a name="EFI_PKT_DEBUG"></a>Debug packet

This packet gives data used for debugging. The interpretation of this packet is variable.

- packet identifier: `EFI_PKT_DEBUG` : 43
- minimum data length: 17
- maximum data length: 528


| Bytes    | Name      | [Enc](#Enc)                            | Repeat | Description                                       |
| -------- | --------- | :------------------------------------: | :----: | ------------------------------------------------- |
| 0...3    | 1)time    | U32                                    | 1      | Time in microseconds of the debug output.         |
| 4...15   | 2)details | U8                                     | 12     | 12 bytes of data to use as the debugger sees fit. |
| 16...527 | 3)comment | Zero-terminated string up to 512 bytes         || Human readable debug string.                      |
[Debug packet bytes]


## <a name="EFI_PKT_TESTMODE"></a>TestMode packet

The test mode packet is used to engage special features for testing software and hardware. In order to engage test mode the EFI must be unlocked, and the engine must not be running. If the engine starts running while test mode is active (as indicated by crank sense detection) the test mode will be automatically canceled. Test mode can also be canceled by this packet, by locking the EFI configuration, or by timeout. When test mode is active this packet will be reported at the slow telemetry rate to indicate the test mode status. The test mode status is always visible in the telemetry errors packet. In addition you can send this packet with zero length to request the current test mode status.

- packet identifier: `EFI_PKT_TESTMODE` : 44
- minimum data length: 13
- maximum data length: 16


| Bytes       | Name                                | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                                                                                                                                                        |
| ----------- | ----------------------------------- | :---------: | :----: | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7         | 1)enabled                           | B1          | 1      | Set to enable test mode. For report packets this is the test mode status.                                                                                                                                                                                                                                                                          |
| 0:6         | 2)enableThrottleClosedCalibration   | B1          | 1      | Set to enable automatic calibration of the throttle `closedPWMout` value.                                                                                                                                                                                                                                                                          |
| 0:5         | 3)enableThrottleOpenCalibration     | B1          | 1      | Set to enable automatic calibration of the throttle `openPWMout` value.                                                                                                                                                                                                                                                                            |
| 0:4         | 4)skipCrankSense1                   | B1          | 1      | Set to skip the processing of one event cycle for crank sense 1.                                                                                                                                                                                                                                                                                   |
| 0:3         | 5)skipCrankSense2                   | B1          | 1      | Set to skip the processing of one event cycle for crank sense 2.                                                                                                                                                                                                                                                                                   |
| 0:2         | 6)addToothCrankSense1               | B1          | 1      | Set to add an extraneous sensed tooth for crank sense 1.                                                                                                                                                                                                                                                                                           |
| 0:1         | 7)addToothCrankSense2               | B1          | 1      | Set to add an extraneous sensed tooth for crank sense 1.                                                                                                                                                                                                                                                                                           |
| 0:0...1:0   | 8)timeout                           | B9          | 1      | Test mode timeout in seconds. For report packets this is the amount of time remaining in the test. If the test is user storage this is percent remaining.                                                                                                                                                                                          |
| 2:7         | 9)testCrankSenseNormal              | B1          | 1      | Set to simulate crank sense events in the normal direction.                                                                                                                                                                                                                                                                                        |
| 2:6         | 10)testInjector1                    | B1          | 1      | Set for injector1 test.                                                                                                                                                                                                                                                                                                                            |
| 2:5         | 11)testInjector2                    | B1          | 1      | Set for injector2 test.                                                                                                                                                                                                                                                                                                                            |
| 2:4         | 12)testSpark1                       | B1          | 1      | Set for spark1 test.                                                                                                                                                                                                                                                                                                                               |
| 2:3         | 13)testSpark2                       | B1          | 1      | Set for spark2 test.                                                                                                                                                                                                                                                                                                                               |
| 2:2         | 14)testFuelpump                     | B1          | 1      | Set to drive the fuel pump at a specific duty cycle.                                                                                                                                                                                                                                                                                               |
| 2:1         | 15)testFuelflow                     | B1          | 1      | Set to control the injector duty cycle to achieve a fuel pressure. This should be combined with testInjector1 or testInjector2 to specify which injector is used to regulate the fuel pressure.                                                                                                                                                    |
| 2:0         | 16)testCrankSenseReverse            | B1          | 1      | Set to simulate crank sense events in the reverse direction.                                                                                                                                                                                                                                                                                       |
| 3:7...3:6   | 17)missFire1                        | B2          | 1      | Number of skipped outputs for spark1; 0 - 3.                                                                                                                                                                                                                                                                                                       |
| 3:5...3:4   | 18)missFire2                        | B2          | 1      | Number of skipped outputs for spark2; 0 - 3.                                                                                                                                                                                                                                                                                                       |
| 3:3         | 19)enableCowlFlap2ClosedCalibration | B1          | 1      | Set to enable automatic calibration of the cowl flap 2 `closedPWM` value.                                                                                                                                                                                                                                                                          |
| 3:2         | 20)enableCowlFlap2OpenCalibration   | B1          | 1      | Set to enable automatic calibration of the cowl flap 2 `openPWM` value.                                                                                                                                                                                                                                                                            |
| 3:1         | 21)enableCowlFlap1ClosedCalibration | B1          | 1      | Set to enable automatic calibration of the cowl flap 1 `closedPWM` value.                                                                                                                                                                                                                                                                          |
| 3:0         | 22)enableCowlFlap1OpenCalibration   | B1          | 1      | Set to enable automatic calibration of the cowl flap 1 `openPWM` value.                                                                                                                                                                                                                                                                            |
| 4...5       | 23)dutycycle                        | U16         | 1      | For injector, spark, and fuel pump tests this is duty cycle in percentage to apply to the injector output, spark output, or fuel pump output.<br>Scaled by 65535/(100) from 0.0 to 100.0.                                                                                                                                                          |
| 6...7       | 24)speed                            | U16         | 1      | The frequency, in events per minute, at which the test should be performed. This does not apply to the fuel pump test.<br>Scaled by 4 from 0.0 to 16383.75.                                                                                                                                                                                        |
| 8...9       | 25)fuelflowProGain                  | F16:9       | 1      | Proportional feedback gain for fuel flow test from kPa of fuel pressure error to percentage of injector duty cycle.                                                                                                                                                                                                                                |
| 10...11     | 26)fuelflowIntGain                  | F16:9       | 1      | Integral feedback gain for fuel flow test from integral of kPa of fuel pressure error to percentage of injector duty cycle.                                                                                                                                                                                                                        |
| 12          | 27)currentThreshold                 | U8          | 1      | The current rise used to indicate that a hard stop was discovered, used with `enableThrottleClosedCalibration`, `enableThrottleOpenCalibration`, `enableCowlFlapClosedCalibration`, and `enableCowlFlapOpenCalibration`.<br>Scaled by 200 from 0.0 to 1.275.<br>This field is optional. If it is not included then the value is assumed to be 0.1. |
| 13          | 28)actualClosedStop                 | U8          | 1      | The actual position, in percent, of the closed hard stop used with `enableThrottleClosedCalibration` or `enableCowlFlapClosedCalibration`.<br>Scaled by 4 from -30.0 to 33.75.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                 |
| 14          | 29)actualOpenStop                   | U8          | 1      | The actual position, in percent, of the open hard stop used with `enableThrottleOpenCalibration` or `enableCowlFlapOpenCalibration`.<br>Scaled by 4 from 70.0 to 133.75.<br>This field is optional. If it is not included then the value is assumed to be 100.                                                                                     |
| 15:7        | 30)testInjector3                    | B1          | 1      | Set for injector3 test.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                                                                                                                                                        |
| 15:6        | 31)testSpark3                       | B1          | 1      | Set for spark3 test.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                                                                                                                                                           |
| 15:5...15:4 | 32)missFire3                        | B2          | 1      | Number of skipped outputs for spark3; 0 - 3.<br>This field is optional. If it is not included then the value is assumed to be 0.                                                                                                                                                                                                                   |
| 15:3        | 33)testUserStorage                  | B1          | 1      | Set to enable test of user storage space. This will exercise the entire EEPROM. Do not disonnect from power until the test is complete, otherwise some data in user stroage may be lost. This test cannot be done simultaneously with other tests.<br>This field is optional. If it is not included then the value is assumed to be 0.             |
| 15:2...15:0 | 34)reserved                         | B3          | 1      | Reserved bits in the packet.                                                                                                                                                                                                                                                                                                                       |
[TestMode packet bytes]


## <a name="EFI_PKT_CRANKSENSETIMING"></a>RequestCrankSenseTiming packet

Request crank sense timing packets. To request the crank sense timing set either `onesample` or `continuous`. To stop the crank sense timing packets clear `continuous`

- packet identifier: `EFI_PKT_CRANKSENSETIMING` : 53
- data length: 2


| Bytes     | Name           | [Enc](#Enc) | Repeat | Description                                                                                                                                                                                                     |
| --------- | -------------- | :---------: | :----: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0:7       | 1)onesample    | B1          | 1      | Set this bit to request one capture of data.                                                                                                                                                                    |
| 0:6       | 2)continuous   | B1          | 1      | Set this bit to request crank sense timing packets be sent continuously.                                                                                                                                        |
| 0:5       | 3)onsynclost   | B1          | 1      | Set this bit to downlink the timing when synchronization is lost.                                                                                                                                               |
| 0:4       | 4)sense2       | B1          | 1      | Set this bit to request crank sense timing for sense 2, otherwise the data are requested for sense 1.                                                                                                           |
| 0:3...0:1 | 5)reserved     | B3          | 1      | Reserved bits for future expansion.                                                                                                                                                                             |
| 0:0...1:0 | 6)numIntervals | B9          | 1      | The requested number of sense intervals. For crank wheels IntelliJect will adjust this to be an even multiple of the number of physical teeth (totalCount - bigGapCount - smallGapCount). Maximum value is 500. |
[RequestCrankSenseTiming packet bytes]


## <a name="EFI_PKT_CRANKSENSETIMING"></a>CrankSenseTiming packet

Details of the crank sense timing. To receive this packet you must first send the `RequestCrankSenseTiming` packet. The crank sense timing details simply give the time intervals of crank sense events (active edges). This is most useful for crank wheels, but can be used for once-per-rev sensors.

- packet identifier: `EFI_PKT_CRANKSENSETIMING` : 53
- minimum data length: 8
- maximum data length: 1012

Enumerations for the crank sense timing packet

| Name                     | Value | Description             |
| ------------------------ | :---: | ----------------------- |
| `maxCrankSenseIntervals` | 500   | Maximum sense intervals |
[<a name="CrankSenseTimingConstants"></a>CrankSenseTimingConstants enumeration]





| Bytes     | Name             | [Enc](#Enc) | Repeat                                                                   | Description                                                                                                                                                                                                                    |
| --------- | ---------------- | :---------: | :----------------------------------------------------------------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 0:7       | 1)onesample      | B1          | 1                                                                        | If set a single crank sense timing packet was sent.                                                                                                                                                                            |
| 0:6       | 2)continuous     | B1          | 1                                                                        | If set crank sense timing packets will be sent continuously.                                                                                                                                                                   |
| 0:5       | 3)onsynclost     | B1          | 1                                                                        | Set this bit to downlink the timing when synchronization is lost.                                                                                                                                                              |
| 0:4       | 4)sense2         | B1          | 1                                                                        | If set this timing data comes from crank sense 2, otherwise it is from crank sense 1.                                                                                                                                          |
| 0:3...0:2 | 5)reserved       | B2          | 1                                                                        | Reserved bits for future expansion.                                                                                                                                                                                            |
| 0:1       | 6)wheelenabled   | B1          | 1                                                                        | Set if this timing data are for a crank wheel, else it is for a once per rev sensor.                                                                                                                                           |
| 0:0...1:0 | 7)numIntervals   | B9          | 1                                                                        | The number of sense intervals in this packet. For crank wheels this will be a multiple of the number of physical teeth (totalCount - bigGapCount - smallGapCount). Maximum value is 500.                                       |
| 2...3     | 8)crankOffset    | U16         | 1                                                                        | Angle of the crank sense in degrees of crank rotation after TDC, in the current crank rotation direction. For crank wheels this is the angle of the first tooth after the big gap.<br>Scaled by 65535/(360) from 0.0 to 360.0. |
| 4         | 9)totalCount     | U8          | 1                                                                        | The number of teeth on the crank wheel plus the missing teeth. This is the full count, as though no synchronization teeth were removed.<br>Only included if wheelenabled is non-zero.                                          |
| 5         | 10)bigGapCount   | U8          | 1                                                                        | The number of teeth missing in the big gap of the crank wheel. The end of the big gap is the crank angle synchronization point. There must always be a non-zero big gap count.<br>Only included if wheelenabled is non-zero.   |
| 6         | 11)smallGapCount | U8          | 1                                                                        | The number of teeth missing in the small gap of the crank wheel. The small gap count can be zero, in which case there is no small gap, and no intra-gap.<br>Only included if wheelenabled is non-zero.                         |
| 7         | 12)intraGapCount | U8          | 1                                                                        | The number of teeth between the big gap and the small gap for the current direction of rotation. Zero if there is no small gap.<br>Only included if wheelenabled is non-zero.                                                  |
| 8...11    | 13)time          | U32         | 1                                                                        | Time in milliseconds since the system booted up. This is the reference time for subsequent intervals.                                                                                                                          |
| 12...1011 | 14)intervals     | U16         | numIntervals, up to [maxCrankSenseIntervals](#CrankSenseTimingConstants) | The time difference of each sense event from the previous event in microseconds.                                                                                                                                               |
[CrankSenseTiming packet bytes]


## <a name="EFI_PKT_BOOTLOADER"></a>Boot Loader Packet

Packet for bootloader interactions. The bootloader is a separate application within the EFI firmware load; and only the bootloader will send, or respond to, this packet. The bootloader uses fixed communications configurations. On serial the bit rate is always 57600 bits per second. On CAN the bit rate is always 1Mbit, the identifier is always 11-bits, the input ID is always 0x400, and the output ID is always 0x401. Only one device on the CAN bus can be in bootloader mode at a time.

When the bootloader is running it will output this packet on a regular basis as a heartbeat. In order to program the firmware you must watch for the bootloader packet and use the serial number that it contains in any packet sent back to the bootloader. In order to start the bootloader you use the [reset command](#EFI_PKT_RESET) packet.

- packet identifier: `EFI_PKT_BOOTLOADER` : 37
- minimum data length: 14
- maximum data length: 270

Enumeration for the bootloader commands

| Name              | Value | Description                                                 |
| ----------------- | :---: | ----------------------------------------------------------- |
| `btldHeartBeat`   | 0     | Heart beat packet from the bootloader, sent once per second |
| `btldEraseCmd`    | 1     | Erase command to the bootloader                             |
| `btldEraseAck`    | 2     | Erase acknowledgement from the bootloader                   |
| `btldEraseNack`   | 3     | Erase failure notification from the bootloader              |
| `btldProgramCmd`  | 4     | Program command to the bootloader                           |
| `btldProgramAck`  | 5     | Program acknowledgement from the bootloader                 |
| `btldProgramNack` | 6     | Program failure notification from the bootloader            |
| `btldReset`       | 7     | Command to reset to the main application                    |
[<a name="bootloadercmds"></a>Commands understood by the bootloader enumeration]




Settings used for bootloader communication

| Name              | Value | Description                                         |
| ----------------- | :---: | --------------------------------------------------- |
| `btldUartBaud`    | 57600 | Bit rate used for the uart under the bootloader     |
| `btldCanBaud`     | 3     | Bit rate used for the CAN under the bootloader      |
| `btldCanInputId`  | 1024  | Identifier used for CAN input under the bootloader  |
| `btldCanOutputId` | 1025  | Identifier used for CAN output under the bootloader |
[<a name="bootloadercomms"></a>Bootloader communications settings enumeration]





| Bytes       | Name                         | [Enc](#Enc) | Repeat              | Description                                                                                                                                                                                                                                               |
| ----------- | ---------------------------- | :---------: | :-----------------: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0...3       | 1)serialnumber               | U32         | 1                   | 32-bit serial number of the EFI. When the bootloader packet comes from the EFI this contains the serial number of the EFI. When the bootloader packet goes to the EFI this must have the correct EFI serial number in order for this packet to be obeyed. |
| 4...7       | 2)startaddress               | U32         | 1                   | Starting address for this packet. This is used for erase as well as program. For the heartbeat packet this will be address `0x0801001C`, and the datsize will be at least 16 bytes.                                                                       |
| 8...11      | 3)datasize                   | U32         | 1                   | Data size for this packet. This is used for erase as well as program. For the program packet this datasize cannot be more than 256. For the program nack packet this indicates the first byte which failed to program.                                    |
| 12          | 4)version                    | U8          | 1                   | Version of the bootloader from 0 to 255.                                                                                                                                                                                                                  |
| 13:7        | 5)hasprogramdata             | B1          | 1                   | Set if this packet includes program data.                                                                                                                                                                                                                 |
| 13:6...13:4 | 6)reserved                   | B3          | 1                   | Reserved bits for future expansion.                                                                                                                                                                                                                       |
| 13:3...13:0 | 7)[command](#bootloadercmds) | B4          | 1                   | The purpose of this bootloader packet.                                                                                                                                                                                                                    |
| 14...269    | 8)programdata                | U8          | datasize, up to 256 | Program data, starting at startaddress.<br>Only included if hasprogramdata is non-zero.                                                                                                                                                                   |
[Boot Loader Packet packet bytes]

<div class="page-break"></div>


----------------------------

# About this ICD

This is the interface control document for data *on the wire*, not data in memory. This document was automatically generated by the [ProtoGen software](https://github.com/billvaglienti/ProtoGen), version 3.1.d. ProtoGen also generates C source code for doing most of the work of encoding data from memory to the wire, and vice versa.

## Encodings

Data for this protocol are sent in BIG endian format. Any field larger than one byte is sent with the most signficant byte first, and the least significant byte last.

Data can be encoded as unsigned integers, signed integers (two's complement), bitfields, and floating point.

| <a name="Enc"></a>Encoding | Interpretation                        | Notes                                                                       |
| :--------------------------: | ------------------------------------- | --------------------------------------------------------------------------- |
| UX                           | Unsigned integer X bits long          | X must be: 8, 16, 24, 32, 40, 48, 56, or 64                                 |
| IX                           | Signed integer X bits long            | X must be: 8, 16, 24, 32, 40, 48, 56, or 64                                 |
| BX                           | Unsigned integer bitfield X bits long | X must be greater than 0 and less than 32                                   |
| F16:X                        | 16 bit float with X significand bits  | 1 sign bit : 15-X exponent bits : X significant bits with implied leading 1 |
| F24:X                        | 24 bit float with X significand bits  | 1 sign bit : 23-X exponent bits : X significant bits with implied leading 1 |
| F32                          | 32 bit float (IEEE-754)               | 1 sign bit : 8 exponent bits : 23 significant bits with implied leading 1   |
| F64                          | 64 bit float (IEEE-754)               | 1 sign bit : 11 exponent bits : 52 significant bits with implied leading 1  |

## Size of fields
The encoding tables give the bytes for each field as X...Y; where X is the first byte (counting from 0) and Y is the last byte. For example a 4 byte field at the beginning of a packet will give 0...3. If the field is 1 byte long then only the starting byte is given. Bitfields are more complex, they are displayed as Byte:Bit. A 3-bit bitfield at the beginning of a packet will give 0:7...0:5, indicating that the bitfield uses bits 7, 6, and 5 of byte 0. Note that the most signficant bit of a byte is 7, and the least signficant bit is 0. If the bitfield is 1 bit long then only the starting bit is given.

The byte count in the encoding tables are based on the maximum length of the field(s). If a field is variable length then the actual byte location of the data may be different depending on the value of the variable field. If the field is a variable length character string this will be indicated in the encoding column of the table. If the field is a variable length array this will be indicated in the repeat column of the encoding table. If the field depends on the non-zero value of another field this will be indicated in the description column of the table.

