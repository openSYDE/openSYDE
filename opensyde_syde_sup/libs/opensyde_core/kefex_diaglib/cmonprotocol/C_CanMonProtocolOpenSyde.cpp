//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to L7 interpretation

   Translate L2 CAN message to openSYDE protocol L7 interpretation

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.hpp" //pre-compiled headers

#include <cctype>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanMonProtocolOpenSyde.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::cmon_protocol;
using namespace stw::scl;
using namespace stw::can;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
//have the protocol constants here as copies, so we can put this class into a library of its own
// without having to add all the protocol header files

//openSYDE protocol contants
// ISO15765-2 structure CAN identifier
#define CAN_ID_P     (0x18000000U)           // CAN-ID priority bits (default 6)
#define CAN_ID_R     (0x02000000U)           // CAN-ID reserved bit (usually 0, STW: routing 1)
#define CAN_ID_DP    (0x01000000U)           // CAN-ID data page bits (usually 0, STW: routing 1)
#define CAN_ID_TOS   (0x00C00000U)           // CAN-ID type of service (only used for routing, usually 11b)
#define CAN_ID_PF__N_TA_TYPE_5 (0x00DA0000U) // CAN-ID protocol data unit format (normal fixed address. physical 218)
#define CAN_ID_PF__N_TA_TYPE_6 (0x00DB0000U) // CAN-ID protocol data unit format (normal fixed address. functional 219)

// ISO15765-2 addressing mode
#define OSY_CTP_ADDR_MODE_PHYSICAL   (0U) ///< CAN TP mode fixed physical addressing
#define OSY_CTP_ADDR_MODE_FUNCTIONAL (1U) ///< CAN TP mode fixed functional addressing

// ISO15765-3 routing
#define OSY_CTP_ROUTING_INACTIVE (0U) ///< CAN TP routing is deactivated on ECU
#define OSY_CTP_ROUTING_ACTIVE   (1U) ///< CAN TP routing is activated on ECU

#define ISO15765_N_PCI_SF  (0x00U) // single frame <=8bytes
#define ISO15765_N_PCI_FF  (0x10U) // first frame <=4095bytes
#define ISO15765_N_PCI_CF  (0x20U) // consecutive frame
#define ISO15765_N_PCI_FC  (0x30U) // flow control
#define STWOPENSYDE_PCI_SF (0xF0U) // STW-specific single frame
#define STWOPENSYDE_PCI_MF (0xE0U) // STW-specific multi frame

//openSYDE service IDs
#define OSY_UDS_SI_DIAGNOSTIC_SESSION_CONTROL          (0x10U)
#define OSY_UDS_SI_ECU_RESET                           (0x11U)
#define OSY_UDS_SI_READ_DATA_BY_IDENTIFIER             (0x22U)
#define OSY_UDS_SI_READ_MEMORY_BY_ADDRESS              (0x23U)
#define OSY_UDS_SI_SECURITY_ACCESS                     (0x27U)
#define OSY_UDS_SI_WRITE_DATA_BY_IDENTIFIER            (0x2EU)
#define OSY_UDS_SI_INPUT_OUTPUT_CONTROL_BY_IDENTIFIER  (0x2FU)
#define OSY_UDS_SI_ROUTINE_CONTROL                     (0x31U)
#define OSY_UDS_SI_REQUEST_DOWNLOAD                    (0x34U)
#define OSY_UDS_SI_REQUEST_UPLOAD                      (0x35U)
#define OSY_UDS_SI_TRANSFER_DATA                       (0x36U)
#define OSY_UDS_SI_REQUEST_TRANSFER_EXIT               (0x37U)
#define OSY_UDS_SI_REQUEST_FILE_TRANSFER               (0x38U)
#define OSY_UDS_SI_WRITE_MEMORY_BY_ADDRESS             (0x3DU)
#define OSY_UDS_SI_TESTER_PRESENT                      (0x3EU)
#define OSY_DPD_SI_OS_READ_DATAPOOLDATA_EVENT_DRIVEN   (0xBAU) ///< system supplier specific
#define OSY_DPD_SI_OS_READ_DATAPOOLDATA_BY_IDENTIFIER  (0xBBU) ///< system supplier specific
#define OSY_DPD_SI_OS_WRITE_DATAPOOLDATA_BY_IDENTIFIER (0xBCU) ///< system supplier specific
#define OSY_UDC_B_SI_READ_SERIAL_NUMBER                (0xBDU) ///< system supplier specific
#define OSY_UDC_B_SI_SS_CAN_TUNNELING                  (0xBEU) ///< system supplier specific
#define OSY_OS_READ_DATAPOOLDATA_EVENT_DRIVEN          (0xF1U) ///< STW-specific single frame specific
#define OSY_OS_POS_STARTUP_MESSAGE                     (0xF3U) ///< STW-specific single frame specific
#define OSY_OS_FSN_STARTUP_MESSAGE                     (0xF4U) ///< STW-specific single frame specific

///negative response byte:
#define OSY_UDS_NR_SI       (0x7FU) ///< Negative response service identifier

//negative response codes
#define OSY_UDS_NRC_NO_ERROR                                    (0x00U) ///< positive response
#define OSY_UDS_NRC_SERVICE_NOT_SUPPORTED                       (0x11U) ///< serviceNotSupported
#define OSY_UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED                  (0x12U) ///< sub-functionNotSupported
#define OSY_UDS_NRC_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT  (0x13U) ///< incorrectMessageLengthOrInvalidFormat
#define OSY_UDS_NRC_RESPONSE_TOO_LONG                           (0x14U) ///< responseTooLong
#define OSY_UDS_NRC_CONDITIONS_NOT_CORRECT                      (0x22U) ///< conditionsNotCorrect
#define OSY_UDS_NRC_REQUEST_SEQUENCE_ERROR                      (0x24U) ///< requestSequenceError
#define OSY_UDS_NRC_REQUEST_OUT_OF_RANGE                        (0x31U) ///< requestOutOfRange
#define OSY_UDS_NRC_SECURITY_ACCESS_DENIED                      (0x33U) ///< securityAccessDenied
#define OSY_UDS_NRC_INVALID_KEY                                 (0x35U) ///< invalidKey
#define OSY_UDS_NRC_EXCEEDED_NUMBER_OF_ATTEMPTS                 (0x36U) ///< exceededNumberOfAttempts
#define OSY_UDS_NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED             (0x37U) ///< requiredTimeDelayNotExpired
#define OSY_UDS_NRC_UPLOAD_DOWNLOAD_NOT_ACCEPTED                (0x70U) ///< uploadDownloadNotAccepted
#define OSY_UDS_NRC_TRANSFER_DATA_SUSPENDED                     (0x71U) ///< transferDataSuspended
#define OSY_UDS_NRC_GENERAL_PROGRAMMING_FAILURE                 (0x72U) ///< generalProgrammingFailure
#define OSY_UDS_NRC_WRONG_BLOCK_SEQUENCE_COUNTER                (0x73U) ///< wrongBlockSequenceCounter
#define OSY_UDS_NRC_RESPONSE_PENDING                            (0x78U) ///< requestCorrectlyReceivedResponsePending
#define OSY_UDS_NRC_SUBFUNCTION_NOT_SUPPORTED_IN_ACTIVE_SESSION (0x7EU) ///< sub-functionNotSupportedInActiveSession
#define OSY_UDS_NRC_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION     (0x7FU) ///< serviceNotSupportedInActiveSession
#define OSY_UDS_NRC_VOLTAGE_TOO_HIGH                            (0x92U) ///< voltageTooHigh
#define OSY_UDS_NRC_VOLTAGE_TOO_LOW                             (0x93U) ///< voltageTooLow

//data identifiers
#define DPD_DATAID_LIST_OF_FEATURES                         (0xA800U)
#define DPD_DATAID_MAX_NUMBER_OF_BLOCK_LENGTH               (0xA801U)
#define DPD_DATAID_DATARATE1                                (0xA810U)
#define DPD_DATAID_DATARATE2                                (0xA811U)
#define DPD_DATAID_DATARATE3                                (0xA812U)
#define UDC_H_DATAID_PROTOCOL_VERSION                       (0xA813U)
#define DPD_DATAID_PROTOCOL_DRIVER_IMPLEMENTATION_VERSION   (0xA814U)
#define FL_H_DATAID_FLASHLOADER_PROTOCOL_VERSION            (0xA815U)
#define DPD_DATAID_MAX_NUM_ASYNC                            (0xA817U)
#define FL_H_DATAID_FLASH_COUNT                             (0xA819U)
#define UDC_H_DATAID_DEVICE_NAME                            (0xA81AU)
#define DPD_DATAID_APPLICATION_NAME                         (0xA81BU)
#define DPD_DATAID_APPLICATION_VERSION                      (0xA81CU)
#define DPD_DATAID_FILE_BASED_TRANSFER_EXIT_RESULT          (0xA81DU)
#define UDC_H_DATAID_ECU_SERIAL_NUMBER_EXT                  (0xA81EU)
#define UDC_H_DATAID_SUB_NODE_ID                            (0xA81FU)
#define UDC_H_DATAID_CERTIFICATE_SERIAL_NUMBER              (0xA820U)
#define UDC_H_DATAID_SECURITY_ACTIVATION                    (0xA821U)
#define UDC_H_DATAID_DEBUGGER_ACTIVATION                    (0xA822U)
#define UDC_H_DATAID_SECURITY_KEY                           (0xA823U)
#define UDC_H_DATAID_BOOTSOFTWARE_IDENTIFICATION            (0xF180U)
#define FL_H_DATAID_SOFTWARE_FINGERPRINT                    (0xF184U)
#define UDC_H_DATAID_ACTIVE_DIAGNOSTIC_SESSION              (0xF186U)
#define UDC_H_DATAID_ECU_SERIAL_NUMBER                      (0xF18CU)
#define UDC_H_DATAID_SYSTEM_SUPPLIER_ECU_HW_NUMBER          (0xF192U)
#define UDC_H_DATAID_SYSTEM_SUPPLIER_ECU_HW_VERSION_NUMBER  (0xF193U)
#define UDC_H_DATAID_UDS_VERSION                            (0xFF00U)

//routine IDs
#define UDS_H_ROUTINE_CTRL_ROUTE_DIAGNOSIS_COMMUNICATION        (0x202U)
#define UDS_H_ROUTINE_CTRL_SEND_CAN_MESSAGE                     (0x203U)
#define UDS_H_ROUTINE_CTRL_TUNNEL_CAN_MESSAGES                  (0x204U)
#define UDS_H_ROUTINE_CTRL_ROUTE_IP_2_IP_COMMUNICATION          (0x205U)
#define UDS_H_ROUTINE_CTRL_REQUEST_PROGRAMMING                  (0x206U)
#define UDS_H_ROUTINE_CTRL_SET_BITRATE                          (0x207U)
#define FL_H_ROUTINE_CTRL_CHECK_FLASH_MEMORY_AVAILABLE          (0x208U)
#define FL_H_ROUTINE_CTRL_READ_FLASHBLOCK_DATA                  (0x209U)
#define UDS_H_ROUTINE_CTRL_CONFIG_FLALOAD_COMMUNICATION_CHANNEL (0x210U)
#define DPD_ROUTINEID_READ_DATAPOOL_METADATA                    (0x211U)
#define DPD_ROUTINEID_VERIFY_DATAPOOL                           (0x212U)
#define DPD_ROUTINEID_NOTIFY_NVM_CHANGED                        (0x213U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_FOR_CHANNEL               (0x214U)
#define UDS_H_ROUTINE_CTRL_SET_IP_ADDRESS_FOR_CHANNEL           (0x215U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_PART1     (0x216U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_PART2     (0x217U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_PART3     (0x218U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_1     (0x219U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_2     (0x21AU)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_3     (0x21BU)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_4     (0x21CU)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_5     (0x21DU)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_6     (0x21EU)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_7     (0x21FU)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_8     (0x220U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_9     (0x221U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_10    (0x222U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_11    (0x223U)
#define UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_12    (0x224U)

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert the openSYDE address information to text

   \param[in]     orc_CanAddressInformation   Address information

   \return
   Text interpretation of address information
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_AddressInformationToText(
   const C_CanMonProtocolOpenSyde::T_CanAddressInformation & orc_CanAddressInformation) const
{
   C_SclString c_Text;

   if (orc_CanAddressInformation.u8_RoutingMode == OSY_CTP_ROUTING_ACTIVE)
   {
      c_Text = "ROUTING ";
      //(bus.node) -> (bus.node): (from -> to)
      c_Text += ("(" + m_GetByteAsStringFormat(orc_CanAddressInformation.t_NodeIdSource.u8_Subnet) + "." +
                 m_GetByteAsStringFormat(orc_CanAddressInformation.t_NodeIdSource.u8_NodeId) + " -> " +
                 m_GetByteAsStringFormat(orc_CanAddressInformation.t_NodeIdTarget.u8_Subnet) + "." +
                 m_GetByteAsStringFormat(orc_CanAddressInformation.t_NodeIdTarget.u8_NodeId) + ") ");
   }
   else
   {
      c_Text = "LOCAL";
      if (orc_CanAddressInformation.u8_AddressMode == OSY_CTP_ADDR_MODE_PHYSICAL)
      {
         c_Text += "_PHYS ";
      }
      else
      {
         c_Text += "_FUNC ";
      }
      //(node) -> (node): (from -> to)
      c_Text += ("(" + m_GetByteAsStringFormat(orc_CanAddressInformation.t_NodeIdSource.u8_NodeId) + " -> ");
      if (orc_CanAddressInformation.t_NodeIdTarget.u8_NodeId == 0x7FU) //broadcast address ?
      {
         c_Text += "ALL) ";
      }
      else
      {
         c_Text += (m_GetByteAsStringFormat(orc_CanAddressInformation.t_NodeIdTarget.u8_NodeId) + ") ");
      }
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert 24bit openSYDE data pool identifier code to text

   Return as "<dpindex>.<listindex>.<elementindex>"
   Will also warn if the reserved bit is set.

   \param[in]     ou32_DataPoolIdentifier      data pool data identifier
   \param[in]     oq_IsResponse                Flag if service for accessing the datapool element was a response or
                                               a request
   \param[in]     orc_CanAddressInformation    Information about nodes involved in communication

   \return
   Text interpretation of data pool data identifier
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_DataPoolIdentifierToText(const uint32_t ou32_DataPoolIdentifier,
                                                                 const bool oq_IsResponse,
                                                                 const T_CanAddressInformation & orc_CanAddressInformation)
const
{
   const bool q_ReservedBitSet = ((ou32_DataPoolIdentifier & 0x00800000U) != 0U) ? true : false;
   const uint8_t u8_DataPoolIndex = static_cast<uint8_t>(ou32_DataPoolIdentifier >> 18) & 0x1FU;
   const uint8_t u8_ListIndex = static_cast<uint16_t>(ou32_DataPoolIdentifier >> 11) & 0x7FU;
   const uint16_t u16_ElementIndex = static_cast<uint16_t>(ou32_DataPoolIdentifier) & 0x7FFU;
   C_SclString c_Text;

   // Not necessary for base class implementation
   static_cast<void>(oq_IsResponse);
   static_cast<void>(orc_CanAddressInformation);

   if (q_ReservedBitSet == true)
   {
      c_Text = "RESBIT=1 ";
   }

   c_Text += ("[" + m_GetByteAsStringFormat(u8_DataPoolIndex) + "." +
              m_GetByteAsStringFormat(u8_ListIndex) + "." +
              m_GetWordAsStringFormat(u16_ElementIndex) + "]");

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disassemble CAN-ID

   \param[in]      ou32_CanId                  CAN-ID to decode
   \param[out]     opr_CanAddressInformation   store decoded CAN configuration

   \return
   C_NO_ERR    disassembly successful
   C_CONFIG    invalid addressing/routing configuration
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CanMonProtocolOpenSyde::mh_DisassembleCanId(const uint32_t ou32_CanId,
                                                      T_CanAddressInformation & or_CanAddressInformation)
{
   int32_t s32_Return = C_NO_ERR;

   /*      |   P   | R  | DP |    PF    |   PS   |   SA   |
      bit  | 28-26 | 25 | 24 |   23-16  |  15-8  |   7-0  |
           |  110  |  0 |  0 | 1101101x | TrgAdr | SrcAdr |  // fixed addressing

           |   P   | R  | DP |       |        PS       |        SA       |
      bit  | 28-26 | 25 | 24 | 23-22 |  21-18 | 17-11  |  10-7  |  6-0   |
           |  110  |  1 |  1 |   11  | SrcBus | SrcAdr | TrgBus | TrgAdr |   // routed communication
   */
   // routing
   if (((ou32_CanId & CAN_ID_R) == CAN_ID_R) && ((ou32_CanId & CAN_ID_DP) == CAN_ID_DP))
   {
      or_CanAddressInformation.t_NodeIdSource.u8_Subnet = static_cast<uint8_t>((ou32_CanId >> 18U) & 0x0F);
      or_CanAddressInformation.t_NodeIdSource.u8_NodeId = static_cast<uint8_t>((ou32_CanId >> 11U) & 0x7F);
      or_CanAddressInformation.t_NodeIdTarget.u8_Subnet = static_cast<uint8_t>((ou32_CanId >>  7U) & 0x0F);
      or_CanAddressInformation.t_NodeIdTarget.u8_NodeId = static_cast<uint8_t>((ou32_CanId >>  0U) & 0x7F);
      or_CanAddressInformation.u8_AddressMode            = OSY_CTP_ADDR_MODE_PHYSICAL; // unused
      or_CanAddressInformation.u8_RoutingMode            = OSY_CTP_ROUTING_ACTIVE;
      // convert remote broadcast address 0x7F -> 0xFF
      if (or_CanAddressInformation.t_NodeIdTarget.u8_NodeId == 0x7FU)
      {
         or_CanAddressInformation.t_NodeIdTarget.u8_NodeId = 0xFFU;
      }
   }
   // no routing
   else if (((ou32_CanId & CAN_ID_R) == 0U) && ((ou32_CanId & CAN_ID_DP) == 0U))
   {
      if ((ou32_CanId & CAN_ID_PF__N_TA_TYPE_6) == CAN_ID_PF__N_TA_TYPE_6)
      {
         or_CanAddressInformation.u8_AddressMode = OSY_CTP_ADDR_MODE_FUNCTIONAL;
      }
      else if ((ou32_CanId & CAN_ID_PF__N_TA_TYPE_5) == CAN_ID_PF__N_TA_TYPE_5)
      {
         or_CanAddressInformation.u8_AddressMode = OSY_CTP_ADDR_MODE_PHYSICAL;
      }
      // unsupported
      else
      {
         s32_Return = C_CONFIG;
      }

      or_CanAddressInformation.t_NodeIdTarget.u8_NodeId = static_cast<uint8_t>((ou32_CanId >> 8U) & 0xFFU);
      or_CanAddressInformation.t_NodeIdSource.u8_NodeId = static_cast<uint8_t>((ou32_CanId >> 0U) & 0xFFU);
      or_CanAddressInformation.t_NodeIdTarget.u8_Subnet = 0U;
      or_CanAddressInformation.t_NodeIdSource.u8_Subnet = 0U;
      or_CanAddressInformation.u8_RoutingMode           = OSY_CTP_ROUTING_INACTIVE;
   }
   // unsupported
   else
   {
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    constructor

   Initialize class members
*/
//----------------------------------------------------------------------------------------------------------------------
C_CanMonProtocolOpenSyde::C_CanMonProtocolOpenSyde(void) :
   C_CanMonProtocolBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert openSYDE service identifier to text

   Assumptions (responsibility of caller):
   * ou8_ServiceSize must be >= 1
   * opu8_ServiceData must be valid

   \param[in]     opu8_ServiceData        service data (starting with service identifier)
   \param[in]     ou8_ServiceSize         number of bytes in opu8_ServiceData (1..6)
   \param[in]     oq_IsNegativeResponse   true: report as negative response

   \return
   Text interpretation of service identifier
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_ServiceIdToText(const uint8_t ou8_ServiceData,
                                                        const bool oq_IsNegativeResponse) const
{
   const bool q_IsResponse = ((ou8_ServiceData & 0x40U) == 0x40U) ? true : false;
   const uint8_t u8_Service = (ou8_ServiceData & 0xBFU);
   C_SclString c_Text;

   if (oq_IsNegativeResponse == true)
   {
      c_Text = "NEGRES ";
   }
   else if (q_IsResponse == true)
   {
      c_Text = "RES ";
   }
   else
   {
      c_Text = "REQ ";
   }

   switch (u8_Service)
   {
   case OSY_UDS_SI_DIAGNOSTIC_SESSION_CONTROL:
      c_Text += "DiagnosticSessionControl";
      break;
   case OSY_UDS_SI_ECU_RESET:
      c_Text += "EcuReset";
      break;
   case OSY_UDS_SI_READ_DATA_BY_IDENTIFIER:
      c_Text += "ReadDataByIdentifier";
      break;
   case OSY_UDS_SI_READ_MEMORY_BY_ADDRESS:
      c_Text += "ReadMemoryByAddress";
      break;
   case OSY_UDS_SI_SECURITY_ACCESS:
      c_Text += "SecurityAccess";
      break;
   case OSY_UDS_SI_WRITE_DATA_BY_IDENTIFIER:
      c_Text += "WriteDataByIdentifier";
      break;
   case OSY_UDS_SI_ROUTINE_CONTROL:
      c_Text += "RoutineControl";
      break;
   case OSY_UDS_SI_REQUEST_DOWNLOAD:
      c_Text += "RequestDownload";
      break;
   case OSY_UDS_SI_TRANSFER_DATA:
      c_Text += "TransferData";
      break;
   case OSY_UDS_SI_REQUEST_TRANSFER_EXIT:
      c_Text += "RequestTransferExit";
      break;
   case OSY_UDS_SI_REQUEST_FILE_TRANSFER:
      c_Text += "RequestFileTransfer";
      break;
   case OSY_UDS_SI_WRITE_MEMORY_BY_ADDRESS:
      c_Text += "RequestWriteMemoryByAddress";
      break;
   case OSY_UDS_SI_TESTER_PRESENT:
      c_Text += "TesterPresent";
      break;
   case OSY_DPD_SI_OS_READ_DATAPOOLDATA_EVENT_DRIVEN:
      c_Text += "ReadDataPoolDataEventDriven";
      break;
   case OSY_DPD_SI_OS_READ_DATAPOOLDATA_BY_IDENTIFIER:
      c_Text += "ReadDataPoolDataByIdentifier";
      break;
   case OSY_DPD_SI_OS_WRITE_DATAPOOLDATA_BY_IDENTIFIER:
      c_Text += "WriteDataPoolDataByIdentifier";
      break;
   case OSY_UDC_B_SI_READ_SERIAL_NUMBER:
      c_Text += "ReadSerialNumber";
      break;
   case OSY_UDC_B_SI_SS_CAN_TUNNELING:
      c_Text += "TunnelCanMessages";
      break;
   default:
      c_Text += "UnknownServiceId (" + this->m_GetValueDecHex(u8_Service) + ")";
      break;
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert openSYDE session identifier to text

   \param[in]     ou8_SessionId        session ID

   \return
   Text interpretation of session ID
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_SessionIdToText(const uint8_t ou8_SessionId) const
{
   C_SclString c_Text;

   switch (ou8_SessionId)
   {
   case 0x01U:
      c_Text = "default";
      break;
   case 0x03U:
      c_Text = "extended";
      break;
   case 0x60U:
      c_Text = "preProgramming";
      break;
   case 0x02U:
      c_Text = "programming";
      break;
   default:
      c_Text = "unknown(" + m_GetValueDecHex(ou8_SessionId) + ")";
      break;
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert openSYDE service identifier data to text

   Assumptions (responsibility of caller):
   * ou8_ServiceSize must be >= 1
   * opu8_ServiceData must be valid
   * we handle either a request or a positive response

   \param[in]     opu8_ServiceData           service data (starting with service identifier)
   \param[in]     ou8_ServiceSize            number of bytes in opu8_ServiceData (1..6)
   \param[in]     orc_CanAddressInformation  information about nodes involved in communication

   \return
   Text interpretation of service identifier
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_ServiceDataToText(const uint8_t * const opu8_ServiceData,
                                                          const uint8_t ou8_ServiceSize,
                                                          const T_CanAddressInformation & orc_CanAddressInformation,
                                                          const bool oq_IsSingleFrame) const
{
   const bool q_IsResponse = ((opu8_ServiceData[0] & 0x40U) == 0x40U) ? true : false;
   const uint8_t u8_Service = (opu8_ServiceData[0] & 0xBFU);
   C_SclString c_Text;
   uint8_t u8_FirstRawByte = 1U; //index of first raw data byte to handle (to be displayed as dump data)

   switch (u8_Service)
   {
   case OSY_UDS_SI_READ_DATA_BY_IDENTIFIER: //no break
   case OSY_UDS_SI_WRITE_DATA_BY_IDENTIFIER:
      if (ou8_ServiceSize < 3U)
      {
         c_Text = " error: DLC too short (no dataIdentifier)";
      }
      else
      {
         c_Text = ":" + m_DataIdentifierAndDataToText(u8_Service == OSY_UDS_SI_WRITE_DATA_BY_IDENTIFIER,
                                                      mh_BytesToWordHighLow(&opu8_ServiceData[1]), q_IsResponse,
                                                      ou8_ServiceSize - 3U, &opu8_ServiceData[3]);
      }
      u8_FirstRawByte = ou8_ServiceSize; //complete interpretation is in subfunction; don't do it here ...
      break;
   case OSY_DPD_SI_OS_READ_DATAPOOLDATA_BY_IDENTIFIER: //no break
   case OSY_DPD_SI_OS_WRITE_DATAPOOLDATA_BY_IDENTIFIER:
      if (ou8_ServiceSize < 4U)
      {
         c_Text = " error: DLC too short (no dataPoolDataIdentifier)";
      }
      else
      {
         const uint32_t u32_DpIndex = (static_cast<uint32_t>(opu8_ServiceData[1]) << 16U) +
                                      (static_cast<uint32_t>(opu8_ServiceData[2]) << 8U) +
                                      opu8_ServiceData[3];
         c_Text += m_DataPoolIdentifierToText(u32_DpIndex, q_IsResponse, orc_CanAddressInformation);
      }
      u8_FirstRawByte = 4U;
      break;
   case OSY_DPD_SI_OS_READ_DATAPOOLDATA_EVENT_DRIVEN:
      if (q_IsResponse == false)
      {
         if (ou8_ServiceSize < 2U)
         {
            c_Text = " error: DLC too short (no transmissionMode)";
         }
         else
         {
            c_Text += " MODE:";
            if (opu8_ServiceData[1] == 0)
            {
               c_Text += "stopSending";
               u8_FirstRawByte = 2U;
            }
            else
            {
               bool q_IsChange = false;
               switch (opu8_ServiceData[1])
               {
               case 1:
                  c_Text += "cyclic rail1";
                  break;
               case 2:
                  c_Text += "cyclic rail2";
                  break;
               case 3:
                  c_Text += "cyclic rail3";
                  break;
               case 4:
                  c_Text += "change rail1";
                  q_IsChange = true;
                  break;
               case 5:
                  c_Text += "change rail2";
                  q_IsChange = true;
                  break;
               case 6:
                  c_Text += "change rail3";
                  q_IsChange = true;
                  break;
               default:
                  c_Text += "undefined";
                  break;
               }
               if (ou8_ServiceSize < 5U)
               {
                  c_Text += " error: DLC too short (no dataPoolDataIdentifier)";
               }
               else
               {
                  const uint32_t u32_DpIndex = (static_cast<uint32_t>(opu8_ServiceData[2]) << 16U) +
                                               (static_cast<uint32_t>(opu8_ServiceData[3]) << 8U) +
                                               opu8_ServiceData[4];
                  c_Text += m_DataPoolIdentifierToText(u32_DpIndex, q_IsResponse, orc_CanAddressInformation);
                  if (q_IsChange == true)
                  {
                     if (ou8_ServiceSize < 6U)
                     {
                        c_Text += " error: DLC too short (no changeDrivenThreshold)";
                     }
                     else
                     {
                        //we only have part of the threshold in the first frame; only display raw data ...
                        u8_FirstRawByte = 5U;
                     }
                  }
                  else
                  {
                     u8_FirstRawByte = 5U;
                  }
               }
            }
         }
      }
      else
      {
         //A response to "stopSending" will not contain any more data.
         //Otherwise we expect the element index:
         if (ou8_ServiceSize >= 4U)
         {
            const uint32_t u32_DpIndex = (static_cast<uint32_t>(opu8_ServiceData[1]) << 16U) +
                                         (static_cast<uint32_t>(opu8_ServiceData[2]) << 8U) +
                                         opu8_ServiceData[3];
            c_Text += m_DataPoolIdentifierToText(u32_DpIndex, q_IsResponse, orc_CanAddressInformation);
            u8_FirstRawByte = 4U; //display data in raw format
         }
         else
         {
            u8_FirstRawByte = 1U; //display data in raw format
         }
      }
      break;
   case OSY_UDS_SI_ROUTINE_CONTROL:
      if (ou8_ServiceSize < 4U)
      {
         c_Text = " error: DLC too short (no subFunction and/or routineIdentifier)";
      }
      else
      {
         switch (opu8_ServiceData[1])
         {
         case 1:
            c_Text = ":start";
            break;
         case 2:
            c_Text = ":stop";
            break;
         case 3:
            c_Text = ":requestresults";
            break;
         default:
            c_Text = ":undefinedsubfunction(" + m_GetValueDecHex(opu8_ServiceData[1]) + ")";
            break;
         }
         c_Text += (":" + m_RoutineDataToText(mh_BytesToWordHighLow(&opu8_ServiceData[2]), q_IsResponse,
                                              ou8_ServiceSize - 4, &opu8_ServiceData[4]));
      }
      u8_FirstRawByte = ou8_ServiceSize; //complete interpretation is in subfunction; don't do it here ...
      break;
   case OSY_UDS_SI_DIAGNOSTIC_SESSION_CONTROL:
      if (ou8_ServiceSize < 2U)
      {
         c_Text = " error: DLC too short (no sessionIdentifier)";
      }
      else
      {
         const bool q_ExpectResponse = ((opu8_ServiceData[1] & 0x80U) == 0) ? true : false;
         const uint8_t u8_Value = opu8_ServiceData[1] & 0x7FU;
         if (q_ExpectResponse == false)
         {
            c_Text += " SUPP_RSP ";
         }
         c_Text += " SESSION:" + m_SessionIdToText(u8_Value);
      }
      u8_FirstRawByte = 2U;
      break;
   case OSY_UDS_SI_TESTER_PRESENT:
      if (ou8_ServiceSize < 2U)
      {
         c_Text = " error: DLC too short (no sub-function)";
      }
      else
      {
         const bool q_ExpectResponse = ((opu8_ServiceData[1] & 0x80U) == 0) ? true : false;
         const uint8_t u8_Value = opu8_ServiceData[1] & 0x7FU;
         if (q_ExpectResponse == false)
         {
            c_Text += " SUPP_RSP ";
         }
         c_Text += " SUBFUNC:" + m_GetByteAsStringFormat(u8_Value);
      }
      u8_FirstRawByte = 2U;
      break;
   case OSY_UDC_B_SI_READ_SERIAL_NUMBER:
      if (q_IsResponse == false)
      {
         if (ou8_ServiceSize == 3U)
         {
            // Special case: Extended request
            c_Text += "Ext";
            c_Text += "  BlockNumber: " + C_SclString::IntToStr(opu8_ServiceData[1]);
            // Byte 3 is reserved
            u8_FirstRawByte = ou8_ServiceSize; //everything displayed ...
         }
         else if (ou8_ServiceSize == 1U)
         {
            // Standard format, nothing to interpret
         }
         else
         {
            c_Text = " error: unexpected DLC received";
         }
      }
      else
      {
         if (ou8_ServiceSize != 7U)
         {
            c_Text = " error: DLC too short (no full serial number)";
         }
         else
         {
            // For the response it is not possible to tell the difference between standard and extended format
            C_SclString c_Snr;
            const uint8_t u8_BlockNumber = opu8_ServiceData[1] & 0x0FU;
            const uint8_t u8_SubNodeId = static_cast<uint8_t>((opu8_ServiceData[1] & 0xF0U) >> 4U);
            const uint32_t u32_UniqueId = (static_cast<uint32_t>(opu8_ServiceData[2]) << 16U) +
                                          (static_cast<uint32_t>(opu8_ServiceData[3]) << 8U) +
                                          static_cast<uint32_t>(opu8_ServiceData[4]);

            // Interpret as standard format with POS
            c_Snr = this->mh_SerialNumberToString(&opu8_ServiceData[1]);
            c_Text += "  Std SNR POS: " + c_Snr;

            c_Text += "  Ext SNR BlockNumber: " + C_SclString::IntToStr(u8_BlockNumber);
            c_Text += "  SubNodeId: " + C_SclString::IntToStr(u8_SubNodeId);
            c_Text += "  UniqueId: " + C_SclString::IntToHex(u32_UniqueId, 6);

            if (u8_BlockNumber == 0)
            {
               // First byte reserved
               c_Text += "  Manufacturer Format: " + C_SclString::IntToStr(opu8_ServiceData[6]);
            }
            else if (u8_BlockNumber == 1)
            {
               C_SclString c_Text2;

               c_Text += "  Length: " + C_SclString::IntToStr(opu8_ServiceData[5]);
               c_Text += "  SNR FSN First sign: ";
               c_Text += static_cast<char_t>(opu8_ServiceData[6]);

               // Potential STW BCD packed in extended service
               c_Text2.PrintFormatted("  SNR POS First signs: %02x", opu8_ServiceData[6]);
               c_Text += c_Text2;
            }
            else
            {
               C_SclString c_Text2;

               // The serial number itself
               c_Text += "  SNR FSN Part: ";
               c_Text += static_cast<char_t>(opu8_ServiceData[5]);
               if (opu8_ServiceData[6] != 0U)
               {
                  // Last byte is 0 if the FSN string is not a multiple of 2
                  c_Text += static_cast<char_t>(opu8_ServiceData[6]);
               }

               // Potential STW BCD packed in extended service
               if (u8_BlockNumber <= 3U)
               {
                  c_Text2.PrintFormatted("  SNR POS Part: %02x%02x", opu8_ServiceData[5], opu8_ServiceData[6]);
                  c_Text += c_Text2;
               }
               else if (u8_BlockNumber == 4U)
               {
                  c_Text2.PrintFormatted("  SNR POS Last signs: %02x", opu8_ServiceData[5]);
                  c_Text += c_Text2;
               }
               else
               {
                  // Nothing to do, POS format would be finished already
               }
            }

            u8_FirstRawByte = 8U; //everything displayed ...
         }
      }
      break;
   case OSY_UDS_SI_READ_MEMORY_BY_ADDRESS: //no break
   case OSY_UDS_SI_WRITE_MEMORY_BY_ADDRESS:
      if ((q_IsResponse == false) || (u8_Service == OSY_UDS_SI_WRITE_MEMORY_BY_ADDRESS))
      {
         //READ, WRITE requests and WRITE response have the same format
         if (ou8_ServiceSize < 2U)
         {
            c_Text = " error: DLC too short (no address format identifier)";
         }
         else
         {
            const uint8_t u8_AddressFormatter = opu8_ServiceData[1];
            const uint8_t u8_NumBytesAddress = (u8_AddressFormatter & 0x0FU);
            const uint8_t u8_NumBytesSize = (u8_AddressFormatter >> 4U);

            uint8_t u8_ExpectedSize = 2U + u8_NumBytesAddress + u8_NumBytesSize;
            if (u8_ExpectedSize > 7U)
            {
               u8_ExpectedSize = 7U;
            }
            u8_FirstRawByte = u8_ExpectedSize;
            if (ou8_ServiceSize < u8_ExpectedSize)
            {
               c_Text = " error: DLC too short (missing or incomplete address and/or memory size)";
            }
            else
            {
               c_Text = "  ADD:";
               for (uint8_t u8_Index = 0U; u8_Index < u8_NumBytesAddress; u8_Index++)
               {
                  c_Text += m_GetByteAsStringFormat(opu8_ServiceData[2U + u8_Index]);
                  if (u8_Index != (u8_NumBytesAddress - 1))
                  {
                     c_Text += ",";
                  }
               }
               c_Text += "  SIZE:";
               for (uint8_t u8_Index = 0U; u8_Index < u8_NumBytesSize; u8_Index++)
               {
                  if (u8_Index <= u8_ExpectedSize)
                  {
                     c_Text += m_GetByteAsStringFormat(opu8_ServiceData[2U + u8_NumBytesAddress + u8_Index]);
                  }
                  if ((u8_Index < (u8_NumBytesSize - 1)) && (u8_Index < u8_ExpectedSize))
                  {
                     c_Text += ",";
                  }
               }
            }
         }
      }
      break;
   case OSY_UDS_SI_ECU_RESET:
      if (ou8_ServiceSize < 2U)
      {
         c_Text = " error: DLC too short (no reset type)";
      }
      else
      {
         switch (opu8_ServiceData[1])
         {
         case 0x02U:
            c_Text = "  TYPE:KEY_OFF_ON";
            break;
         case 0x60U:
            c_Text = "  TYPE:TO_FLASHLOADER";
            break;
         default:
            c_Text = "  TYPE:UNDEFINED";
            break;
         }
      }
      u8_FirstRawByte = 2U;
      break;
   case OSY_UDS_SI_SECURITY_ACCESS:
      if (ou8_ServiceSize < 2U)
      {
         c_Text = " error: DLC too short (no access type)";
      }
      else
      {
         if (ou8_ServiceSize > 2U)
         {
            c_Text = "  MODE:";
            if (oq_IsSingleFrame == false)
            {
               c_Text += "secure";
            }
            else
            {
               c_Text += "non-secure";
            }
         }
         else
         {
            c_Text = "";
         }
         if (opu8_ServiceData[1] == 0U)
         {
            c_Text += "  TYPE:INVALID";
         }
         else
         {
            c_Text += "  TYPE:";
            if ((opu8_ServiceData[1] & 0x01U) == 0x01U)
            {
               c_Text += "  REQUEST_SEED  LEVEL:" + m_GetByteAsStringFormat(opu8_ServiceData[1]);
            }
            else
            {
               c_Text += "  SEND_KEY  LEVEL:" + m_GetByteAsStringFormat(opu8_ServiceData[1] - 1U);
            }

            if (ou8_ServiceSize > 2U)
            {
               //we expect a 4 byte seed or key
               if (ou8_ServiceSize < 6U)
               {
                  c_Text += " error: DLC too short (no full seed or key)";
               }
               else
               {
                  const uint32_t u32_Value = (static_cast<uint32_t>(opu8_ServiceData[2]) << 24U) +
                                             (static_cast<uint32_t>(opu8_ServiceData[3]) << 16U) +
                                             (static_cast<uint32_t>(opu8_ServiceData[4]) << 8U) +
                                             opu8_ServiceData[5];
                  c_Text += "  VALUE:" + m_GetValueDecHex(u32_Value);
               }
            }
         }
      }
      u8_FirstRawByte = ou8_ServiceSize;
      break;
   case OSY_UDS_SI_REQUEST_DOWNLOAD:
      //no specifics reported for now
      break;
   case OSY_UDS_SI_TRANSFER_DATA:
      if (ou8_ServiceSize < 2U)
      {
         c_Text = " error: DLC too short (no block sequence counter)";
      }
      else
      {
         c_Text = "  BLK:" + m_GetByteAsStringFormat(opu8_ServiceData[1]);
      }
      u8_FirstRawByte = 2U;
      break;
   case OSY_UDS_SI_REQUEST_TRANSFER_EXIT:
      //no specifics reported for now
      break;
   case OSY_UDS_SI_REQUEST_FILE_TRANSFER:
      //no specifics reported for now
      break;
   default:
      break;
   }

   if ((ou8_ServiceSize - u8_FirstRawByte) > 0) //we have data byte(s) left to display in raw format ...
   {
      uint8_t u8_Index;
      c_Text += "  DATA[";
      for (u8_Index = u8_FirstRawByte; u8_Index < ou8_ServiceSize; u8_Index++)
      {
         c_Text += m_GetByteAsStringFormat(opu8_ServiceData[u8_Index]);
         if (u8_Index != (ou8_ServiceSize - 1))
         {
            c_Text += ",";
         }
      }
      c_Text += "]";
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert openSYDE negative response code to text

   \param[in]     ou8_NrCode      negative response code

   \return
   Text interpretation of negative response code
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_NegativeResponseCodeToText(const uint8_t ou8_NrCode) const
{
   C_SclString c_Text;

   switch (ou8_NrCode)
   {
   case OSY_UDS_NRC_NO_ERROR:
      c_Text = "positive response";
      break;
   case OSY_UDS_NRC_SERVICE_NOT_SUPPORTED:
      c_Text = "serviceNotSupported";
      break;
   case OSY_UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED:
      c_Text = "sub-functionNotSupported";
      break;
   case OSY_UDS_NRC_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT:
      c_Text = "incorrectMessageLengthOrInvalidFormat";
      break;
   case OSY_UDS_NRC_RESPONSE_TOO_LONG:
      c_Text = "responseTooLong";
      break;
   case OSY_UDS_NRC_CONDITIONS_NOT_CORRECT:
      c_Text = "conditionsNotCorrect";
      break;
   case OSY_UDS_NRC_REQUEST_SEQUENCE_ERROR:
      c_Text = "requestSequenceError";
      break;
   case OSY_UDS_NRC_REQUEST_OUT_OF_RANGE:
      c_Text = "requestOutOfRange";
      break;
   case OSY_UDS_NRC_SECURITY_ACCESS_DENIED:
      c_Text = "securityAccessDenied";
      break;
   case OSY_UDS_NRC_INVALID_KEY:
      c_Text = "invalidKey";
      break;
   case OSY_UDS_NRC_EXCEEDED_NUMBER_OF_ATTEMPTS:
      c_Text = "exceededNumberOfAttempts";
      break;
   case OSY_UDS_NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED:
      c_Text = "requiredTimeDelayNotExpired";
      break;
   case OSY_UDS_NRC_UPLOAD_DOWNLOAD_NOT_ACCEPTED:
      c_Text = "uploadDownloadNotAccepted";
      break;
   case OSY_UDS_NRC_TRANSFER_DATA_SUSPENDED:
      c_Text = "transferDataSuspended";
      break;
   case OSY_UDS_NRC_GENERAL_PROGRAMMING_FAILURE:
      c_Text = "generalProgrammingFailure";
      break;
   case OSY_UDS_NRC_WRONG_BLOCK_SEQUENCE_COUNTER:
      c_Text = "wrongBlockSequenceCounter";
      break;
   case OSY_UDS_NRC_RESPONSE_PENDING:
      c_Text = "requestCorrectlyReceivedResponsePending";
      break;
   case OSY_UDS_NRC_SUBFUNCTION_NOT_SUPPORTED_IN_ACTIVE_SESSION:
      c_Text = "sub-functionNotSupportedInActiveSession";
      break;
   case OSY_UDS_NRC_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION:
      c_Text = "serviceNotSupportedInActiveSession";
      break;
   case OSY_UDS_NRC_VOLTAGE_TOO_HIGH:
      c_Text = "voltageTooHigh";
      break;
   case OSY_UDS_NRC_VOLTAGE_TOO_LOW:
      c_Text = "voltageTooLow";
      break;
   default:
      c_Text = "unknownnegativeresponsecode (" + m_GetValueDecHex(ou8_NrCode) + ")";
      break;
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data as ASCII text if representable

   Will add printable characters as such.
   Will insert non-printable characters as value.

   \param[in]     ou8_NumBytes          number of data bytes (in opu8_Data)
   \param[in]     opu8_Data             payload data

   \return
   Text interpretation of data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_GetSupposedlyAsciiText(const uint8_t ou8_NumBytes,
                                                               const uint8_t * const opu8_Data)
const
{
   C_SclString c_Text = "\"";
   C_SclString c_Text2;
   uint8_t u8_Index;

   for (u8_Index = 0U; u8_Index < ou8_NumBytes; u8_Index++)
   {
      if (std::isprint(opu8_Data[u8_Index]) == 0)
      {
         c_Text2.PrintFormatted("[\\x%02X]", opu8_Data[u8_Index]);
      }
      else
      {
         (void)c_Text2.PrintFormatted("%c", opu8_Data[u8_Index]);
      }
      c_Text += c_Text2;
   }
   c_Text += "\"";

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert three byte version to string

   Always prints as decimal

   \param[in]     opu8_Version          version in three byte binary format

   \return
   Text interpretation of data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::mh_ThreeByteVersionToString(const uint8_t * const opu8_Version)
{
   C_SclString c_Text;

   c_Text.PrintFormatted("%d.%02dr%d", opu8_Version[0], opu8_Version[1], opu8_Version[2]);
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert openSYDE data identifier code and data to text

   \param[in]     ou16_DataIdentifier      data identifier
   \param[in]     oq_IsResponse            false: decode request
                                           true: decode response
   \param[in]     ou8_PayloadSize          number of data bytes (in opu8_Payload)
   \param[in]     opu8_Payload             payload data

   \return
   Text interpretation of data identifier
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_DataIdentifierAndDataToText(const bool oq_IsWrite,
                                                                    const uint16_t ou16_DataIdentifier,
                                                                    const bool oq_IsResponse,
                                                                    const uint8_t ou8_PayloadSize,
                                                                    const uint8_t * const opu8_Payload) const
{
   C_SclString c_Text;
   uint8_t u8_FirstRawByte = 0U;

   switch (ou16_DataIdentifier)
   {
   case DPD_DATAID_LIST_OF_FEATURES:
      c_Text = "ListOfFeatures";
      break;
   case DPD_DATAID_MAX_NUMBER_OF_BLOCK_LENGTH:
      c_Text = "MaxNumberOfBlockLength";
      break;
   case DPD_DATAID_DATARATE1:
      c_Text = "DataRate1";
      break;
   case DPD_DATAID_DATARATE2:
      c_Text = "DataRate2";
      break;
   case DPD_DATAID_DATARATE3:
      c_Text = "DataRate3";
      break;
   case UDC_H_DATAID_PROTOCOL_VERSION:
      c_Text = "ProtocolVersion";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 3U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += ("  VERSION:" + mh_ThreeByteVersionToString(&opu8_Payload[0]));
            u8_FirstRawByte = ou8_PayloadSize; //finished here ...
         }
      }
      break;
   case DPD_DATAID_PROTOCOL_DRIVER_IMPLEMENTATION_VERSION:
      c_Text = "ProtocolDriverImplementationVersion";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 3U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += ("  VERSION:" + mh_ThreeByteVersionToString(&opu8_Payload[0]));
            u8_FirstRawByte = ou8_PayloadSize; //finished here ...
         }
      }
      break;
   case FL_H_DATAID_FLASHLOADER_PROTOCOL_VERSION:
      c_Text = "FlashloaderProtocolVersion";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 3U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += ("  VERSION:" + mh_ThreeByteVersionToString(&opu8_Payload[0]));
            u8_FirstRawByte = ou8_PayloadSize; //finished here ...
         }
      }
      break;
   case DPD_DATAID_MAX_NUM_ASYNC:
      c_Text = "MaximumNumberOfEventDrivenTransmissions";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 2U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += ("  MAX_NUMBER:" + m_GetValueDecHex(mh_BytesToWordHighLow(&opu8_Payload[0])));
            u8_FirstRawByte = 2U; //finished here ...
         }
      }
      break;
   case FL_H_DATAID_FLASH_COUNT:
      c_Text = "FlashCount";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 4U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += ("  FLASH_COUNT:" + m_GetValueDecHex(mh_BytesToDwordHighLow(&opu8_Payload[0])));
            u8_FirstRawByte = ou8_PayloadSize; //finished here ...
         }
      }
      break;
   case UDC_H_DATAID_DEVICE_NAME:
      c_Text = "DeviceName";
      if (oq_IsResponse == true)
      {
         c_Text += ("  DEV_NAME:" + m_GetSupposedlyAsciiText(ou8_PayloadSize, opu8_Payload));
         u8_FirstRawByte = ou8_PayloadSize; //finished here ...
      }
      break;
   case DPD_DATAID_APPLICATION_NAME:
      c_Text = "ApplicationName";
      if (oq_IsResponse == true)
      {
         c_Text += ("  APP_NAME:" + m_GetSupposedlyAsciiText(ou8_PayloadSize, opu8_Payload));
         u8_FirstRawByte = ou8_PayloadSize; //finished here ...
      }
      break;
   case DPD_DATAID_APPLICATION_VERSION:
      c_Text = "ApplicationVersion";
      if (oq_IsResponse == true)
      {
         c_Text += ("  APP_VERSION:" + m_GetSupposedlyAsciiText(ou8_PayloadSize, opu8_Payload));
         u8_FirstRawByte = ou8_PayloadSize; //finished here ...
      }
      break;
   case DPD_DATAID_FILE_BASED_TRANSFER_EXIT_RESULT:
      c_Text = "FileBasedTransferExitResult";
      break;
   case UDC_H_DATAID_BOOTSOFTWARE_IDENTIFICATION:
      c_Text = "BootSoftwareIdentification";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 4U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += "  MODULES:" + m_GetValueDecHex(opu8_Payload[0]) + "  VERSION:" +
                      mh_ThreeByteVersionToString(&opu8_Payload[1]);
            u8_FirstRawByte = ou8_PayloadSize; //finished here ...
         }
      }
      break;
   case UDC_H_DATAID_SUB_NODE_ID:
      c_Text = "SubNodeId";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 1U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += "  SubNodeId: " + m_GetByteAsStringFormat(opu8_Payload[0]);
            u8_FirstRawByte = ou8_PayloadSize; //finished here ...
         }
      }
      break;
   case UDC_H_DATAID_CERTIFICATE_SERIAL_NUMBER:
      c_Text = "CertificateSerialNumber";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 3U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            C_SclString c_Text2;
            c_Text2.PrintFormatted("  CERTIFICATESNR:%02x:%02x:%02x", opu8_Payload[0], opu8_Payload[1],
                                   opu8_Payload[2]);

            c_Text += c_Text2;
            u8_FirstRawByte = 3U; //finished here ...
         }
      }
      break;
   case UDC_H_DATAID_SECURITY_ACTIVATION:
      c_Text = "SecurityActivation";
      if (((oq_IsResponse == true) && (oq_IsWrite == false)) ||
          ((oq_IsResponse == false) && (oq_IsWrite == true)))
      {
         if (ou8_PayloadSize != 2U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            if ((opu8_Payload[0] & 0x01) == 0x01)
            {
               // security on
               c_Text += "  ACTIVATION:on";
            }
            else
            {
               // security off
               c_Text += "  ACTIVATION:off";
            }

            if (opu8_Payload[1] == 0)
            {
               c_Text += "  ALGO:RSA_1024_WITH_PKCS1_PADDING";
            }
            else
            {
               c_Text += "  ALGO:" + m_GetByteAsStringFormat(opu8_Payload[1]);
            }
            u8_FirstRawByte = 2U; //finished here ...
         }
      }
      break;
   case UDC_H_DATAID_DEBUGGER_ACTIVATION:
      c_Text = "DebuggerActivation";
      if (((oq_IsResponse == true) && (oq_IsWrite == false)) ||
          ((oq_IsResponse == false) && (oq_IsWrite == true)))
      {
         if (ou8_PayloadSize != 1U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            if ((opu8_Payload[0] & 0x01) == 0x01)
            {
               // security on
               c_Text += "  DEBUGGER:activated";
            }
            else
            {
               // security off
               c_Text += "  DEBUGGER:deactivated";
            }
            u8_FirstRawByte = 1U; //finished here ...
         }
      }
      break;
   case UDC_H_DATAID_SECURITY_KEY:
      c_Text = "SecurityKey";
      if (oq_IsResponse == false)
      {
         if (ou8_PayloadSize != 3U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            C_SclString c_Text2;
            c_Text2.PrintFormatted("  PUBLICKEY:%02x:%02x:%02x", opu8_Payload[0], opu8_Payload[1], opu8_Payload[2]);

            c_Text += c_Text2;
            u8_FirstRawByte = 3U; //finished here ...
         }
      }
      break;
   case FL_H_DATAID_SOFTWARE_FINGERPRINT:
      c_Text = "ApplicationSoftwareFingerPrint";
      break;
   case UDC_H_DATAID_ACTIVE_DIAGNOSTIC_SESSION:
      c_Text = "ActiveDiagnosticSession";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 1U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += ("  SESSION:" + m_SessionIdToText(opu8_Payload[0]));
            u8_FirstRawByte = 1U; //finished here ...
         }
      }
      break;
   case UDC_H_DATAID_ECU_SERIAL_NUMBER:
      c_Text = "EcuSerialNumber";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 3U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            C_SclString c_Text2;

            if (opu8_Payload[0] < static_cast<uint8_t>(0x20))
            {
               //format up to and including 2019. E.g: 05.123456.1001
               c_Text2.PrintFormatted("  SNR:%02x.%02x%02x", opu8_Payload[0], opu8_Payload[1], opu8_Payload[2]);
            }
            else
            {
               //format from 2020. E.g: 200012345678
               c_Text2.PrintFormatted("  SNR:%02x%02x%02x", opu8_Payload[0], opu8_Payload[1], opu8_Payload[2]);
            }

            c_Text += c_Text2;
            u8_FirstRawByte = 3U; //finished here ...
         }
      }
      break;
   case UDC_H_DATAID_ECU_SERIAL_NUMBER_EXT:
      c_Text = "EcuSerialNumberExtended";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize > 2U)
         {
            c_Text += "  SNR Manufacturer Format: " + C_SclString::IntToStr(opu8_Payload[0]);
            c_Text += "  SNR Length: " + C_SclString::IntToStr(opu8_Payload[1]);
            if (opu8_Payload[0] != 0U)
            {
               c_Text += "  SNR First sign: ";
               c_Text += static_cast<char_t>(opu8_Payload[2]);
            }
            else
            {
               C_SclString c_Text2;
               // STW BCD packed in extended service
               c_Text2.PrintFormatted("  SNR First signs: %02x", opu8_Payload[2]);
               c_Text += c_Text2;
            }
            u8_FirstRawByte = ou8_PayloadSize; //finished here ...
         }
         else
         {
            c_Text += " error: incorrect number of data bytes";
         }
      }

      break;
   case UDC_H_DATAID_SYSTEM_SUPPLIER_ECU_HW_NUMBER:
      c_Text = "SystemSupplierEcuHardwareNumber";
      if (oq_IsResponse == true)
      {
         if (ou8_PayloadSize != 4U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += ("  HW_NUMBER:" + m_GetValueDecHex(mh_BytesToDwordHighLow(&opu8_Payload[0])));
            u8_FirstRawByte = 4U; //finished here ...
         }
      }
      break;
   case UDC_H_DATAID_SYSTEM_SUPPLIER_ECU_HW_VERSION_NUMBER:
      c_Text = "SystemSupplierEcuHardwareVersionNumber";
      if (oq_IsResponse == true)
      {
         //Should be ASCII ...
         c_Text += ("  HW_VERSION_NUMBER:" + m_GetSupposedlyAsciiText(ou8_PayloadSize, opu8_Payload));
         u8_FirstRawByte = ou8_PayloadSize; //finished here ...
      }
      break;
   case UDC_H_DATAID_UDS_VERSION:
      c_Text = "UdsVersion";
      break;
   default:
      c_Text = "unknowndataidentifier (" + m_GetValueDecHex(ou16_DataIdentifier) + ")";
      break;
   }

   //there is more data we cannot interpret: display as raw
   if ((ou8_PayloadSize - u8_FirstRawByte) > 0) //do we have data byte(s) left to display in raw format ?
   {
      c_Text += m_RawDataToString(ou8_PayloadSize - u8_FirstRawByte, &opu8_Payload[u8_FirstRawByte]);
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert openSYDE routine identifier code to text

   \param[in]     ou16_RoutineIdentifier        routine identifier
   \param[in]     oq_IsResponse                 true: response flag was set
                                                false: response flag was not set (we have a request)
   \param[in]     ou8_DataSize                  number of bytes in payload
   \param[in]     opu8_Data                     payload of routine starting with the first byte after
                                                 the routine identifier (2 bytes in a FF, up to 3 bytes in a SF)

   \return
   Text interpretation of routine data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_RoutineDataToText(const uint16_t ou16_RoutineIdentifier,
                                                          const bool oq_IsResponse, const uint8_t ou8_DataSize,
                                                          const uint8_t * const opu8_Data) const
{
   C_SclString c_Text;
   uint8_t u8_FirstRawByte = 0U;

   switch (ou16_RoutineIdentifier)
   {
   case DPD_ROUTINEID_READ_DATAPOOL_METADATA:
      c_Text = "ReadDataPoolMetaData";
      //no details reported for now
      break;
   case DPD_ROUTINEID_VERIFY_DATAPOOL:
      //segmented transfer expected
      c_Text = "VerifyDataPool";
      if (ou8_DataSize < 2U)
      {
         c_Text += " error: incorrect number of data bytes";
      }
      else
      {
         if (oq_IsResponse == false)
         {
            c_Text += "  DATA_POOL:" + m_GetValueDecHex(opu8_Data[0]) +
                      "  MSB_CHECKSUM:" + m_GetValueDecHex(opu8_Data[1]);
         }
         else
         {
            c_Text += "  DATA_POOL:" + m_GetValueDecHex(opu8_Data[0]) + "  MATCH:";
            switch (opu8_Data[1])
            {
            case 0U:
               c_Text += "YES";
               break;
            case 1U:
               c_Text += "NO";
               break;
            default:
               c_Text += "UNDEFINED";
               break;
            }
         }
         u8_FirstRawByte = 2U; //finished here
      }
      break;
   case DPD_ROUTINEID_NOTIFY_NVM_CHANGED:
      c_Text = "NotifyNvmChanged";

      if (ou8_DataSize < 2U)
      {
         c_Text += " error: incorrect number of data bytes";
      }
      else
      {
         c_Text += "  DATA_POOL:" + m_GetValueDecHex(opu8_Data[0]) + "  LIST:" + m_GetValueDecHex(opu8_Data[1]);
         if (ou8_DataSize >= 3U)
         {
            switch (opu8_Data[2])
            {
            case 0:
               c_Text += "  ACK:OK";
               break;
            case 1:
               c_Text += "  ACK:NOK";
               break;
            default:
               c_Text += "  ACK:UNDEFINED";
               break;
            }
         }
         u8_FirstRawByte = ou8_DataSize; //finished here
      }
      break;
   case UDS_H_ROUTINE_CTRL_ROUTE_DIAGNOSIS_COMMUNICATION:
      c_Text = "RouteDiagnosisCommunication";
      //no details reported for now
      break;
   case UDS_H_ROUTINE_CTRL_SEND_CAN_MESSAGE:
      c_Text = "SendCanMessage";
      //no details reported for now
      break;
   case UDS_H_ROUTINE_CTRL_TUNNEL_CAN_MESSAGES:
      c_Text = "TunnelCanMessages";
      //no details reported for now
      break;
   case UDS_H_ROUTINE_CTRL_REQUEST_PROGRAMMING:
      c_Text = "RequestProgramming";
      //no further parameters
      break;
   case UDS_H_ROUTINE_CTRL_SET_BITRATE:
      //no details reported for now
      c_Text = "SetBitrate";
      break;
   case FL_H_ROUTINE_CTRL_CHECK_FLASH_MEMORY_AVAILABLE:
      //no details reported for now
      c_Text = "CheckFlashMemoryAvailable";
      break;
   case FL_H_ROUTINE_CTRL_READ_FLASHBLOCK_DATA:
      c_Text = "ReadFlashblockData";
      if (oq_IsResponse == false)
      {
         if (ou8_DataSize != 1U)
         {
            c_Text += " error: incorrect number of data bytes";
         }
         else
         {
            c_Text += ("  BLOCK_INDEX:" + m_GetValueDecHex(opu8_Data[0]));
            u8_FirstRawByte = ou8_DataSize; //finished here ...
         }
      }
      break;
   case UDS_H_ROUTINE_CTRL_CONFIG_FLALOAD_COMMUNICATION_CHANNEL:
      //no details reported for now
      c_Text = "ConfigFlashloaderCommunicationChannel";
      break;
   case UDS_H_ROUTINE_CTRL_SET_NODEID_FOR_CHANNEL:
      //no details reported for now
      c_Text = "SetNodeIdForChannel";
      break;
   case UDS_H_ROUTINE_CTRL_SET_IP_ADDRESS_FOR_CHANNEL:
      //no details reported for now
      c_Text = "SetIpAddressForChannel";
      break;
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_PART1:
      c_Text = "SetNodeIdBySerialNumberPart1";
      if (oq_IsResponse == false)
      {
         if (ou8_DataSize == 3U)
         {
            C_SclString c_Text2;
            c_Text2.PrintFormatted("  SNR POS First Part: %02x%02x%02x", opu8_Data[0], opu8_Data[1], opu8_Data[2]);
            c_Text += c_Text2;
            u8_FirstRawByte = ou8_DataSize; //finished here ...
         }
         else
         {
            c_Text += " error: incorrect number of data bytes";
         }
      }
      break;
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_PART2:
      c_Text = "SetNodeIdBySerialNumberPart2";
      if (oq_IsResponse == false)
      {
         if (ou8_DataSize == 3U)
         {
            C_SclString c_Text2;
            c_Text2.PrintFormatted("  SNR POS Last Part: %02x%02x%02x", opu8_Data[0], opu8_Data[1], opu8_Data[2]);
            c_Text += c_Text2;
            u8_FirstRawByte = ou8_DataSize; //finished here ...
         }
         else
         {
            c_Text += " error: incorrect number of data bytes";
         }
      }
      break;
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_PART3:
      c_Text = "SetNodeIdBySerialNumberPart3";
      if (oq_IsResponse == false)
      {
         if (ou8_DataSize == 2U)
         {
            c_Text += "  BusId: " + C_SclString::IntToStr(opu8_Data[0]);
            c_Text += "  NodeId: " + C_SclString::IntToStr(opu8_Data[1]);
            u8_FirstRawByte = ou8_DataSize; //finished here ...
         }
         else
         {
            c_Text += " error: incorrect number of data bytes";
         }
      }
      break;
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_1:
      c_Text = "SetNodeIdBySerialNumberExtPart1";
      if (oq_IsResponse == false)
      {
         if (ou8_DataSize == 3U)
         {
            // Byte 0 is reserved
            c_Text += "  BusId: " + C_SclString::IntToStr(opu8_Data[1]);
            c_Text += "  NodeId: " + C_SclString::IntToStr(opu8_Data[2]);
            u8_FirstRawByte = ou8_DataSize; //finished here ...
         }
         else
         {
            c_Text += " error: incorrect number of data bytes";
         }
      }
      break;
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_2:
      c_Text = "SetNodeIdBySerialNumberExtPart2";
      if (oq_IsResponse == false)
      {
         if (ou8_DataSize == 3U)
         {
            c_Text += "  SubNodeId: " + C_SclString::IntToStr(opu8_Data[0]);
            c_Text += "  SNR Manufacturer Format: " + C_SclString::IntToStr(opu8_Data[1]);
            c_Text += "  SNR Length: " + C_SclString::IntToStr(opu8_Data[2]);
            u8_FirstRawByte = ou8_DataSize; //finished here ...
         }
         else
         {
            c_Text += " error: incorrect number of data bytes";
         }
      }
      break;
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_3:  // Similar service
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_4:  // Similar service
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_5:  // Similar service
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_6:  // Similar service
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_7:  // Similar service
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_8:  // Similar service
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_9:  // Similar service
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_10: // Similar service
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_11: // Similar service
   case UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_12: // Similar service
      //no details reported for now
      c_Text = "SetNodeIdBySerialNumberExtPart" + C_SclString::IntToStr(
         (ou16_RoutineIdentifier - UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_1) + 1U);

      if (oq_IsResponse == false)
      {
         if (ou8_DataSize > 0U)
         {
            uint32_t u32_SnrSignCounter;
            // Show the parts of serial number as string
            c_Text += "  SNR FSN Part: ";
            for (u32_SnrSignCounter = 0U; u32_SnrSignCounter < ou8_DataSize; ++u32_SnrSignCounter)
            {
               c_Text += static_cast<char_t>(opu8_Data[u32_SnrSignCounter]);
            }

            if ((ou8_DataSize == 3U) &&
                ((ou16_RoutineIdentifier == UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_3) ||
                 (ou16_RoutineIdentifier == UDS_H_ROUTINE_CTRL_SET_NODEID_BY_SERIALNUMBER_EXT_4)))
            {
               C_SclString c_Text2;
               // POS could be possible, but can not be differed. Show both possibilities
               c_Text2.PrintFormatted("  SNR POS Part: %02x%02x%02x", opu8_Data[0], opu8_Data[1], opu8_Data[2]);
               c_Text += c_Text2;
            }
            u8_FirstRawByte = ou8_DataSize; //finished here ...
         }
         else
         {
            c_Text += " error: incorrect number of data bytes";
         }
      }
      break;
   default:
      c_Text = "unknownroutineidentifier (" + m_GetValueDecHex(ou16_RoutineIdentifier) + ")";
      break;
   }

   //there is more data we cannot interpret: display as raw
   if ((ou8_DataSize - u8_FirstRawByte) > 0) //do we have data byte(s) left to display in raw format ?
   {
      c_Text += m_RawDataToString(ou8_DataSize - u8_FirstRawByte, &opu8_Data[u8_FirstRawByte]);
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert openSYDE CAN message to text representation.

   \param[in]     orc_Msg              message to convert

   \return
   Text interpretation of CAN message ("" if the message can not be interpreted)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Text;
   C_SclString c_Error;

   if (orc_Msg.u8_XTD == 1U) //29bit ID ?
   {
      int32_t s32_Return;
      uint16_t u16_NumBytes;
      T_CanAddressInformation t_Address;
      //decode ID:
      s32_Return = C_CanMonProtocolOpenSyde::mh_DisassembleCanId(orc_Msg.u32_ID, t_Address);
      if (s32_Return == C_NO_ERR)
      {
         c_Text = this->m_AddressInformationToText(t_Address);

         //now go for the data bytes:
         if (orc_Msg.u8_DLC < 1U)
         {
            c_Error = "DLC too short (no PCI byte)";
         }
         else
         {
            switch (orc_Msg.au8_Data[0] & 0xF0U)
            {
            case ISO15765_N_PCI_SF:
               uint8_t u8_Index;

               u16_NumBytes = (orc_Msg.au8_Data[0] & 0x0FU);
               c_Text += (" SF  NUM " + m_GetByteAsStringFormat(static_cast<uint8_t>(u16_NumBytes)) + " ");

               if (orc_Msg.au8_Data[1] == OSY_UDS_NR_SI) //negative response ?
               {
                  if ((u16_NumBytes == 3U) && (orc_Msg.u8_DLC >= 4U))
                  {
                     //regular negative response
                     //set response flag to let engine flag the message as "response"
                     c_Text += m_ServiceIdToText(orc_Msg.au8_Data[2], true) + " NRC:" +
                               m_NegativeResponseCodeToText(orc_Msg.au8_Data[3]);
                  }
                  else if ((u16_NumBytes == 6U) && (orc_Msg.u8_DLC >= 7U) &&
                           (orc_Msg.au8_Data[2] == OSY_DPD_SI_OS_READ_DATAPOOLDATA_EVENT_DRIVEN))
                  {
                     //special negative response for this service
                     const uint32_t u32_DpIndex = (static_cast<uint32_t>(orc_Msg.au8_Data[4]) << 16U) +
                                                  (static_cast<uint32_t>(orc_Msg.au8_Data[5]) << 8U) +
                                                  orc_Msg.au8_Data[6];
                     c_Text += m_ServiceIdToText(orc_Msg.au8_Data[2], true) + " NRC:" +
                               m_NegativeResponseCodeToText(orc_Msg.au8_Data[3]) + " " +
                               m_DataPoolIdentifierToText(u32_DpIndex, true, t_Address);
                  }
                  else
                  {
                     c_Text += "NEGRES with invalid length (incorrect number of bytes in header or not enough data" \
                               " bytes in CAN message)";
                  }
               }
               else
               {
                  if (orc_Msg.u8_DLC < (1 + u16_NumBytes))
                  {
                     c_Error = "DLC too short (not enough data bytes)";
                  }
                  else
                  {
                     if (u16_NumBytes > 0U)
                     {
                        c_Text += m_ServiceIdToText(orc_Msg.au8_Data[1]);
                        c_Text += m_ServiceDataToText(&orc_Msg.au8_Data[1], static_cast<uint8_t>(u16_NumBytes),
                                                      t_Address, true);
                     }
                  }
               }
               break;
            case ISO15765_N_PCI_FF:
               c_Text += " FF ";
               if (orc_Msg.u8_DLC != 8) //FF only makes sense if all bytes are used
               {
                  c_Error = "DLC too short (not equals 8)";
               }
               else
               {
                  u16_NumBytes = ((static_cast<uint16_t>(orc_Msg.au8_Data[0] & 0x0FU)) << 8U) + orc_Msg.au8_Data[1];
                  c_Text += (" NUM " + m_GetWordAsStringFormat(u16_NumBytes) + " ");
                  c_Text += m_ServiceIdToText(orc_Msg.au8_Data[2]);
                  c_Text += m_ServiceDataToText(&orc_Msg.au8_Data[2], 6, t_Address, false);
               }
               break;
            case ISO15765_N_PCI_CF:
               c_Text += (" CF  SN " + C_SclString::IntToStr(orc_Msg.au8_Data[0] & 0x0FU));
               if (orc_Msg.u8_DLC < 2) //CF with no data byte does not make sense
               {
                  c_Error = "DLC too short (not >= 2)";
               }
               else
               {
                  //we must assume it's the last frame: get number of occupied bytes
                  u16_NumBytes = orc_Msg.u8_DLC - 1U;

                  c_Text += ("  NUM " + m_GetByteAsStringFormat(static_cast<uint8_t>(u16_NumBytes)) + "  DATA[");

                  for (u8_Index = 0; u8_Index < u16_NumBytes; u8_Index++)
                  {
                     c_Text += m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Index + 1]);
                     if (u8_Index != (u16_NumBytes - 1))
                     {
                        c_Text += ",";
                     }
                  }
                  c_Text += "]";
               }
               break;
            case ISO15765_N_PCI_FC:
               c_Text += " FC ";

               if (orc_Msg.u8_DLC < 2) //not enough data
               {
                  c_Error = "DLC too short (< 3)";
               }
               else
               {
                  c_Text += (" FS " + m_GetByteAsStringFormat(orc_Msg.au8_Data[0] & 0x0FU) +
                             "  BS " + m_GetByteAsStringFormat(orc_Msg.au8_Data[1]) +
                             "  STMIN " + m_GetByteAsStringFormat(orc_Msg.au8_Data[2]));
               }
               break;
            case STWOPENSYDE_PCI_SF:
               c_Text += " OSF ";
               if (orc_Msg.au8_Data[0] == OSY_OS_READ_DATAPOOLDATA_EVENT_DRIVEN)
               {
                  c_Text += " ReadDataPoolDataEventDriven ";
                  if (orc_Msg.u8_DLC < 3U) //not enough data
                  {
                     c_Error = "DLC too short (no dataPoolDataIdentifier)";
                  }
                  else
                  {
                     //we should have data:
                     const uint32_t u32_DpIndex = (static_cast<uint32_t>(orc_Msg.au8_Data[1]) << 16U) +
                                                  (static_cast<uint32_t>(orc_Msg.au8_Data[2]) << 8U) +
                                                  orc_Msg.au8_Data[3];
                     c_Text += m_DataPoolIdentifierToText(u32_DpIndex, true, t_Address);
                     //display data in raw format
                     if ((orc_Msg.u8_DLC - 4U) > 0U) //do we have data byte(s) left to display in raw format ...
                     {
                        c_Text += m_RawDataToString(orc_Msg.u8_DLC - 4U, &orc_Msg.au8_Data[4]);
                     }
                  }
               }
               else if (orc_Msg.au8_Data[0] == OSY_OS_POS_STARTUP_MESSAGE)
               {
                  c_Text += " Startup message POS ";
                  if (orc_Msg.u8_DLC < 8U) //not enough data
                  {
                     c_Error = "DLC too short (no dataPoolDataIdentifier)";
                  }
                  else
                  {
                     //we should have data:
                     C_SclString c_Snr;
                     uint32_t u32_SubNodeId = static_cast<uint32_t>(orc_Msg.au8_Data[1] & 0xF0U) >> 4U;
                     c_Snr = this->mh_SerialNumberToString(&orc_Msg.au8_Data[2]);
                     c_Text += "  SubNodeId: " + C_SclString::IntToStr(u32_SubNodeId);
                     c_Text += "  SNR: " + c_Snr;
                  }
               }
               else if (orc_Msg.au8_Data[0] == OSY_OS_FSN_STARTUP_MESSAGE)
               {
                  c_Text += " Startup message FSN ";
                  if (orc_Msg.u8_DLC < 2U) //not enough data
                  {
                     c_Error = "DLC too short";
                  }
                  else
                  {
                     //we should have data:
                     uint32_t u32_BlockNr =  static_cast<uint32_t>(orc_Msg.au8_Data[1] & 0x0FU);
                     uint32_t u32_SubNodeId = static_cast<uint32_t>(orc_Msg.au8_Data[1] & 0xF0U) >> 4U;
                     uint32_t u32_SnrSignStartIndex;
                     uint32_t u32_SnrSignCounter;
                     bool q_DlcCorrect = true;

                     c_Text += "  BlockNumber: " + C_SclString::IntToStr(u32_BlockNr);
                     c_Text += "  SubNodeId: " + C_SclString::IntToStr(u32_SubNodeId);

                     if ((u32_BlockNr == 0U) &&
                         (orc_Msg.u8_DLC > 4U)) // SNR must have a length of at least 1
                     {
                        c_Text += "  SNR Manufacturer Format: " + C_SclString::IntToStr(orc_Msg.au8_Data[2]);
                        c_Text += "  SNR Length: " + C_SclString::IntToStr(orc_Msg.au8_Data[3]);
                        u32_SnrSignStartIndex = 4U;
                     }
                     else if (u32_BlockNr > 0U)
                     {
                        u32_SnrSignStartIndex = 2U;
                     }
                     else
                     {
                        q_DlcCorrect = false;
                        c_Error = "DLC too short";
                     }

                     if (q_DlcCorrect == true)
                     {
                        // The rest of the message is a part of the SNR itself
                        c_Text += "  SNR Block part: ";

                        for (u32_SnrSignCounter = u32_SnrSignStartIndex; u32_SnrSignCounter < orc_Msg.u8_DLC;
                             ++u32_SnrSignCounter)
                        {
                           c_Text += static_cast<char_t>(orc_Msg.au8_Data[u32_SnrSignCounter]);
                        }
                     }
                  }
               }
               else
               {
                  c_Error = "unknown service" + m_GetByteAsStringFormat(orc_Msg.au8_Data[0] & 0x0FU);
               }
               break;

            case STWOPENSYDE_PCI_MF:
               c_Text += " OMF";
               if ((orc_Msg.au8_Data[0] & 0x0FU) == 0x00U)
               {
                  //it's an OMFI (openSYDE multi-frame first)
                  c_Text += "I ";
                  if (orc_Msg.u8_DLC != 8) //first OMF only makes sense if all bytes are used
                  {
                     c_Error = "DLC too short (not equals 8)";
                  }
                  else
                  {
                     c_Text += ("NUM " + m_GetByteAsStringFormat(orc_Msg.au8_Data[1]) + " ");
                     c_Text += m_ServiceIdToText(orc_Msg.au8_Data[2]);
                     c_Text += m_ServiceDataToText(&orc_Msg.au8_Data[2], 6, t_Address, false);
                  }
               }
               else
               {
                  //it's an OMFO (openSYDE multi-frame following)
                  c_Text += ("O SN " + C_SclString::IntToStr(orc_Msg.au8_Data[0] & 0x0FU));

                  u16_NumBytes = orc_Msg.u8_DLC - 1U;

                  c_Text += ("  NUM " + m_GetByteAsStringFormat(static_cast<uint8_t>(u16_NumBytes)) + "  DATA[");

                  for (u8_Index = 0; u8_Index < u16_NumBytes; u8_Index++)
                  {
                     c_Text += m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Index + 1]);
                     if (u8_Index != (u16_NumBytes - 1))
                     {
                        c_Text += ",";
                     }
                  }
                  c_Text += "]";
               }
               break;
            default:
               c_Error = "Unknown N_PCItype";
               break;
            }
         }
      }
      else
      {
         //nothing we understand ...
      }
   }

   if (c_Error != "")
   {
      c_Text += "Error: " + c_Error + " !";
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Get raw data as text

   \return
   Text representation of raw data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::m_RawDataToString(const uint8_t ou8_NumBytes,
                                                        const uint8_t * const opu8_Data) const
{
   uint8_t u8_Index;
   C_SclString c_Text = "  DATA[";

   for (u8_Index = 0U; u8_Index < ou8_NumBytes; u8_Index++)
   {
      c_Text += m_GetByteAsStringFormat(opu8_Data[u8_Index]);
      if (u8_Index != (ou8_NumBytes - 1))
      {
         c_Text += ",";
      }
   }
   c_Text += "]";
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name of protocol as string

   \return
   Text representation of protocol name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolOpenSyde::GetProtocolName(void) const
{
   return "STW openSYDE";
}

//----------------------------------------------------------------------------------------------------------------------
