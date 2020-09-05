// eficommsProtocol.h was generated by ProtoGen version 3.1.d

#ifndef _EFICOMMSPROTOCOL_H
#define _EFICOMMSPROTOCOL_H

// Language target is C, C++ compilers: don't mangle us
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 * \mainpage eficomms protocol stack
 * 
 * This ICD describes the communications of the IntelliJect electronic fuel
 * injection (EFI) system from [Power4Flight](http://www.power4flight.com/).
 * IntelliJect communicates using packets of data transported over asyncrhonous
 * serial, USB, Controller Area Network (CAN), or over a network connection.
 * The IntelliJect standard developers kit (SDK) includes C source code for
 * implementing packet encoding and decoding routines.
 * 
 * The protocol API enumeration is incremented anytime the protocol is changed
 * in a way that affects compatibility with earlier versions of the protocol.
 * The protocol enumeration for this version is: 10
 */

#include <stdint.h>

//! \return the protocol API enumeration
#define geteficommsApi() 10

// Translation provided externally. The macro takes a `const char *` and returns a `const char *`
#ifndef translateeficomms
    #define translateeficomms(x) x
#endif

/*!
 * The list of packet type identifiers.
 */
typedef enum
{
    EFI_PKT_CANTRANSPORT,                    //!< Identifier for CAN transport of packets with more than 8 bytes of data (packet-over-CAN schema)
    EFI_PKT_PUMPCONFIGURATION,               //!< Fuel pump configuration information
    EFI_PKT_COMMCONFIGURATION,               //!< Communications configuration information
    EFI_PKT_SENSORCONFIGURATION,             //!< Sensor configuration information for a single sensor
    EFI_PKT_THROTTLECONFIGURATION,           //!< Throttle configuration information
    EFI_PKT_RESERVED,                        
    EFI_PKT_INJECTORCONFIGURATION,           //!< Injector configuration information
    EFI_PKT_RPMCONTROLLER,                   //!< RPM Controller configuration
    EFI_PKT_ENGINEWEAR,                      //!< Engine wear information
    EFI_PKT_TABLE,                           //!< Table settings for a single table
    EFI_PKT_TABLEVALUE,                      //!< Table single value
    EFI_PKT_RESERVED1,                       
    EFI_PKT_SOFTWAREINFO,                    //!< Software version information
    EFI_PKT_STICKY_ERRORS,                   //!< Sticky error information
    EFI_PKT_ENABLE,                          //!< User enables
    EFI_PKT_USERTHROTTLE,                    //!< Throttle command
    EFI_PKT_RPMCOMMAND,                      //!< RPM command
    EFI_PKT_REQUEST,                         //!< Request for multiple configuration packets
    EFI_PKT_TELEMETRYTIME,                   //!< Telemetry time information
    EFI_PKT_TELEMETRYFAST,                   //!< Fast telemetry
    EFI_PKT_TELEMETRYSENSORS,                //!< Telemetry information for sensors
    EFI_PKT_TELEMETRYSENSORS2,               //!< Telemetry2 information for sensors
    EFI_PKT_TELEMETRYFUEL,                   //!< Fueling information
    EFI_PKT_TELEMETRYSLOW,                   //!< Miscellanious slow telemetry
    EFI_PKT_TELEMETRYCPU,                    //!< Telemetry about the CPU performance
    EFI_PKT_TELEMETRYINJECTOR,               //!< Telemetry about the injectors
    EFI_PKT_TELEMETRYERRORS,                 //!< Dynamic error information
    EFI_PKT_NACK,                            //!< Packet not-acknowledge
    EFI_PKT_UNLOCK,                          //!< Configuration unlock
    EFI_PKT_LOCKCONTROL,                     //!< Configuration locking for the system
    EFI_PKT_RESET,                           //!< Report or command a reset of the system
    EFI_PKT_FILE,                            //!< File name for the settings file
    EFI_PKT_COMMENT,                         //!< Configuration comment
    EFI_PKT_STORAGE,                         //!< Storage configuration command
    EFI_PKT_HARDWAREINFO,                    //!< Information about the hardware
    EFI_PKT_TELEMETRYFASTSUM,                //!< A super packet containing all the fast telemetry
    EFI_PKT_TELEMETRYSLOWSUM,                //!< A super packet containing all the slow telemetry
    EFI_PKT_BOOTLOADER,                      //!< Bootloader control packet
    EFI_PKT_TELEMETRYSENSORS3,               //!< Telemetry3 information for sensors
    EFI_PKT_CONFIGHASH,                      //!< Send this packet to flag the current configuration as valid
    EFI_PKT_TELEMETRYCOMMS,                  //!< Telemetry about communications performance and errors
    EFI_PKT_OSCILLOSCOPE_SETUP,              //!< Oscilloscope setup packet
    EFI_PKT_OSCILLOSCOPE_DATA,               //!< Oscilloscope data packet
    EFI_PKT_DEBUG,                           //!< Packet used for debug
    EFI_PKT_TESTMODE,                        //!< Packet used for testing
    EFI_PKT_TELEMETRYSDCARD,                 //!< Telemetry about the SD card
    EFI_PKT_FUELUSED,                        //!< Packet used to set or report the fuel used
    EFI_PKT_TELEMETRYAUTOFUEL,               //!< Packet used for reporting autofuel results
    EFI_PKT_TELEMETRYSENSORS4,               //!< Telemetry4 information for sensors
    EFI_PKT_ENGINEWEAREXT,                   //!< Extended engine wear information
    EFI_PKT_MAINTENANCECONFIGURATION,        //!< Maintenance schedule configuration
    EFI_PKT_MAINTENANCESTATUS,               //!< Maintenance status
    EFI_PKT_LOGENTRY,                        //!< Engine log entry
    EFI_PKT_CRANKSENSETIMING,                //!< Crank wheel timing details
    EFI_PKT_SDCARDJOURNAL,                   //!< SD Card journal information
    EFI_PKT_INTERRUPTDETAILS,                //!< Detailed information about interrupts
    EFI_PKT_COOLINGCONFIGURATION,            //!< Cooling configuration information
    EFI_PKT_NEWENGINECONFIGURATION,          //!< Engine configuration information for api 7 and later
    EFI_PKT_TELEMETRYGCU,                    //!< Telemetry information for the generator control unit
    EFI_PKT_TELEMETRYEXTENDEDOUTPUTS,        //!< Telemetry for injector3, spark2, and spark3
    EFI_PKT_TELEMETRYOILINJ,                 //!< Telemetry for the oil injection system
    EFI_PKT_MAXID = EFI_PKT_TELEMETRYOILINJ, //!< Maximum packet ID to or from an EFI using this version of the protocol
    EFI_PKT_SDBLOCKDATA = 253,               //!< This packet appears at the start of each block (except the journal) in SD cards
    EFI_PKT_SENSORAUTOOFFSET = 254,          //!< Packet for storing sensor offsets for auto-corrected sensors
    EFI_PKT_QUICKRESTART = 255               //!< Packet for immediate spark and inject needed to keep an engine running through a system reset
} efiPacketIds;

//! \return the label of a 'efiPacketIds' enum entry, based on its value
const char* efiPacketIds_EnumLabel(int value);

//! \return the title of a 'efiPacketIds' enum entry, based on its value
const char* efiPacketIds_EnumTitle(int value);

/*!
 * Different levels of locking/unlocking
 */
typedef enum
{
    EFI_LCK_LOCKED,          //!< EFI configuration is locked, unlock must be issued to change anything
    EFI_LCK_PARTIAL_UNLOCKED,//!< EFI configuration partially unlocked, some data may still be protected
    EFI_LCK_PASSWORD_UNLOCKED //!< EFI configuration is fully unlocked, all data are unprotected
} efiunlocklevels;

/*!
 * States of crank wheel synchronization
 */
typedef enum
{
    noCrankWheelSync,    //!< No syncrhonization information is available
    onegapCrankWheelSync,//!< One of the gaps has been discovered, no direction information
    twogapCrankWheelSync,//!< Both gaps have been discovered (or only one is configured)
    fullCrankWheelSync   //!< Big gap confirmed at the correct location
} efiCrankWheelSyncStatus;

/*!
 * States of crank direction
 */
typedef enum
{
    noCrankDirection,    //!< Crank direction is unknown or cannot be known (single gap)
    normalCrankDirection,//!< Crank direction is normal rotation
    reverseCrankDirection //!< Crank direction is reverse rotation
} efiCrankDirection;

/*!
 * Triggers for maintenance items
 */
typedef enum
{
    maintenanceTriggerTime,      //!< Maintenance is triggered by engine run time
    maintenanceTriggerRevs,      //!< Maintenance is triggered by number of revolutions
    maintenanceTriggerHotTime,   //!< Maintenance is triggered by engine time above CHT limit
    maintenanceTriggerHighLoadTime,//!< Maintenance is triggered by engine time above the high load threshold
    maintenanceTriggerStarts,    //!< Maintenance is triggered by number of engine starts
    maintenanceTriggerCHT,       //!< Maintenance is triggered by cylinder head temperature
    numMaintenanceTriggers       
} MaintenanceTriggers;

//! \return the title of a 'MaintenanceTriggers' enum entry, based on its value
const char* MaintenanceTriggers_EnumTitle(int value);

//! \return the comment of a 'MaintenanceTriggers' enum entry, based on its value
const char* MaintenanceTriggers_EnumComment(int value);

/*!
 * Maximum number of maintenance items allowed in the schedule.
 */
typedef enum
{
    maxMaintenanceItems = 14 //!< Maximum number of maintenance items allowed in the schedule.
} MaxMaintenanceItems;

/*!
 * Maximum number of characters in a user name.
 */
typedef enum
{
    maxNameCharacters = 20 //!< Maximum number of characters in a user name.
} MaxNameCharacters;

/*!
 * List of tables used in the EFI.
 */
typedef enum
{
    EFI_TABLE_FUEL,                      //!< Fuel as micro-grams per revolution per cc of displacement.
    EFI_TABLE_SPARK1ADV,                 //!< Spark1 advance in degrees.
    EFI_TABLE_SPARK2DELAY,               //!< Delay of spark2 after spark1 in degrees.
    EFI_TABLE_CHARGETEMP,                //!< Fraction of CHT (versus MAT) to use in the determination of the charge temperature.
    EFI_TABLE_FM1,                       //!< First fuel multiplier.
    EFI_TABLE_INJECTOR1PHASE,            //!< First injector angle in degrees.
    EFI_TABLE_INJECTOR1TRIM,             //!< First injector start time in microseconds.
    EFI_TABLE_MINTHROTTLE,               //!< First minimum throttle in percent.
    EFI_TABLE_MAXTHROTTLE,               //!< First maximum throttle in percent.
    EFI_TABLE_SA1,                       //!< First spark adder in degrees.
    EFI_TABLE_FM2,                       //!< Second fuel multiplier.
    EFI_TABLE_FM3,                       //!< Third fuel multiplier.
    EFI_TABLE_MAP,                       //!< Ratio of manifold to baro pressure (for failed map sensor).
    EFI_TABLE_SHAFTPOWER,                //!< Shaft power in Watts on standard day.
    EFI_TABLE_INJECTOR2RATIO,            //!< Percentage of injector 2 versus injectors 1 and 3 in providing the total fuel flow.
    EFI_TABLE_INJECTOR2PHASE,            //!< Second injector angle in degrees.
    EFI_TABLE_INJECTOR2TRIM,             //!< Second injector start time in microseconds.
    EFI_TABLE_FM4,                       //!< Fourth fuel multiplier.
    EFI_TABLE_IGNDWELL,                  //!< Microseconds of time the ignition output is inactive between sparks.
    EFI_TABLE_SA2,                       //!< Second spark adder in degrees.
    EFI_TABLE_FM5,                       //!< Fifth fuel multiplier.
    EFI_TABLE_INJECTOR3RATIO,            //!< Percentage of injector 3 versus injectors 1 and 2 in providing the total fuel flow.
    EFI_TABLE_INJECTOR3PHASE,            //!< Third injector angle in degrees.
    EFI_TABLE_INJECTOR3TRIM,             //!< Third injector start time in microseconds.
    EFI_TABLE_SPARK3DELAY,               //!< Delay of spark3 after spark1 in degrees.
    EFI_TABLE_MINTHROTTLE2,              //!< Second minimum throttle in percent.
    EFI_TABLE_MAXTHROTTLE2,              //!< Second maximum throttle in percent.
    EFI_TABLE_FM6,                       //!< Sixth fuel multiplier.
    EFI_TABLE_FM7,                       //!< Seventh fuel multiplier.
    EFI_TABLE_OILINJ_RATIO,              //!< Desired ratio of fuel to oil.
    EFI_TABLE_OILINJ_DUTY,               //!< Percentage duty cycle of the oil injection pump.
    EFI_TABLE_OILINJ_OUTPUT,             //!< Output of the oil injection pump per pulse in micro-grams of oil.
    EFI_TABLE_SA3,                       //!< Third spark adder in degrees.
    EFI_TABLE_USER1,                     //!< First user table.
    EFI_TABLE_USER2,                     //!< Second user table.
    EFI_TABLE_USER3,                     //!< Third user table.
    EFI_TABLE_USER4,                     //!< Fourth user table.
    EFI_TABLE_MAXINDEX = EFI_TABLE_USER4 //!< Maximum table index.
} efiTableList;

//! \return the title of a 'efiTableList' enum entry, based on its value
const char* efiTableList_EnumTitle(int value);

//! \return the comment of a 'efiTableList' enum entry, based on its value
const char* efiTableList_EnumComment(int value);

/*!
 * Size of the table list
 */
typedef enum
{
    NUM_EFI_TABLES = EFI_TABLE_MAXINDEX+1 //!< The number of 2D tables in the EFI.
} efiTableListSize;

/*!
 * List of tables used in the 1.x version of the EFI
 */
typedef enum
{
    EFI_OLDTABLE_FUEL,                               //!< Fuel as micro-grams per revolution per cc of displacement, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_SPARK1ADV,                          //!< Spark1 advance in degrees, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_SPARK2DELAY,                        //!< Delay of spark2 after spark1 in degrees, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_CHARGETEMP,                         //!< Fuel modifier for difference between head and inlet temperature: multiplier(k) = (k(CHTref - MAT) + MATa) / (k(CHT - MAT) + MATa), indexed by [throttle/load, rpm]
    EFI_OLDTABLE_HEADTEMP,                           //!< Fuel modifier for head temperature: multiplier(k) = (k + 1), indexed by [throttle/load, CHT]
    EFI_OLDTABLE_INJECTOR1PHASE,                     //!< First injector angle in degrees, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_INJECTOR1TRIM,                      //!< First injector start time in microseconds, indexed by [pressure, volts]
    EFI_OLDTABLE_MINTHROTTLE,                        //!< Minimum throttle, indexed by [CHT cold, rpm]
    EFI_OLDTABLE_MAXTHROTTLE,                        //!< Maximum throttle, indexed by [CHT hot, rpm]
    EFI_OLDTABLE_SPARKRETARD,                        //!< Spark retard in degrees, indexed by [throttle/load, CHT hot]
    EFI_OLDTABLE_DOWNTRANSIENT,                      //!< Fuel modifier for negative throttle transient in throttle percent per second: multiplier(k) = (k(dthrottle/dt) + 1), indexed by [throttle/load, rpm]
    EFI_OLDTABLE_2NDDENSITY_FUEL,                    //!< Fuel modifier for second order density ratio: multiplier(k) = (1 + k(1.225 - density)/1.225), indexed by [throttle/load, rpm]
    EFI_OLDTABLE_MAP,                                //!< Ratio of manifold to baro pressure (for failed map sensor), indexed by [throttle, rpm]
    EFI_OLDTABLE_SHAFTPOWER,                         //!< Shaft power in Watts on standard day, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_INJECTOR2RATIO,                     //!< Percentage of injector 2 versus injectors 1 and 3 in providing the total fuel flow, indexed by [throttle/load, rpm].
    EFI_OLDTABLE_INJECTOR2PHASE,                     //!< Second injector angle in degrees, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_INJECTOR2TRIM,                      //!< Second injector start time in microseconds, indexed by [pressure, volts]
    EFI_OLDTABLE_STARTFUEL,                          //!< Fuel multiplier used for starting, indexed by [CHT, numrevs]
    EFI_OLDTABLE_IGNDWELL,                           //!< Microseconds of time the ignition output is inactive between sparks, indexed by [volts, rpm]
    EFI_OLDTABLE_DENSITY_SPARK,                      //!< Spark advance adder for density ratio: adder(k) = k(1.225 - density)/1.225, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_UPTRANSIENT,                        //!< Fuel modifier for positive throttle transient in throttle percent per second: multiplier(k) = (k(dthrottle/dt) + 1), indexed by [throttle/load, rpm]
    EFI_OLDTABLE_INJECTOR3RATIO,                     //!< Percentage of injector 3 versus injectors 1 and 2 in providing the total fuel flow, indexed by [throttle/load, rpm].
    EFI_OLDTABLE_INJECTOR3PHASE,                     //!< Third injector angle in degrees, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_INJECTOR3TRIM,                      //!< Third injector start time in microseconds, indexed by [pressure, volts]
    EFI_OLDTABLE_SPARK3DELAY,                        //!< Delay of spark3 after spark1 in degrees, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_MINTHROTTLE2,                       //!< Second minimum throttle, indexed by [CHT cold, density ratio]
    EFI_OLDTABLE_MAXTHROTTLE2,                       //!< Second maximum throttle, indexed by [CHT hot, density ratio]
    EFI_OLDTABLE_FUELMULT,                           //!< Fuel multiplier: multiplier(k) = (k + 1), indexed by [throttle/load, rpm]
    EFI_OLDTABLE_MAT,                                //!< Fuel modifier for manifold temperature: multiplier(k) = (k + 1), indexed by [throttle/load, MAT]
    EFI_OLDTABLE_OILINJ_RATIO,                       //!< Desired ratio of fuel to oil, indexed by [throttle/load, rpm]
    EFI_OLDTABLE_OILINJ_DUTY,                        //!< Duty cycle of the oil injection pump, indexed by [oil temperature, volts]
    EFI_OLDTABLE_OILINJ_OUTPUT,                      //!< Output of the oil injection pump per pulse, indexed by [oil temperature, duty cycle]
    EFI_OLDTABLE_MAXINDEX = EFI_OLDTABLE_OILINJ_OUTPUT //!< Maximum old table index
} efiOldTableList;

/*!
 * Size of the old table list
 */
typedef enum
{
    NUM_EFI_OLDTABLES = EFI_OLDTABLE_MAXINDEX+1  //!< The number of 2D old tables in the 1.x version of the EFI.
} efiOldTableListSize;

/*!
 * The maximum size of an EFI table
 */
typedef enum
{
    EFI_TABLE_MAX_ROWS = 21, //!< Maximum number of rows in an EFI table
    EFI_TABLE_MAX_COLS = 15  //!< Maximum number of cols in an EFI table
} efiTableSize;

/*!
 * List of variables that can be used as row or column indexes in a table, or
 * used as multipliers or dividers of table output
 */
typedef enum
{
    EFI_TBLIDX_NULL,                     //!< The value of 1.0.
    EFI_TBLIDX_THROTTLE,                 //!< Position of the throttle in percent.
    EFI_TBLIDX_LOAD,                     //!< Percent ratio of manifold pressure to barometric pressure.
    EFI_TBLIDX_RPM,                      //!< Engine speed in revolutions per minute.
    EFI_TBLIDX_BARO_RATIO,               //!< Ratio of barometric air pressure to standard day pressure.
    EFI_TBLIDX_MAP_RATIO,                //!< Ratio of manifold air pressure to standard day pressure.
    EFI_TBLIDX_CHARGE_TEMP_RATIO,        //!< Ratio of absolute charge temperature to absolute charge temperature reference.
    EFI_TBLIDX_MAT_RATIO,                //!< Ratio of manifold air absolute temperature to standard day absolute temperature.
    EFI_TBLIDX_DENSITY_RATIO,            //!< Ratio of air density to standard day density.
    EFI_TBLIDX_ONEMINUS_DENSITY_RATIO,   //!< 1.0 - the ratio of air density to standard day density.
    EFI_TBLIDX_MAT,                      //!< Average manifold air temperature.
    EFI_TBLIDX_CHARGE_TEMP,              //!< Charge temperature.
    EFI_TBLIDX_CHT,                      //!< Average cylinder head temperature.
    EFI_TBLIDX_CHTCOLD,                  //!< Coldest cylinder head temperature.
    EFI_TBLIDX_CHTHOT,                   //!< Hottest cylinder head temperature.
    EFI_TBLIDX_MAP,                      //!< Manifold air pressure.
    EFI_TBLIDX_BARO,                     //!< Barometric air pressure.
    EFI_TBLIDX_FUELP,                    //!< Augmented fuel pressure (fuelp + baro - map).
    EFI_TBLIDX_FUELP_RAW,                //!< Gauge fuel pressure.
    EFI_TBLIDX_VOLTAGE,                  //!< Voltage of the injector rail.
    EFI_TBLIDX_REVS,                     //!< Engine revolutions since start.
    EFI_TBLIDX_TIME,                     //!< Elapsed time since engine start.
    EFI_TBLIDX_THROTTLE_RATE,            //!< Rate of change of the throttle position.
    EFI_TBLIDX_POS_THROTTLE_RATE,        //!< Rate of change of the throttle position, if the throttle rate is positive, else zero.
    EFI_TBLIDX_NEG_THROTTLE_RATE,        //!< Rate of change of the throttle position, if the throttle rate is negative, else zero.
    EFI_TBLIDX_RPM_RATE,                 //!< Rate of change of the engine speed.
    EFI_TBLIDX_POS_RPM_RATE,             //!< Rate of change of the engine speed, if the rpm rate is positive, else zero.
    EFI_TBLIDX_NEG_RPM_RATE,             //!< Rate of change of the engine speed, if the rpm rate is negative, else zero.
    EFI_TBLIDX_OIL_TEMP,                 //!< Oil temperature.
    EFI_TBLIDX_OIL_DUTY,                 //!< Duty cycle of the oil injection pump.
    EFI_TBLIDX_OIL_ON_TIME,              //!< The amount of time the oil injection pump is on per cycle.
    EFI_TBLIDX_USER1,                    //!< Output of the user 1 table
    EFI_TBLIDX_USER2,                    //!< Output of the user 2 table
    EFI_TBLIDX_USER3,                    //!< Output of the user 3 table
    EFI_TBLIDX_USER4,                    //!< Output of the user 4 table
    EFI_TBLIDX_MAXINDEX = EFI_TBLIDX_USER4 //!< Maximum table variable index
} efiTableIndex;

//! \return the title of a 'efiTableIndex' enum entry, based on its value
const char* efiTableIndex_EnumTitle(int value);

//! \return the comment of a 'efiTableIndex' enum entry, based on its value
const char* efiTableIndex_EnumComment(int value);

/*!
 * Size of the index list
 */
typedef enum
{
    NUM_EFI_TBLIDXS = EFI_TBLIDX_MAXINDEX+1  //!< The number variables in the indice list.
} efiTableIndexSize;

/*!
 * Source of the throttle information, for throttle command or throttle
 * position sensor
 */
typedef enum
{
    EFI_THROTTLE_START,  //!< Throttle command comes form starting throttle setting; throttle position sense is unknown.
    EFI_THROTTLE_CMD,    //!< Throttle command comes from user command; throttle position sense is assumed to match output.
    EFI_THROTTLE_PWMIN,  //!< Throttle (command or sense) comes from PWM input
    EFI_THROTTLE_ANALOGIN,//!< Throttle (command or sense) comes from analog input
    EFI_THROTTLE_RPMDIRECT,//!< Throttle command comes from the RPM governor with a direct user command
    EFI_THROTTLE_MAP,    //!< Throttle position sense comes from manifold pressure estimate
    EFI_THROTTLE_CAN,    //!< Throttle position sense comes from CAN servo
    NUM_EFI_THROTTLES    
} efiThrottleSource;

/*!
 * SD Card status
 */
typedef enum
{
    EFI_SD_HARDWARE_BAD, //!< The SD card has not been detected; it may be missing, or the hardware may be bad.
    EFI_SD_MBR_BAD,      //!< The SD card does not have a valid boot sector, it needs formatting.
    EFI_SD_VOLUME_BAD,   //!< The SD card does not have a valid volume sector, it needs formatting.
    EFI_SD_ROOT_BAD,     //!< The SD card does not have the expected root directory layout, it needs directory setup.
    EFI_SD_FORMATTING,   //!< The SD card is being formatted. This status is always followed by `EFI_SD_SETTINGUP`.
    EFI_SD_SETTINGUP,    //!< The SD card root directory is being setup.
    EFI_SD_READY,        //!< The SD card is ready to record.
    EFI_SD_TRANSFER      //!< The SD card is reporting recorded data.
} efiSDStatus;

//! \return the title of a 'efiSDStatus' enum entry, based on its value
const char* efiSDStatus_EnumTitle(int value);

/*!
 * Values used in the definition of the generic packet
 */
typedef enum
{
    EFI_PACKET_SYNC0 = 0x57,     //!< First synchronization byte for a EFI packet
    EFI_PACKET_SYNC0EX = 0x75,   //!< First synchronization byte for a special EFI packet (sdcard record)
    EFI_PACKET_SYNC1 = 0xAC,     //!< Second synchronization byte for a EFI packet
    EFI_PACKET_SYNC1EX = 0xC8,   //!< Second synchronization byte for a special EFI packet (uplink recording)
    EFI_PACKET_SYNC1MASK = 0xEC, //!< Mask to apply to second synchronization byte to remove size bits
    EFI_PACKET_SIZE = 1408,      //!< An EFI packet can transport 1408 bytes
    EFI_PACKET_HEADER = 4,       //!< An EFI packet has 4 bytes for the header
    EFI_PACKET_OVERHEAD = 8,     //!< An EFI packet has 8 bytes of overhead
    EFI_PACKET_TYPEBITS = 6      //!< The number of bits for the type field of an EFI packet
} PacketIds;

/*!
 * General structure of an IntelliJect EFI packet
 */
typedef struct
{
    uint8_t  sync0;                   //!< First synchronization byte to indicate a packet may be forthcoming
    uint8_t  sync1;                   //!< Second synchronization byte, and upper 3 bits of size
    uint8_t  sizeLo;                  //!< Lower 8 bits of packet size
    uint8_t  type;                    //!< 6 bit packet type, upper 2 bits reserved
    uint8_t  data[EFI_PACKET_SIZE+4]; //!< Packet data space, plus 4 bytes for crc32
    uint16_t rxstate;                 //!< Receive state for processing a receive packet byte by byte, not part of the transmission
}efiPacket_t;


// The prototypes below provide an interface to the packets.
// They are not auto-generated functions, but must be hand-written

//! \return the packet data pointer from the packet
uint8_t* geteficommsPacketData(efiPacket_t* pkt);

//! \return the packet data pointer from the packet, const
const uint8_t* geteficommsPacketDataConst(const efiPacket_t* pkt);

//! Complete a packet after the data have been encoded
void finisheficommsPacket(efiPacket_t* pkt, int size, uint32_t packetID);

//! \return the size of a packet from the packet header
int geteficommsPacketSize(const efiPacket_t* pkt);

//! \return the ID of a packet from the packet header
uint32_t geteficommsPacketID(const efiPacket_t* pkt);

#ifdef __cplusplus
}
#endif
#endif // _EFICOMMSPROTOCOL_H