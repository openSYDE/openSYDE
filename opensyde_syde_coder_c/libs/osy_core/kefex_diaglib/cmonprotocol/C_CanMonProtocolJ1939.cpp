//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN message to text conversion class for the J1939 protocol

   Converts a CAN message structure to text. The interpretation is based on
   the J1939 CAN protocol specification.
   Implemented version: SAE J1939/71 REV June 2006

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanMonProtocolJ1939.hpp"
#include "C_SclString.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::cmon_protocol;
using namespace stw::scl;
using namespace stw::can;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
/* PDU formats, PDU1 (0..0xEF), PDU2 (0xF0..0xFF) */
#define J1939_PDU1_MAX    (static_cast<uint8_t>(0xEFU))

/* PGNs */
#define J1939_MAX_PGN     (259U)

/* Request */
#define J1939_PGN_REQUEST                    (static_cast<uint16_t>(0xEA00U)) // Request

/* Acknowledgement */
#define J1939_PGN_ACK                        (static_cast<uint16_t>(0xE800U)) // ACK/NACK PGN
#define J1939_ACK_POS                        (static_cast<uint8_t>(0U))       // Positive Acknowledgement
#define J1939_ACK_NEG                        (static_cast<uint8_t>(1U))       // Negative Acknowledgement
#define J1939_ACK_ACCESS_DENIED              (static_cast<uint8_t>(2U))       // Access denied
#define J1939_ACK_CANNOT_RESPOND             (static_cast<uint8_t>(3U))       // Cannot respond

/* Address Claim */
#define J1939_PGN_ADDRESS_CLAIMED            (static_cast<uint16_t>(0xEE00U)) // Address Claim Response

/* Multi-Packet Broadcast/Peer-to-Peer */
#define J1939_PGN_TP_CM                      (static_cast<uint16_t>(0xEC00U)) // TP.CM Connection Management
#define J1939_PGN_TP_DT                      (static_cast<uint16_t>(0xEB00U)) // TP.DT Data Transfer
#define J1939_TP_CM_RTS                      (static_cast<uint8_t>(16U))      // TP.CM Ready to Send
#define J1939_TP_CM_CTS                      (static_cast<uint8_t>(17U))      // TP.CM Clear to Send
#define J1939_TP_CM_EOM_ACK                  (static_cast<uint8_t>(19U))      // TP.CM End of message acknowledgement
#define J1939_TP_CM_CONN_ABORT               (static_cast<uint8_t>(0xFFU))    // TP.CM Connection Abort
#define J1939_TP_CM_BAM                      (static_cast<uint8_t>(32U))      // TP.CM Broadcast Announce Message
#define J1939_TP_CM_ABORT_AS_BUSY            (static_cast<uint8_t>(1U))       // TP.CM Connection Abort Reason 1
#define J1939_TP_CM_ABORT_AS_RESOURCE_LACK   (static_cast<uint8_t>(2U))       // TP.CM Connection Abort Reason 2
#define J1939_TP_CM_ABORT_AS_TIMEOUT         (static_cast<uint8_t>(3U))       // TP.CM Connection Abort Reason 3

/* Destination Addresses */
#define J1939_DA_BROADCAST                   (static_cast<uint8_t>(0xFFU))    // Destination address for Broadcasts
#define J1939_DA_MASK                        (static_cast<uint16_t>(0xFF00U)) // Mask destination address

/* Source Addresses */
#define J1939_SA_NULL                        (static_cast<uint8_t>(0xFEU)) // NULL address (used by addressless nodes)

/* -- Types --------------------------------------------------------------------------------------------------------- */

typedef struct
{
   uint16_t u16_Value;    // PGN value, e.g. 0x0000)
   C_SclString c_Brief;   // PGN brief text, e.g. TSC1
   C_SclString c_Verbose; // PGN verbose text, e.g. Torque/Speed Control 1
}T_J1939_PGN;

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
const T_J1939_PGN mat_Pgn[J1939_MAX_PGN] =
{
   {0x0000U, "TSC1",     "Torque/Speed Control 1"},
   {0x0100U, "TC1",      "Transmission Control 1"},
   {0x0400U, "XBR",      "External Brake Request"},
   {0xAE00U, "TPRS",     "Tire Pressure Reference Setting"},
   {0xCF00U, "CTL",      "Continuous Torque & Speed Limit Request"},
   {0xD000U, "CL",       "Cab Illumination Message"},
   {0xD100U, "ASC6",     "Air Suspension Control 6"},
   {0xD200U, "ASC2",     "Air Suspension Control 2"},
   {0xD500U, "TDA",      "Time/Date Adjust"},
   {0xDC00U, "ATS",      "Anti-theft Status"},
   {0xDD00U, "ATR",      "Anti-theft Request"},
   {0xDE00U, "RESET",    "Reset"},
   {0xE000U, "CM1",      "Cab Message 1"},
   {0xF000U, "ERC1",     "Electronic Retarder Controller"},
   {0xF001U, "EBC1",     "Electronic Brake Controller 1"},
   {0xF002U, "ETC1",     "Electronic Transmission Controller 1"},
   {0xF003U, "EEC2",     "Electronic Engine Controller 2"},
   {0xF004U, "EEC1",     "Electronic Engine Controller 1"},
   {0xF005U, "ETC2",     "Electronic Transmission Controller 2"},
   {0xF006U, "EAC1",     "Electronic Axle Controller 1"},
   {0xF007U, "FLI1",     "Forward Lane Image urgent msg"},
   {0xF008U, "HPG",      "Hydraulic Pressure Governor Info"},
   {0xF009U, "VDC2",     "Vehicle Dynamic Stability Control 2"},
   {0xF00AU, "EGF1",     "Engine Gas Flow Rate"},
   {0xF00BU, "ESC1",     "Electronic Steering Control"},
   {0xF00CU, "ETC8",     "Electronic Transmission Controller 8"},
   {0xF00DU, "LOI",      "Land Levelling System Operational Information"},
   {0xF00EU, "AT1I1",    "Aftertreatment 1 Intake Gas 1"},
   {0xF00FU, "AT1O1",    "Aftertreatment 1 Outlet Gas 1"},
   {0xF010U, "AT2I1",    "Aftertreatment 2 Intake Gas 1"},
   {0xF011U, "AT2O1",    "Aftertreatment 2 Outlet Gas 1"},
   {0xF012U, "FWSS1",    "Fifth Wheel Smart Systems 1"},
   {0xF013U, "SSI",      "Slope Sensor Information"},
   {0xF014U, "BI",       "Blade Information"},
   {0xF016U, "CCS",      "Cylinder Combustion Status"},
   {0xF017U, "KL1",      "Engine Knock Level 1"},
   {0xF018U, "KL2",      "Engine Knock Level 2"},
   {0xF019U, "KL3",      "Engine Knock Level 3"},
   {0xF01AU, "TFAC",     "Engine Throttle / Fuel Actuator Control Command"},
   {0xFD98U, "AHI1",     "Aftertreatment 1 Historical Information"},
   {0xFD99U, "AHI2",     "Aftertreatment 2 Historical Information"},
   {0xFD9BU, "CRI1",     "Catalyst Reagent Information"},
   {0xFD9CU, "SEP2",     "Sensor Electrical Power 2"},
   {0xFD9DU, "SEP1",     "Sensor Electrical Power 1"},
   {0xFD9EU, "AAC2",     "Aftertreatment 2 Air Control"},
   {0xFD9FU, "AAC1",     "Aftertreatment 1 Air Control"},
   {0xFDA0U, "AFC2",     "Aftertreatment 2 Fuel Control"},
   {0xFDA1U, "AFC1",     "Aftertreatment 1 Fuel Control"},
   {0xFDA2U, "GFI3",     "Fuel Information 3 (Gaseous)"},
   {0xFDA3U, "EEC6",     "Electronic Engine Controller 3"},
   {0xFDA4U, "PTODE",    "PTO Drive Engagement"},
   {0xFDA5U, "DC2",      "Door Control 2"},
   {0xFDA8U, "WCM2",     "Wireless Communications Message 2"},
   {0xFDA9U, "WCM1",     "Wireless Communications Message 1"},
   {0xFDAAU, "EFLP4",    "Engine Fluid Level/Pressure 4"},
   {0xFDAEU, "FWSS2",    "Fifth Wheel Smart Systems 2"},
   {0xFDAFU, "ATM2",     "Aftertreatment 2 Intermediate Gas"},
   {0xFDB0U, "AT2O2",    "Aftertreatment 2 Outlet Gas 2"},
   {0xFDB1U, "AT2I2",    "Aftertreatment 2 Intake Gas 2"},
   {0xFDB2U, "ATM1",     "Aftertreatment 1 Intermediate Gas"},
   {0xFDB3U, "AT1O2",    "Aftertreatment 1 Outlet Gas 2"},
   {0xFDB4U, "AT1I2",    "Aftertreatment 1 Intake Gas 2"},
   {0xFDB9U, "TPRI",     "Tire Pressure Reference Information"},
   {0xFDBAU, "TR6",      "Farebox Status"},
   {0xFDBBU, "TR5",      "Farebox Point of Sale"},
   {0xFDBCU, "TR4",      "Farebox Service Detail"},
   {0xFDBDU, "TR3",      "Signal Preemption"},
   {0xFDBEU, "TR1",      "Transit Route"},
   {0xFDBFU, "TR2",      "Transit Milepost"},
   {0xFDC0U, "TR7",      "Passenger Counter"},
   {0xFDC1U, "EFLP3",    "Engine Fluid Level/Pressure 3"},
   {0xFDC4U, "EBC5",     "Electronic Brake Controller 5"},
   {0xFDC5U, "ECUID",    "ECU Identification Information"},
   {0xFDC6U, "",         "Cold Start Aids"},
   {0xFDC7U, "OHCSS",    "Off-Highway Engine Control Selection States"},
   {0xFDC8U, "ISCS",     "Operator Primary Intermediate Speed Control State"},
   {0xFDC9U, "CMI",      "Electronic Control Module Information"},
   {0xFDCAU, "ISC",      "Intermediate Speed Control"},
   {0xFDCBU, "OHECS",    "Off-Highway Engine Control Selection"},
   {0xFDCCU, "OEL",      "Operators External Light Controls Message"},
   {0xFDCDU, "OWW",      "Operator Wiper and Washer Controls Message"},
   {0xFDD0U, "IC2",      "Inlet/Exhaust Conditions 2"},
   {0xFDD1U, "FMS",      "FMS-standard Interface Identity/Capabilities"},
   {0xFDD2U, "EP",       "ECU Peformance"},
   {0xFDD3U, "TCI6",     "Turbocharger Information 6"},
   {0xFDD4U, "CM3",      "Cab Message 3"},
   {0xFDD5U, "EEC5",     "Electronic Engine Controller 5"},
   {0xFDD6U, "BJM1",     "Basic Joystick Message 1"},
   {0xFDD7U, "EJM1",     "Extended Joystick Message 1"},
   {0xFDD8U, "BJM2",     "Basic Joystick Message 2"},
   {0xFDD9U, "EJM2",     "Extended Joystick Message 2"},
   {0xFDDAU, "BJM3",     "Basic Joystick Message 3"},
   {0xFDDBU, "EJM3",     "Extended Joystick Message 3"},
   {0xFDDCU, "MCI",      "Marine Control Information"},
   {0xFDDFU, "FWD",      "Front Wheel Drive"},
   {0xFDE0U, "AMB2",     "Ambient Conditions 2"},
   {0xFDE1U, "CACI",     "Cab A/C Climate System Information"},
   {0xFDE2U, "SPR",      "Supply Pressure Demand"},
   {0xFDE3U, "EOAC",     "Equipment Operation and Control"},
   {0xFDE4U, "EPD",      "Equipment Performance Data"},
   {0xFDE5U, "MVS",      "Maximum Vehicle Speed Limit Status"},
   {0xFDE6U, "HBS",      "Hydraulic Braking System"},
   {0xFE07U, "ET",       "Exhaust Temperature"},
   {0xFE40U, "LD",       "Lighting Data"},
   {0xFE41U, "LC",       "Lighting Command"},
   {0xFE4AU, "ETC7",     "Electronic Transmission Controller 7"},
   {0xFE4BU, "TCFG2",    "Transmission Configuration 2"},
   {0xFE4CU, "ML",       "Military Lighting Command"},
   {0xFE4DU, "TAVG",     "Total Averaged Information"},
   {0xFE4EU, "DC1",      "Door Control 1"},
   {0xFE4FU, "VDC1",     "Vehicle Dynamic Stability Control 1"},
   {0xFE50U, "BT1",      "Battery Temperature"},
   {0xFE51U, "ACC2",     "Adaptive Cruise Control, Operator Input"},
   {0xFE52U, "VEP3",     "Vehicle Electrical Power 3"},
   {0xFE53U, "RTC1",     "Retarder Continuous Torque & Speed Limit"},
   {0xFE54U, "ECT1",     "Engine Continuous Torque & Speed Limit"},
   {0xFE55U, "GFD",      "Gaseous Fuel Properties"},
   {0xFE56U, "TI1",      "Tank Information"},
   {0xFE57U, "ASC5",     "Air Suspension Control 5"},
   {0xFE58U, "ASC4",     "Air Suspension Control 4"},
   {0xFE59U, "ASC3",     "Air Suspension Control 3"},
   {0xFE5AU, "ASC1",     "Air Suspension Control 1"},
   {0xFE5BU, "FLI2",     "Forward Lane Image"},
   {0xFE66U, "BM",       "Battery Main Switch Information"},
   {0xFE67U, "CCC",      "Climatic Control Configuration"},
   {0xFE68U, "VF",       "Vehicle Fluids"},
   {0xFE69U, "ET3",      "Engine Temperature 3"},
   {0xFE6AU, "EFS",      "Engine Fuel/lube systems"},
   {0xFE6BU, "DI",       "Driver Identification"},
   {0xFE6CU, "TCO1",     "Tachograph"},
   {0xFE6DU, "HTR",      "Heater Information"},
   {0xFE6EU, "HRW",      "High Resolution Wheel Speed"},
   {0xFE6FU, "ACC1",     "Adaptive Cruise Control"},
   {0xFE70U, "CVW",      "Combination Vehicle Weigth"},
   {0xFE71U, "LTP",      "Laser Tracer Position"},
   {0xFE72U, "LBC",      "Laser Leveling System Blade Control"},
   {0xFE73U, "LMP",      "Laser Receiver Mast Position"},
   {0xFE74U, "LSP",      "Modify Leveling System Control Set Point"},
   {0xFE75U, "LVD",      "Laser Leveling System Vertical Deviation"},
   {0xFE76U, "LVDD",     "Laser Leveling System Vertical Position Display Data"},
   {0xFE77U, "AP",       "Auxiliary Pressures"},
   {0xFE78U, "TP1",      "Tire Pressure Control Unit Mode and Status"},
   {0xFE79U, "TP2",      "Tire Pressure Control Unit Target Pressures"},
   {0xFE7AU, "TP3",      "Tire Pressure Control Unit Current Pressures"},
   {0xFE7BU, "CT1",      "Combustion Time 1"},
   {0xFE7CU, "CT2",      "Combustion Time 2"},
   {0xFE7DU, "CT3",      "Combustion Time 3"},
   {0xFE7EU, "CT4",      "Combustion Time 4"},
   {0xFE7FU, "CT5",      "Combustion Time 5"},
   {0xFE80U, "CT6",      "Combustion Time 6"},
   {0xFE81U, "GFI2",     "Fuel Information 2"},
   {0xFE82U, "IT1",      "Ignition Time 1"},
   {0xFE83U, "IT2",      "Ignition Time 2"},
   {0xFE84U, "IT3",      "Ignition Time 3"},
   {0xFE85U, "IT4",      "Ignition Time 4"},
   {0xFE86U, "IT5",      "Ignition Time 5"},
   {0xFE87U, "IT6",      "Ignition Time 6"},
   {0xFE88U, "ISO1",     "Ignition Transformer Secondary Output 1"},
   {0xFE89U, "ISO2",     "Ignition Transformer Secondary Output 2"},
   {0xFE8AU, "ISO3",     "Ignition Transformer Secondary Output 3"},
   {0xFE8BU, "GFP",      "Gaseous Fuel Pressure"},
   {0xFE8CU, "AAI",      "Auxiliary Analog Information"},
   {0xFE8DU, "VEP2",     "Vehicle Electrical Power 2"},
   {0xFE8EU, "S2",       "Service 2"},
   {0xFE8FU, "SP2",      "Supply Pressure 2"},
   {0xFE90U, "ETH",      "Engine Torque History"},
   {0xFE91U, "FL",       "Fuel Leakage"},
   {0xFE92U, "EI",       "Engine Information"},
   {0xFE93U, "EES",      "Engine Electrical System/Module Information"},
   {0xFE94U, "EAC",      "Engine Auxiliary Coolant"},
   {0xFE95U, "RBI",      "Rebuild Information"},
   {0xFE96U, "TCW",      "Turbocharger Wastegate"},
   {0xFE97U, "TCI5",     "Turbocharger Information 5"},
   {0xFE98U, "TCI4",     "Turbocharger Information 4"},
   {0xFE99U, "TCI3",     "Turbocharger Information 3"},
   {0xFE9AU, "TCI2",     "Turbocharger Information 2"},
   {0xFE9BU, "TCI1",     "Turbocharger Information 1"},
   {0xFE9CU, "MBT3",     "Main Bearing Temperature 3"},
   {0xFE9DU, "MBT2",     "Main Bearing Temperature 2"},
   {0xFE9EU, "MBT1",     "Main Bearing Temperature 1"},
   {0xFE9FU, "EPT5",     "Exhaust Port Temperature 5"},
   {0xFEA0U, "EPT4",     "Exhaust Port Temperature 4"},
   {0xFEA1U, "EPT3",     "Exhaust Port Temperature 3"},
   {0xFEA2U, "EPT2",     "Exhaust Port Temperature 2"},
   {0xFEA3U, "EPT1",     "Exhaust Port Temperature 1"},
   {0xFEA4U, "ET2",      "Engine Temperature 2"},
   {0xFEA5U, "IMT2",     "Intake Manifold Information 2"},
   {0xFEA6U, "IMT1",     "Intake Manifold Information 1"},
   {0xFEA7U, "AT",       "Alternator Temperature"},
   {0xFEA8U, "AC",       "Articulation Control"},
   {0xFEA9U, "EO1",      "Exhaust Oxygen 1"},
   {0xFEAAU, "AF2",      "Alternate Fuel 2"},
   {0xFEABU, "ETC6",     "Electronic Transmission Controller 6"},
   {0xFEACU, "EBC4",     "Wheel Brake Lining Remaining Information"},
   {0xFEADU, "EBC3",     "Wheel Application Pressure High Range Information"},
   {0xFEAEU, "AIR1",     "Air Supply Pressure"},
   {0xFEAFU, "GFC",      "Fuel Consumption (Gaseous)"},
   {0xFEB0U, "TTI2",     "Trip Time Information 2"},
   {0xFEB1U, "EH",       "ECU History"},
   {0xFEB2U, "GFI1",     "Fuel Information 1 (Gaseous)"},
   {0xFEB3U, "LFI",      "Fuel Information (Liquid)"},
   {0xFEB4U, "TTI1",     "Trip Time Information 1"},
   {0xFEB5U, "TSI",      "Trip Shutdown Information"},
   {0xFEB6U, "TVI",      "Trip Vehicle Speed/Cruise Distance Information"},
   {0xFEB7U, "LF",       "Engine Speed/Load Factor Information"},
   {0xFEB8U, "GTFI",     "Trip Fuel Information (Gaseous)"},
   {0xFEB9U, "LTFI",     "Trip Fuel Information (Liquid)"},
   {0xFEBAU, "TDI",      "Trip Distance Information"},
   {0xFEBBU, "TFI",      "Trip Fan Information"},
   {0xFEBCU, "CBI",      "Compression/Service Brake Information"},
   {0xFEBDU, "FD",       "Fan Drive"},
   {0xFEBEU, "EEC4",     "Electronic Engine Controller 4"},
   {0xFEBFU, "EBC2",     "Wheel Speed information"},
   {0xFEC0U, "SERV",     "Service Information"},
   {0xFEC1U, "VDHR",     "High Resolution Vehicle Distance"},
   {0xFEC2U, "ERC2",     "Electronic Retarder Controller 2"},
   {0xFEC3U, "ETC5",     "Electronic Transmission Controller 5"},
   {0xFEC5U, "ETC4",     "Electronic Transmission Controller 4"},
   {0xFEC7U, "ETC3",     "Electronic Transmission Controller 3"},
   {0xFED5U, "AS",       "Alterator Speed"},
   {0xFED9U, "AUXIO",    "Auxiliary Input/Output Status"},
   {0xFEDAU, "SOFT",     "Software Identification"},
   {0xFEDBU, "EFLP2",    "Engine Fluid Level/Pressure 2"},
   {0xFEDCU, "IO",       "Idle Operation"},
   {0xFEDDU, "TC",       "Turbocharger"},
   {0xFEDEU, "AIR2",     "Air Start Pressure"},
   {0xFEDFU, "EEC3",     "Electronic Engine Controller 3"},
   {0xFEE0U, "VD",       "Vehicle Distance"},
   {0xFEE1U, "RC",       "Retarder Configuration"},
   {0xFEE2U, "TCFG",     "Transmission Configuration"},
   {0xFEE3U, "EC",       "Engine Configuration"},
   {0xFEE4U, "SHUTDOW",  "Shutdown"},
   {0xFEE5U, "HOURS",    "Engine Hours, Revolutions"},
   {0xFEE6U, "TD",       "Time/Date"},
   {0xFEE7U, "VH",       "Vehicle Hours"},
   {0xFEE8U, "VDS",      "Vehicle Direction/Speed"},
   {0xFEE9U, "LFC",      "Fuel Consumption (Liquid)"},
   {0xFEEAU, "VW",       "Vehicle Weight"},
   {0xFEEBU, "CI",       "Component Identification"},
   {0xFEECU, "VI",       "Vehicle Identification"},
   {0xFEEDU, "CCSS",     "Cruise Control/Vehicle Speed Setup"},
   {0xFEEEU, "ET1",      "Engine Temperatur 1"},
   {0xFEEFU, "EFLP1",    "Engine Fluid Level/Pressure 1"},
   {0xFEF0U, "PTO",      "Power Takeoff Information"},
   {0xFEF1U, "CCVS",     "Cruise Control/Vehicle Speed"},
   {0xFEF2U, "LFE",      "Fuel Economy (Liquid)"},
   {0xFEF3U, "VP",       "Vehicle Position"},
   {0xFEF4U, "TIRE",     "Tire Condition"},
   {0xFEF5U, "AMB",      "Ambient Conditions"},
   {0xFEF6U, "IC1",      "Inlet/Exhaust Conditions 1"},
   {0xFEF7U, "VEP1",     "Vehicle Electrical Power 1"},
   {0xFEF8U, "TRF1",     "Transmission Fluids 1"},
   {0xFEF9U, "AI",       "Axle Information"},
   {0xFEFAU, "B",        "Brakes"},
   {0xFEFBU, "RF",       "Retarder Fluids"},
   {0xFEFCU, "DD",       "Dash Display"},
   {0xFEFDU, "A1",       "Alternate Fuel 1"},
   {0xFEFEU, "AWPP",     "Auxiliary Water Pump Pressure"},
   {0xFEFFU, "WFI",      "Water in Fuel Indicator"}
};

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
C_CanMonProtocolJ1939::C_CanMonProtocolJ1939(void) :
   C_CanMonProtocolBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert CAN message to text representation.

   \param[in]     orc_Msg              message to convert

   \return
   Text interpretation of CAN message ("" if the message can not be interpreted)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolJ1939::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   uint8_t u8_PduFormat;
   uint8_t u8_PduSpecific;
   uint16_t u16_Pgn;
   C_SclString c_Text;
   C_SclString c_Pgn;
   C_SclString c_PduSpecific;
   C_SclString c_SourceAddress = "SA ";
   C_SclString c_Data;
   C_SclString c_Reason = "REASON ";
   C_SclString c_MessageSize;
   C_SclString c_TotalPackets = "TOTAL_PACKETS ";
   C_SclString c_MaxPackets = "MAX_PACKETS ";
   C_SclString t_PGNMBM = "PGN ";
   C_SclString c_SequenceNumber = "PACKET ";
   C_SclString c_NextPacket = "NEXT_PACKET ";
   C_SclString c_Address = "ADDRESS ";

   if (orc_Msg.u8_XTD == 1)
   {
      //just evaluate CAN messages with extended ID
      //get PGN
      u16_Pgn = static_cast<uint16_t>(orc_Msg.u32_ID >> 8U);
      //get PDU format
      u8_PduFormat = static_cast<uint8_t>(orc_Msg.u32_ID >> 16U);
      //get PDU specific
      u8_PduSpecific = static_cast<uint8_t>(orc_Msg.u32_ID >> 8U);

      //evaluate PDU format
      if (u8_PduFormat <= J1939_PDU1_MAX)
      {
         //PDU1 (specific, peer to peer), PDU specific (PS) contains destination address (DA)
         //mask out DA
         u16_Pgn &= J1939_DA_MASK;
         switch (u16_Pgn)
         {
         case J1939_PGN_TP_CM:
            //Broadcast or Peer-to-Peer Connection Management
            //evaluate control byte
            switch (orc_Msg.au8_Data[0])
            {
            case J1939_TP_CM_BAM:
               //Broadcast Announce Message
               c_Pgn = "TP.CM.BAM";
               //Message size (in Byte)
               c_MessageSize += m_GetMessageSize(orc_Msg);
               //Total number of packages
               c_TotalPackets += m_GetByteAsStringFormat(orc_Msg.au8_Data[3]);
               //Parameter Group Number of the multi-packet message
               t_PGNMBM += m_GetValueDecHex(m_GetPgn(&orc_Msg.au8_Data[5]));
               //Concatenate data
               c_Data = c_MessageSize + " " + c_TotalPackets + " " + t_PGNMBM;
               break;
            case J1939_TP_CM_RTS:
               //Request to send
               c_Pgn = "TP.CM.RTS";
               //Message size (in Byte)
               c_MessageSize += m_GetMessageSize(orc_Msg);
               //Total number of packages
               c_TotalPackets += m_GetByteAsStringFormat(orc_Msg.au8_Data[3]);
               //Maximum number of packages in response to CTS
               if (orc_Msg.au8_Data[4] < 0xFF)
               {
                  c_MaxPackets += m_GetByteAsStringFormat(orc_Msg.au8_Data[4]);
               }
               else
               {
                  c_MaxPackets = "NO_MAX_PACKETS";
               }
               //Parameter Group Number of the multi-packet message
               t_PGNMBM += m_GetValueDecHex(m_GetPgn(&orc_Msg.au8_Data[5]));
               //Concatenate data
               c_Data = c_MessageSize + " " + c_TotalPackets + " " + c_MaxPackets + " " + t_PGNMBM;
               break;
            case J1939_TP_CM_CTS:
               //Clear to send
               c_Pgn = "TP.CM.CTS";
               //Total number of packages (should not exceed 5th byte in RTS)
               c_TotalPackets += m_GetByteAsStringFormat(orc_Msg.au8_Data[1]);
               //Next package number
               c_NextPacket += m_GetByteAsStringFormat(orc_Msg.au8_Data[2]);
               //Parameter Group Number of the multi-packet message
               t_PGNMBM += m_GetValueDecHex(m_GetPgn(&orc_Msg.au8_Data[5]));
               //Concatenate data
               c_Data = c_TotalPackets + " " + c_NextPacket + " " + t_PGNMBM;
               break;
            case J1939_TP_CM_EOM_ACK:
               //End of message acknowledgment
               c_Pgn = "TP.CM.EOM_ACK";
               //Message size (in Byte)
               c_MessageSize += m_GetMessageSize(orc_Msg);
               //Total number of packages
               c_TotalPackets += m_GetByteAsStringFormat(orc_Msg.au8_Data[3]);
               //Parameter Group Number of the multi-packet message
               t_PGNMBM += m_GetValueDecHex(m_GetPgn(&orc_Msg.au8_Data[5]));
               //Concatenate data
               c_Data = c_MessageSize + " " + c_TotalPackets + " " + t_PGNMBM;
               break;
            case J1939_TP_CM_CONN_ABORT:
               //Connection abort
               c_Pgn = "TP.CM.CONN_ABORT";
               //Connection abort reason
               switch (orc_Msg.au8_Data[1])
               {
               case J1939_TP_CM_ABORT_AS_BUSY:
                  c_Reason += "1 ";
                  break;
               case J1939_TP_CM_ABORT_AS_RESOURCE_LACK:
                  c_Reason += "2 ";
                  break;
               case J1939_TP_CM_ABORT_AS_TIMEOUT:
                  c_Reason += "3 ";
                  break;
               default:
                  //Reserved by SAE
                  c_Reason += "UNKNOWN ";
                  break;
               }
               //Parameter Group Number of the multi-packet message
               t_PGNMBM += m_GetValueDecHex(m_GetPgn(&orc_Msg.au8_Data[5]));
               //Concatenate data
               c_Data = c_Reason + " " + t_PGNMBM;
               break;
            default:
               //unknown flow control message
               break;
            }
            break;
         case J1939_PGN_TP_DT:
            //Broadcast or Peer-to-Peer Transport Protocol
            c_Pgn = "TP.DT";
            //Sequence number
            c_SequenceNumber += m_GetByteAsStringFormat(orc_Msg.au8_Data[0]);
            //Concatenate data
            c_Data = c_SequenceNumber + " " + m_GetData(orc_Msg, 1);
            break;
         case J1939_PGN_REQUEST:
            if ((orc_Msg.u8_DLC == 3U) &&
                (orc_Msg.au8_Data[0] == 0U) &&
                (orc_Msg.au8_Data[1] == 0xEEU) &&
                (orc_Msg.au8_Data[2] == 0U))
            {
               //Network Management: Request for Address Claimed message
               c_Pgn = "NM.REQ.ADDR_CLAIMED";
               c_Data = "";
            }
            else
            {
               //Global or specific request
               c_Pgn = "REQ";
               //Requested Parameter Group Number
               t_PGNMBM += m_GetValueDecHex(m_GetPgn(&orc_Msg.au8_Data[0]));
               //Concatenate data
               c_Data = t_PGNMBM + " (" + m_PgnToString(m_GetPgn(&orc_Msg.au8_Data[0])) + ")";
            }
            break;
         case J1939_PGN_ADDRESS_CLAIMED:
            //Network Management: Address Claimed Response
            if (static_cast<uint8_t>(orc_Msg.u32_ID) == J1939_SA_NULL)
            {
               //No address claimed
               c_Pgn = "NM.CANNOT_CLAIM";
            }
            else
            {
               //Address claimed
               c_Pgn = "NM.ADDR_CLAIMED";
            }
            c_Data = m_GetName(orc_Msg);
            break;
         case J1939_PGN_ACK:
            //Acknowledgement
            //Evaluate control byte
            switch (orc_Msg.au8_Data[0])
            {
            case J1939_ACK_POS:
               //Positive acknowledge
               c_Pgn = "ACK";
               break;
            case J1939_ACK_NEG:
               //Negative acknowledge
               c_Pgn = "NACK";
               break;
            case J1939_ACK_ACCESS_DENIED:
               //Access denied (PGN supported, but security denied access)
               c_Pgn = "ACCESS_DENIED";
               break;
            case J1939_ACK_CANNOT_RESPOND:
               //Cannot respond (PGN supported, but ECU cannot respond; request data later)
               c_Pgn = "CANNOT_RESPOND";
               break;
            default:
               //unknown control
               break;
            }
            //Address
            c_Address += m_GetByteAsStringFormat(orc_Msg.au8_Data[4]);
            //Parameter Group Number of the acknowledgement message
            t_PGNMBM += m_GetValueDecHex(m_GetPgn(&orc_Msg.au8_Data[5]));
            //Concatenate data
            c_Data = c_Address + " " + t_PGNMBM;
            break;
         default:
            //Remaining PDU1
            c_Pgn = m_PgnToString(u16_Pgn);
            //Concatenate data
            c_Data = m_GetData(orc_Msg, 0);
            break;
         }
         if (u8_PduSpecific != J1939_DA_BROADCAST)
         {
            //set PDU specific part (destination address)
            c_PduSpecific += "DA " + m_GetByteAsStringFormat(u8_PduSpecific) + " ";
         }
      }
      else
      {
         //PDU2 (global, broadcast), PDU specific (PS) contains group extension (GE)
         //evaluate PGN
         c_Pgn = m_PgnToString(u16_Pgn);
         if (c_Pgn == "PDU2")
         {
            //set PDU2 specific part
            c_PduSpecific = "GE " + m_GetByteAsStringFormat(u8_PduSpecific) + " ";
         }

         //Concatenate data
         c_Data = m_GetData(orc_Msg, 0);
      }

      //get source address
      c_SourceAddress += m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.u32_ID)) + " ";

      //Concatenate resulting string
      c_Text = c_Pgn + " " + c_SourceAddress + c_PduSpecific + c_Data;
   }
   else
   {
      //CAN message with standard (11Bit) identifier
      c_Text = "";
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name of protocol as string

   \return
   Text representation of protocol name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolJ1939::GetProtocolName(void) const
{
   return "SAE J1939 06/2006";
}

//----------------------------------------------------------------------------------------------------------------------

C_SclString C_CanMonProtocolJ1939::m_PgnToString(const uint32_t ou32_Pgn) const
{
   C_SclString c_Pgn = "";
   uint16_t u16_Pos;
   uint16_t u16_First = 0U;
   uint16_t u16_Last = static_cast<uint16_t>((sizeof(mat_Pgn) / sizeof(T_J1939_PGN)) - 1);

   //do a binary search for desired PGN
   while (u16_First <= u16_Last)
   {
      u16_Pos = static_cast<uint16_t>((u16_First + u16_Last) / 2);
      if (static_cast<uint16_t>(ou32_Pgn) > mat_Pgn[u16_Pos].u16_Value)
      {
         u16_First = static_cast<uint16_t>(u16_Pos + 1);
      }
      else if (static_cast<uint16_t>(ou32_Pgn) < mat_Pgn[u16_Pos].u16_Value)
      {
         u16_Last = static_cast<uint16_t>(u16_Pos - 1);
      }
      else
      {
         //found PGN
         c_Pgn = mat_Pgn[u16_Pos].c_Brief;
         break;
      }
   }

   if (c_Pgn == "")
   {
      //specific PGN not found -> evaluate PDU format
      if (static_cast<uint8_t>(ou32_Pgn >> 8U) <= J1939_PDU1_MAX)
      {
         c_Pgn = "PDU1";
      }
      else
      {
         c_Pgn = "PDU2";
      }
   }

   return c_Pgn;
}

//----------------------------------------------------------------------------------------------------------------------

uint32_t C_CanMonProtocolJ1939::m_GetPgn(const uint8_t * const opu8_Data) const
{
   return (static_cast<uint32_t>(opu8_Data[2]) << 16U) + (static_cast<uint32_t>(opu8_Data[1]) << 8U) + opu8_Data[0];
}

//----------------------------------------------------------------------------------------------------------------------

C_SclString C_CanMonProtocolJ1939::m_GetMessageSize(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Size = "SIZE ";

   c_Size += m_GetWordAsStringFormat(
      (static_cast<uint16_t>(static_cast<uint16_t>(orc_Msg.au8_Data[2]) << 8U)) + orc_Msg.au8_Data[1]);

   return c_Size;
}

//----------------------------------------------------------------------------------------------------------------------

C_SclString C_CanMonProtocolJ1939::m_GetData(const T_STWCAN_Msg_RX & orc_Msg, const uint8_t ou8_StartIdx) const
{
   uint8_t u8_Index;
   C_SclString c_Data = "DATA [";

   if ((ou8_StartIdx < 8U) && (ou8_StartIdx < orc_Msg.u8_DLC))
   {
      for (u8_Index = ou8_StartIdx; u8_Index < orc_Msg.u8_DLC; u8_Index++)
      {
         c_Data += m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Index]);
         if (u8_Index < (orc_Msg.u8_DLC - 1))
         {
            c_Data += ", ";
         }
      }
   }
   else
   {
      c_Data += "UNKNOWN";
   }

   c_Data += "]";

   return c_Data;
}

//----------------------------------------------------------------------------------------------------------------------

C_SclString C_CanMonProtocolJ1939::m_GetName(const T_STWCAN_Msg_RX & orc_Msg) const
{
   uint8_t u8_Index;
   uint64_t u64_Name = 0U;
   C_SclString c_Data = "NAME: ";

   //copy the eight data bytes to the 64bit NAME
   for (u8_Index = 0U; u8_Index < orc_Msg.u8_DLC; u8_Index++)
   {
      u64_Name += static_cast<uint64_t>(orc_Msg.au8_Data[u8_Index]) << (8 * u8_Index);
   }

   //Arbitrary Address Capable
   c_Data += "AAC:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>(u64_Name >> 63U));
   c_Data += " ";

   //Industry Group
   c_Data += "IG:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>((u64_Name >> 60U) & 0x07U));
   c_Data += " ";

   //Vehicle System Instance
   c_Data += "VSI:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>((u64_Name >> 56U) & 0x0FU));
   c_Data += " ";

   //Vehicle System
   c_Data += "VS:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>((u64_Name >> 49U) & 0x7FU));
   c_Data += " ";

   //Reserved
   c_Data += "R:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>((u64_Name >> 48U) & 0x01U));
   c_Data += " ";

   //Function
   c_Data += "F:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>((u64_Name >> 40U) & 0xFFU));
   c_Data += " ";

   //Function Instance
   c_Data += "FI:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>((u64_Name >> 35U) & 0x1FU));
   c_Data += " ";

   //ECU Instance
   c_Data += "EI:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>((u64_Name >> 32U) & 0x07U));
   c_Data += " ";

   //Manufacturer Code
   c_Data += "MC:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>((u64_Name >> 21U) & 0x07FFU));
   c_Data += " ";

   //Identity Number
   c_Data += "IN:";
   c_Data += m_GetValueDecHex(static_cast<uint32_t>(u64_Name & 0x1FFFFFU));

   return c_Data;
}
