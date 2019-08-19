//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implements textual interpretation of SHIP-IP! and STW IVA protocol messages

   As the IVA protocol is fully included into the SHIP-IP! protocol, and there is no ambiguity
   this class is used to interpret both protocols.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/

//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstdlib>
#include "CCMONProtocolTarget.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CCMONProtocolSHIPIPIVA.h"
#include "TGLUtils.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_errors;
using namespace stw_cmon_protocol;
using namespace stw_scl;
using namespace stw_can;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

//Have the protocol constants here as copies, so we can put this class into a library of its own
// without having to add all the protocol header files.
//This is not very nice. On the other hand we'd produce a lot of dependencies if we'd include the header files
// for all possible L7 protocols.

//IVA protocol constants
/// \name  Defines of the jobs low byte for IVA functions (command low byte of the CAN messages)
// \group_begin
#define JOB_LB_IVA_LOGON                 (static_cast<uint8>(0x01U)) ///< Command low byte for Job "Log On"
#define JOB_LB_IVA_LOGOFF                (static_cast<uint8>(0x02U)) ///< Command low byte for Job "Log Off"
#define JOB_LB_IVA_SERVICE_READ          (static_cast<uint8>(0x03U)) ///< Command low byte for Job "Service Read Funktionen"
#define JOB_LB_IVA_SERVICE_WRITE         (static_cast<uint8>(0x04U)) ///< Command low byte for Job "Service Write Funktionen"
#define JOB_LB_IVA_READ_INDEX_EXPEDITED  (static_cast<uint8>(0x10U)) ///< Command low byte for Job "Read Index Data"
#define JOB_LB_IVA_WRITE_INDEX_EXPEDITED (static_cast<uint8>(0x11U)) ///< Command low byte for Job "Write Index Data"
#define JOB_LB_IVA_READ_INDEX_SEG_START  (static_cast<uint8>(0x12U)) ///< Command low byte for Job "Read Index Segment"
#define JOB_LB_IVA_READ_INDEX_SEG_FLOW   (static_cast<uint8>(0x13U)) ///< Command low byte for Job "Read Index Flow"
#define JOB_LB_IVA_WRITE_INDEX_SEG_START (static_cast<uint8>(0x14U)) ///< Command low byte for Job "Write Index Segment Start"
#define JOB_LB_IVA_WRITE_INDEX_SEG_DATA  (static_cast<uint8>(0x15U)) ///< Command low byte for Job "Write Index Segment Data"
#define JOB_LB_IVA_READ_EEP_EXPEDITED    (static_cast<uint8>(0x20U)) ///< Command low byte for Job "Read Eeprom Data"
#define JOB_LB_IVA_WRITE_EEP_EXPEDITED   (static_cast<uint8>(0x21U)) ///< Command low byte for Job "Write Eeprom Data"
#define JOB_LB_IVA_READ_EEP_SEG_START    (static_cast<uint8>(0x22U)) ///< Command low byte for Job "Read Eeprom Segment"
#define JOB_LB_IVA_READ_EEP_SEG_FLOW     (static_cast<uint8>(0x23U)) ///< Command low byte for Job "Read Eeprom Flow"
#define JOB_LB_IVA_WRITE_EEP_SEG_START   (static_cast<uint8>(0x24U)) ///< Command low byte for Job "Write Eeprom Segment Start"
#define JOB_LB_IVA_WRITE_EEP_SEG_DATA    (static_cast<uint8>(0x25U)) ///< Command low byte for Job "Write Eeprom Segment Data"
#define JOB_LB_IVA_UPDATE_LIST_CHECKSUM  (static_cast<uint8>(0x50U)) ///< Command low byte for Job "Update Chacksum"
#define JOB_LB_IVA_CHECK_LIST_CHECKSUM   (static_cast<uint8>(0x51U)) ///< Command low byte for Job "Check Checksum"
#define JOB_LB_IVA_WRITE_LIST_DEFAULT    (static_cast<uint8>(0x60U)) ///< Command low byte for Job "Write List Default"

// \group_end

/// \name  IVA Numbers for services
/// \group_begin
#define IVA_SERVICE_PROT_VER      (static_cast<uint16>(0x0000U)) ///< Service: Protocol Version
#define IVA_SERVICE_IMPL_VER      (static_cast<uint16>(0x0001U)) ///< Service: Driver Version (Inplemented Version)
#define IVA_SERVICE_DEVICE_TYPE1  (static_cast<uint16>(0x0003U)) ///< Service: Device Name 1. Part
#define IVA_SERVICE_DEVICE_TYPE2  (static_cast<uint16>(0x0004U)) ///< Service: Device Name 2. Part
#define IVA_SERVICE_DEVICE_TYPE3  (static_cast<uint16>(0x0005U)) ///< Service: Device Name 3. Part
#define IVA_SERVICE_DEVICE_TYPE4  (static_cast<uint16>(0x0006U)) ///< Service: Device Name4. Part
#define IVA_SERVICE_OD_VER        (static_cast<uint16>(0x0007U)) ///< Service: Version of the objekt directory
#define IVA_SERVICE_BSMAX_SERVER  (static_cast<uint16>(0x0008U)) ///< Service: Max. Numbers of Blocks for segmented write access
#define IVA_SERVICE_STMIN_SERVER  (static_cast<uint16>(0x0009U)) ///< Service: Min. time beween the CAN messages for segmented write access
#define IVA_SERVICE_BSMAX_CLIENT  (static_cast<uint16>(0x000AU)) ///< Service: Max. Numbers of Blocks for segmented read access
#define IVA_SERVICE_STMIN_CLIENT  (static_cast<uint16>(0x000BU)) ///< Service: Min. time beween the CAN messages for segmented read access
/// \group_end

#define CA_NO_ERR        (0)
#define CA_UNKNOWN_ERR   (1)
#define CA_WARN          (2)
#define CA_DEFAULT       (3)
#define CA_BUSY          (4)
#define CA_RANGE         (5)
#define CA_OVERFLOW      (6)
#define CA_RD_WR         (7)
#define CA_NOACT         (8)
#define CA_COM           (9)
#define CA_CONFIG       (10)
#define CA_CHECKSUM     (11)

//SHIP-IP! constants:
//sip_application_constants.h
//access types:
#define SIP_APP_AAT_READ                     (0x00)  // read server access
#define SIP_APP_AAT_WRITE                    (0x01)  // write server access
#define SIP_APP_AAT_WRITE_QUOTED             (0x02)  // write server access and quote payload
#define SIP_APP_AAT_READ_CYCLIC              (0x03)  // install/response cyclic transfer
#define SIP_APP_AAT_READ_STOP                (0x04)  // stop specific cyclic transfer
#define SIP_APP_AAT_READ_STOP_ALL            (0x05)  // stop all cyclic transfers
#define SIP_APP_AAT_DEFINE_DYNAMIC_SERVICE   (0x06)  // define new service on server
#define SIP_APP_AAT_READ_MEMORY              (0x10)  // read from memory
#define SIP_APP_AAT_WRITE_MEMORY             (0x11)  // write to memory
#define SIP_APP_AAT_READ_OD                  (0x20)  // read from object dictionary
#define SIP_APP_AAT_WRITE_OD                 (0x21)  // write to object dictionary

//error codes:
#define SIP_APP_ERR_NO_ERR                   (0x00)   // everything allright
#define SIP_APP_ERR_UNEXPECTED_FLAG          (0x80)   // AREQ wrong, AERF set
#define SIP_APP_ERR_ACCESS_TYPE              (0x81)   // requested access type not available
#define SIP_APP_ERR_SERVICE_AREA             (0x82)   // requested service area not available
#define SIP_APP_ERR_SERVICE_NAME             (0x83)   // requested service name not available
#define SIP_APP_ERR_SERVICE_PARA             (0x84)   // requested service parameter not available
#define SIP_APP_ERR_READ_WRITE               (0x85)   // trying to write a read only service or vice versa
#define SIP_APP_ERR_DYNAMIC_OVERFLOW         (0x86)   // out of resources - can not configure requested dynamic service
#define SIP_APP_ERR_CYCLIC_OVERFLOW          (0x87)   // out of resources - cannot configure requested cyclic service
#define SIP_APP_ERR_DATASIZE                 (0x88)   // data length error (payload size inconsistent with access type or actual service to be executed)
#define SIP_APP_ERR_SERVICE                  (0x89)   // service error – cannot handle service
#define SIP_APP_ERR_SERVICE_ALIT             (0x90)   // location information target wrong

//memory types
#define SIP_APP_MEM_TYPE_EEPROM              (0x00)   // target memory: EEPROM
#define SIP_APP_MEM_TYPE_FLASH               (0x01)   // target memory: FLASH
//no more defined for now

#define SIP_APP_MEM_EEP_INDEX_SYSTEM         (0x00)   // index of system EEPROM
#define SIP_APP_MEM_EEP_INDEX_USER           (0x01)   // index of user EEPROM

//object dictionary types
#define SIP_APP_OBJECT_DICTIONARY_TYPE_IV    (0x00)   // IV as used at STW for system EEPROM mapping
//no more defined for now

//sip_tp_can_constants.h
#define SIP_TPC_MSG_POS_NSID              (23)    // Server ID
#define SIP_TPC_MSG_POS_TCRQ              (22)    // CAN request flag
#define SIP_TPC_MSG_POS_TCFT              (19)    // CAN frame type
#define SIP_TPC_MSG_POS_ADS_SINGLE        (16)    // Data size single frame
#define SIP_TPC_MSG_POS_ADS_SEGMENTED     (8)     // Data size segmented first frame

#define SIP_TPC_SEGMENTED_FIRST_MAX_DATA_SIZE            (9)

#define SIP_TPC_TCFT_EXPEDITED_SINGLE             (0UL)
#define SIP_TPC_TCFT_SEGMENTED_FIRST              (1UL)
#define SIP_TPC_TCFT_SEGMENTED_CONSECUTIVE        (2UL)
#define SIP_TPC_TCFT_ERROR                        (3UL)

#define SIP_TPC_TCRQ_IS_REQUEST        (0)

#define SIP_TPC_MASK_NSID               (0x03FUL << (SIP_TPC_MSG_POS_NSID)) // last six bit = server id (NSID)
#define SIP_TPC_MASK_TCRQ               (0x001UL << (SIP_TPC_MSG_POS_TCRQ)) // TCRQ mask
#define SIP_TPC_MASK_TCFT               (0x003UL << (SIP_TPC_MSG_POS_TCFT)) // CAN frame type mask
#define SIP_TPC_MASK_ADS_SINGLE_SEGCON  (0x007UL << (SIP_TPC_MSG_POS_ADS_SINGLE)) // Data size mask of frame expedited single
#define SIP_TPC_MASK_ADS_SEGMENTED      (0x7FFUL << (SIP_TPC_MSG_POS_ADS_SEGMENTED)) // Data size mask of frame segmented first

#define SIP_TPC_ERROR_NO_ERROR          (0U)
#define SIP_TPC_ERROR_INVALID_DATA_SIZE (1U)  //segmented transfer first frame with too little data -> use single frame
#define SIP_TPC_ERROR_STATE             (2U)  //segmented first received when expecting segmented consecutive
                                             //-> receiver still shall try to handle new incoming segmented transfer
                                             //or: segmented consecutive frame received when not expecting it
#define SIP_TPC_ERROR_DLC               (3U)  //not enough data bytes in message
#define SIP_TPC_ERROR_BLOCK_COUNTER     (4U)  //wrong block counter detected in segmented consecutive frame
#define SIP_TPC_ERROR_FRAME_TYPE        (5U)  //unknown frame type received

//SIP eleven specific constants (note that some constants from TPC are reused)
#define SIP_TPC11_MSG_POS_NSID              (1U)    // Server ID (in ID)
#define SIP_TPC11_MSG_POS_TCFT              (5U)    // CAN frame type (in DB0)

#define SIP_TPC11_SEGMENTED_FIRST_MAX_DATA_SIZE  (6U)

#define SIP_TPC11_MASK_NSID               (0x03FUL << (SIP_TPC11_MSG_POS_NSID)) // last six bit = server id (NSID)
#define SIP_TPC11_MASK_TCRQ               (0x001UL)                             // TCRQ mask
#define SIP_TPC11_MASK_TCFT               (0x007UL << (SIP_TPC11_MSG_POS_TCFT)) // CAN frame type mask
#define SIP_TPC11_MASK_ADS_SINGLE_SEGCON  (0x007UL)                             // Data size mask of frame expedited single
#define SIP_TPC11_MASK_ADS_SEGMENTED_MSB  (0x007UL)                            // Data size mask of frame segmented first upper three bits

//sip_services_target_constants.h
// -- Application service area (ASA) defines
#define SIP_ASA_APPLICATION                                        (1U)
#define SIP_ASA_BACKGROUND_FUNCTION                                (12U)
#define SIP_ASA_CAN                                                (6U)
#define SIP_ASA_ETHERNET                                           (13U)
#define SIP_ASA_INDEX_VARIABLE_ACCESS                              (8U)
#define SIP_ASA_INPUT                                              (3U)
#define SIP_ASA_MEMORY                                             (7U)
#define SIP_ASA_OUTPUT                                             (4U)
#define SIP_ASA_PROTOCOL                                           (0U)
#define SIP_ASA_REAL_TIME_CLOCK                                    (11U)
#define SIP_ASA_SERIAL                                             (5U)
#define SIP_ASA_SUPERVISOR                                         (10U)
#define SIP_ASA_SYSTEM                                             (2U)
#define SIP_ASA_TEMPORARY_SERVICE                                  (9U)

// -- Application service number (ASN) defines
#define SIP_ASN_APP_BIOS                                           (1U)
#define SIP_ASN_APP_DESTROY                                        (2U)
#define SIP_ASN_APP_VERSION                                        (0U)
#define SIP_ASN_BGF_NAME                                           (4U)
#define SIP_ASN_BGF_PARAMETER                                      (5U)
#define SIP_ASN_BGF_SETUP                                          (3U)
#define SIP_ASN_BGF_START                                          (0U)
#define SIP_ASN_BGF_STATUS                                         (2U)
#define SIP_ASN_BGF_STOP                                           (1U)
#define SIP_ASN_CAN_BUS_STATE                                      (1U)
#define SIP_ASN_CAN_CLOSE                                          (5U)
#define SIP_ASN_CAN_INIT                                           (0U)
#define SIP_ASN_CAN_RX_ID                                          (2U)
#define SIP_ASN_CAN_RX_MESSAGE                                     (3U)
#define SIP_ASN_CAN_TX_MESSAGE                                     (4U)
#define SIP_ASN_ETH_CLOSE                                          (1U)
#define SIP_ASN_ETH_INIT                                           (0U)
#define SIP_ASN_ETH_STATE                                          (2U)
#define SIP_ASN_IVA_CHECKSUM_CHECK                                 (1U)
#define SIP_ASN_IVA_CHECKSUM_UPDATE                                (2U)
#define SIP_ASN_IVA_DEFAULTS                                       (3U)
#define SIP_ASN_IVA_SERVICE                                        (0U)
#define SIP_ASN_I_COUNTER                                          (2U)
#define SIP_ASN_I_DUTY_CYCLE                                       (3U)
#define SIP_ASN_I_EVENT_COUNTER                                    (5U)
#define SIP_ASN_I_INIT                                             (0U)
#define SIP_ASN_I_STATE                                            (4U)
#define SIP_ASN_I_TEST_COMPARATOR                                  (6U)
#define SIP_ASN_I_VALUE                                            (1U)
#define SIP_ASN_I_VALUE_UNSCALED                                   (7U)
#define SIP_ASN_MEM_INFO                                           (1U)
#define SIP_ASN_MEM_TEST                                           (0U)
#define SIP_ASN_MEM_TEST_FAILURE_DETAILS                           (2U)
#define SIP_ASN_O_CONTROL_PARAMETER                                (6U)
#define SIP_ASN_O_CURRENT                                          (4U)
#define SIP_ASN_O_EVENT_ERROR                                      (7U)
#define SIP_ASN_O_FREQUENCY                                        (2U)
#define SIP_ASN_O_INIT                                             (0U)
#define SIP_ASN_O_OVERCURRENT_TESTMODE                             (9U)
#define SIP_ASN_O_STATE                                            (3U)
#define SIP_ASN_O_STATE_RESET                                      (8U)
#define SIP_ASN_O_VALUE                                            (1U)
#define SIP_ASN_O_VOLTAGE                                          (5U)
#define SIP_ASN_PROT_MAXBUFFERSIZE                                 (1U)
#define SIP_ASN_PROT_MAXNUMCYCLICTRANSMISSIONS                     (2U)
#define SIP_ASN_PROT_MAXNUMDYNAMICCONFIGURATIONS                   (3U)
#define SIP_ASN_PROT_PARAMETER                                     (4U)
#define SIP_ASN_PROT_SERVICE_VERSION                               (5U)
#define SIP_ASN_PROT_VERSION                                       (0U)
#define SIP_ASN_RTC_TIME_DATE                                      (0U)
#define SIP_ASN_SER_CLEAR_RX_BUFFER                                (3U)
#define SIP_ASN_SER_CLOSE                                          (4U)
#define SIP_ASN_SER_INIT                                           (0U)
#define SIP_ASN_SER_RX_BUFFER                                      (1U)
#define SIP_ASN_SER_TX_BUFFER                                      (2U)
#define SIP_ASN_SSV_ERROR_REACTION                                 (3U)
#define SIP_ASN_SSV_INIT_VOLTAGE                                   (0U)
#define SIP_ASN_SSV_STATUS                                         (1U)
#define SIP_ASN_SSV_TEST_TIME                                      (2U)
#define SIP_ASN_SYS_ADC_CALIBRATION                                (11U)
#define SIP_ASN_SYS_ADC_CALIBRATION_STATE                          (12U)
#define SIP_ASN_SYS_ADC_FILTER                                     (20U)
#define SIP_ASN_SYS_ADC_VALUE_16BIT                                (13U)
#define SIP_ASN_SYS_ADC_VALUE_SCALED_32BIT                         (9U)
#define SIP_ASN_SYS_BEEPER                                         (3U)
#define SIP_ASN_SYS_BUTTON                                         (8U)
#define SIP_ASN_SYS_CLOSE                                          (19U)
#define SIP_ASN_SYS_COMPONENT_COUNT                                (16U)
#define SIP_ASN_SYS_COMPONENT_VERSION                              (17U)
#define SIP_ASN_SYS_DIAG                                           (10U)
#define SIP_ASN_SYS_IGNITION                                       (2U)
#define SIP_ASN_SYS_INFO                                           (0U)
#define SIP_ASN_SYS_LED                                            (4U)
#define SIP_ASN_SYS_RELAY                                          (6U)
#define SIP_ASN_SYS_RELAY_ERROR_COUNTERS                           (14U)
#define SIP_ASN_SYS_RELAY_RESET_ERROR_COUNTERS                     (15U)
#define SIP_ASN_SYS_RESET                                          (1U)
#define SIP_ASN_SYS_SENSOR_SUPPLY                                  (7U)
#define SIP_ASN_SYS_TEMPERATURE                                    (5U)
#define SIP_ASN_SYS_WAKEUP                                         (18U)

// -- Application sub service identifiers (ASP) defines
#define SIP_ASP_APP_BIOS_VERSION                                   (0U)
#define SIP_ASP_APP_VERSION_GET                                    (0U)
#define SIP_ASP_BGF_SETUP_COUNT                                    (0U)
#define SIP_ASP_IVA_SERVICE_LOGOFF                                 (1U)
#define SIP_ASP_IVA_SERVICE_LOGON                                  (0U)
#define SIP_ASP_IVA_SERVICE_OBJECT_DICTIONARY_VERSION              (2U)
#define SIP_ASP_MEM_INFO_FLASH_LOADER_VERSION                      (0U)
#define SIP_ASP_MEM_TEST_FAILURE_DETAILS_GET                       (0U)
#define SIP_ASP_PROT_MAXBUFFERSIZE_GET                             (0U)
#define SIP_ASP_PROT_MAXNUMCYCLICTRANSMISSIONS_GET                 (0U)
#define SIP_ASP_PROT_MAXNUMDYNAMICCONFIGURATIONS_GET               (0U)
#define SIP_ASP_PROT_PARAMETER_BITRATE                             (0U)
#define SIP_ASP_PROT_SERVICE_VERSION_GET                           (0U)
#define SIP_ASP_PROT_VERSION_GET                                   (0U)
#define SIP_ASP_SYS_CLOSE_EXECUTE                                  (0U)
#define SIP_ASP_SYS_COMPONENT_COUNT_GET                            (0U)
#define SIP_ASP_SYS_DIAG_LAST_RESET_REASON                         (2U)
#define SIP_ASP_SYS_DIAG_VOLTAGE_OVER                              (1U)
#define SIP_ASP_SYS_DIAG_VOLTAGE_UNDER                             (0U)
#define SIP_ASP_SYS_IGNITION_CONFIG                                (0U)
#define SIP_ASP_SYS_IGNITION_STATUS                                (1U)
#define SIP_ASP_SYS_INFO_ARTICLE_NUMBER                            (1U)
#define SIP_ASP_SYS_INFO_DEVICE_ID                                 (2U)
#define SIP_ASP_SYS_INFO_EEPROM_SIZE                               (5U)
#define SIP_ASP_SYS_INFO_RAM_SIZE                                  (3U)
#define SIP_ASP_SYS_INFO_ROM_SIZE                                  (4U)
#define SIP_ASP_SYS_INFO_SERIAL_NUMBER                             (0U)
#define SIP_ASP_SYS_INFO_STATUS_INIT                               (6U)
#define SIP_ASP_SYS_RESET_EXECUTE                                  (0U)
#define SIP_ASP_SYS_TEMPERATURE_CPU                                (1U)
#define SIP_ASP_SYS_TEMPERATURE_SYSTEM                             (0U)
#define SIP_ASP_SYS_TEMPERATURE                                    (2U)

// - Application sub services s defines - payload size defines
#define SIP_PSIZE_APP_BIOS_VERSION                                   (4U)
#define SIP_PSIZE_APP_VERSION_GET                                    (2U)
#define SIP_PSIZE_BGF_NAME                                           (22U)
#define SIP_PSIZE_BGF_PARAMETER                                      (92U)
#define SIP_PSIZE_BGF_SETUP_COUNT                                    (1U)
#define SIP_PSIZE_BGF_START                                          (12U)
#define SIP_PSIZE_BGF_STATUS                                         (4U)
#define SIP_PSIZE_BGF_STOP                                           (32U)
#define SIP_PSIZE_CAN_BUS_STATE                                      (1U)
#define SIP_PSIZE_CAN_INIT                                           (4U)
#define SIP_PSIZE_CAN_RX_ID                                          (5U)
#define SIP_PSIZE_CAN_RX_MESSAGE                                     (15U)
#define SIP_PSIZE_CAN_TX_MESSAGE                                     (14U)
#define SIP_PSIZE_ETH_INIT                                           (2U)
#define SIP_PSIZE_ETH_STATE                                          (4U)
#define SIP_PSIZE_IVA_CHECKSUM_CHECK                                 (1U)
#define SIP_PSIZE_IVA_DEFAULTS                                       (1U)
#define SIP_PSIZE_IVA_SERVICE_LOGON                                  (6U)
#define SIP_PSIZE_IVA_SERVICE_OBJECT_DICTIONARY_VERSION              (2U)
#define SIP_PSIZE_I_COUNTER                                          (4U)
#define SIP_PSIZE_I_DUTY_CYCLE                                       (4U)
#define SIP_PSIZE_I_EVENT_COUNTER                                    (2U)
#define SIP_PSIZE_I_INIT                                             (8U)
#define SIP_PSIZE_I_STATE                                            (1U)
#define SIP_PSIZE_I_TEST_COMPARATOR                                  (1U)
#define SIP_PSIZE_I_VALUE                                            (4U)
#define SIP_PSIZE_I_VALUE_UNSCALED                                   (2U)
#define SIP_PSIZE_MEM_INFO_FLASH_LOADER_VERSION                      (5U)
#define SIP_PSIZE_MEM_TEST                                           (11U)
#define SIP_PSIZE_MEM_TEST_FAILURE_DETAILS_GET                       (5U)
#define SIP_PSIZE_O_CONTROL_PARAMETER                                (14U)
#define SIP_PSIZE_O_CURRENT                                          (4U)
#define SIP_PSIZE_O_EVENT_ERROR                                      (2U)
#define SIP_PSIZE_O_FREQUENCY                                        (4U)
#define SIP_PSIZE_O_INIT                                             (6U)
#define SIP_PSIZE_O_OVERCURRENT_TESTMODE                             (1U)
#define SIP_PSIZE_O_STATE                                            (2U)
#define SIP_PSIZE_O_VALUE                                            (4U)
#define SIP_PSIZE_O_VOLTAGE                                          (4U)
#define SIP_PSIZE_PROT_MAXBUFFERSIZE_GET                             (2U)
#define SIP_PSIZE_PROT_MAXNUMCYCLICTRANSMISSIONS_GET                 (2U)
#define SIP_PSIZE_PROT_MAXNUMDYNAMICCONFIGURATIONS_GET               (2U)
#define SIP_PSIZE_PROT_PARAMETER_BITRATE                             (4U)
#define SIP_PSIZE_PROT_SERVICE_VERSION_GET                           (2U)
#define SIP_PSIZE_PROT_VERSION_GET                                   (2U)
#define SIP_PSIZE_RTC_TIME_DATE                                      (6U)
#define SIP_PSIZE_SER_INIT                                           (8U)
#define SIP_PSIZE_SER_RX_BUFFER                                      (9U)
#define SIP_PSIZE_SER_TX_BUFFER                                      (9U)
#define SIP_PSIZE_SSV_ERROR_REACTION                                 (1U)
#define SIP_PSIZE_SSV_INIT_VOLTAGE                                   (9U)
#define SIP_PSIZE_SSV_STATUS                                         (4U)
#define SIP_PSIZE_SYS_ADC_CALIBRATION_STATE                          (2U)
#define SIP_PSIZE_SYS_ADC_FILTER                                     (4U)
#define SIP_PSIZE_SYS_ADC_VALUE_16BIT                                (2U)
#define SIP_PSIZE_SYS_ADC_VALUE_SCALED_32BIT                         (4U)
#define SIP_PSIZE_SYS_BEEPER                                         (3U)
#define SIP_PSIZE_SYS_BUTTON                                         (1U)
#define SIP_PSIZE_SYS_COMPONENT_COUNT_GET                            (1U)
#define SIP_PSIZE_SYS_COMPONENT_VERSION                              (35U)
#define SIP_PSIZE_SYS_DIAG_LAST_RESET_REASON                         (4U)
#define SIP_PSIZE_SYS_DIAG_VOLTAGE_OVER                              (1U)
#define SIP_PSIZE_SYS_DIAG_VOLTAGE_UNDER                             (1U)
#define SIP_PSIZE_SYS_IGNITION_CONFIG                                (1U)
#define SIP_PSIZE_SYS_IGNITION_STATUS                                (1U)
#define SIP_PSIZE_SYS_INFO_ARTICLE_NUMBER                            (4U)
#define SIP_PSIZE_SYS_INFO_DEVICE_ID                                 (17U)
#define SIP_PSIZE_SYS_INFO_EEPROM_SIZE                               (4U)
#define SIP_PSIZE_SYS_INFO_RAM_SIZE                                  (4U)
#define SIP_PSIZE_SYS_INFO_ROM_SIZE                                  (4U)
#define SIP_PSIZE_SYS_INFO_SERIAL_NUMBER                             (6U)
#define SIP_PSIZE_SYS_INFO_STATUS_INIT                               (5U)
#define SIP_PSIZE_SYS_LED                                            (2U)
#define SIP_PSIZE_SYS_RELAY                                          (1U)
#define SIP_PSIZE_SYS_RELAY_ERROR_COUNTERS                           (8U)
#define SIP_PSIZE_SYS_SENSOR_SUPPLY                                  (4U)
#define SIP_PSIZE_SYS_TEMPERATURE_CPU                                (2U)
#define SIP_PSIZE_SYS_TEMPERATURE_SYSTEM                             (2U)
#define SIP_PSIZE_SYS_WAKEUP                                         (1U)
#define SIP_PSIZE_SYS_TEMPERATURE                                    (2U)

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
bool C_CMONProtocolSHIP_IP_IVA::hmq_ServiceTableInitialized = false;
SCLDynamicArray<C_CMONProtocolSIPASADescription> C_CMONProtocolSHIP_IP_IVA::hmc_Services;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert CAN message to text representation.

   Can either be:
   - in IVA protocol
   - SHIP-IT! via SHIP-IT! Vanilla transport protocol
   - SHIP-IT! via SHIP-IT! Eleven transport protocol
   The definition of those three protocol allows us to clearly distinguish between the two.

   \param[in]     orc_Msg              message to convert

   \return
   Text interpretation of CAN message ("" if the message can not be interpreted)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_CMONProtocolSHIP_IP_IVA::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   if (hmq_ServiceTableInitialized == false)
   {
      //initialize service description table (just once)
      hm_InitServiceTable();
      hmq_ServiceTableInitialized = true;
   }

   if (orc_Msg.u8_XTD == 0U)
   {
      return m_MessageToString11Bit(orc_Msg);
   }
   return m_SIPMessageToString(orc_Msg);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert CAN message with 11bit ID to text representation.

   Convert CAN message with 11bit ID to text represenation.
   Can either be:
   - in IVA protocol
   - SHIP-IT! via SHIP-IT! Eleven transport protocol
   The definition of those two protocol allows us to clearly distinguish between the two.

   \param[in]     orc_Msg              message to convert

   \return
   Text interpretation of CAN message ("" if the message can not be interpreted)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_CMONProtocolSHIP_IP_IVA::m_MessageToString11Bit(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SCLString c_Text;
   C_SCLString c_Help;
   bool q_IsError;
   uint8 u8_Service;
   uint8  u8_Loop;

   c_Text = "";

   if ((orc_Msg.u32_ID >= 0x50U) && (orc_Msg.u32_ID <= 0x150U) && (orc_Msg.u8_RTR == 0U) &&
       (orc_Msg.au8_Data[0] == 0xFDU) && (orc_Msg.u8_DLC >= 2U))
   {
      //it might be an IVA protocol message !
      uint8 u8_ServerID;
      bool q_IsRequest;

      //request or response ?
      q_IsRequest = ((orc_Msg.u32_ID & 0x01U) == 0x01U);
      u8_ServerID = static_cast<uint8>((orc_Msg.u32_ID & 0x7FEU) - 0x50U);

      //request or response ?
      if (q_IsRequest == true)
      {
         c_Text = "IVA -> " + m_GetByteAsStringFormat(u8_ServerID);

         //which service ?
         switch (orc_Msg.au8_Data[1])
         {
         case JOB_LB_IVA_LOGON:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = " LOGON REQ   (*DLC Error*)";
            }
            else
            {
               c_Help = " LOGON REQ  CHK " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                       "  VER " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[4])) +
                       "  NUMVARS " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_LOGOFF:
            if (orc_Msg.u8_DLC != 2U)
            {
               c_Help = " LOGOFF REQ   (*DLC Error*)";
            }
            else
            {
               c_Help = " LOGOFF REQ";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_SERVICE_READ:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = " SERVICE_READ REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " SERVICE_READ REQ  INDEX " +
                                        m_IVAServiceIndexToString(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_SERVICE_WRITE:
            if (orc_Msg.u8_DLC < 5U)
            {
               c_Help = " SERVICE_WRITE REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " SERVICE_WRITE REQ  INDEX " +
                            m_IVAServiceIndexToString(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) + "  DATA [";
               for (u8_Loop = 4U; u8_Loop < orc_Msg.u8_DLC; u8_Loop++)
               {
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Loop]);
                  if (u8_Loop < (orc_Msg.u8_DLC - 1))
                  {
                     c_Help += ",";
                  }
               }
               c_Help += "]";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_INDEX_EXPEDITED:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = " READ_INDEX_EXP REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = m_KFXTextAndIndexToString(" READ_INDEX_EXP REQ ",
                                                    m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_INDEX_EXPEDITED:
            if ((orc_Msg.u8_DLC < 5U) && (orc_Msg.u8_DLC != 7U))
            {
               c_Help = " WRITE_INDEX_EXP REQ   (*DLC Error*)";
            }
            else
            {
               c_Help = m_KFXTextAndIndexToString(" WRITE_INDEX_EXP REQ ",
                            m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) + "  VALUE ";
               switch (orc_Msg.u8_DLC)
               {
               case 5U:
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[4]);
                  break;
               case 6U:
                  c_Help += m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
                  break;
               case 8U:
                  c_Help += m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
                  break;
               default:
                  return "";
               }
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_INDEX_SEG_START:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = " READ_INDEX_SEG_STA REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = m_KFXTextAndIndexToString(" READ_INDEX_SEG_STA REQ ",
                                            m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                                     "  NUMBYTES " + m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_INDEX_SEG_FLOW:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = " READ_INDEX_SEG_FLO REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = m_KFXTextAndIndexToString(" READ_INDEX_SEG_FLO REQ ",
                                            m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                                     "  NUMLEFT " + m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_INDEX_SEG_START:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = " WRITE_INDEX_SEG_STA REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = m_KFXTextAndIndexToString(" WRITE_INDEX_SEG_STA REQ ",
                                                m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                                     "  NUMBYTES " + m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_INDEX_SEG_DATA:
            if (orc_Msg.u8_DLC < 4U)
            {
               c_Help = " WRITE_INDEX_SEG_DAT REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " WRITE_INDEX_SEG_DAT REQ  BLK " + m_GetByteAsStringFormat(orc_Msg.au8_Data[2]) + "  DATA [";
               for (u8_Loop = 3U; u8_Loop < orc_Msg.u8_DLC; u8_Loop++)
               {
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Loop]);
                  if (u8_Loop < (orc_Msg.u8_DLC - 1))
                  {
                     c_Help += ",";
                  }
               }
               c_Help += "]";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_EEP_EXPEDITED:
            if (orc_Msg.u8_DLC != 7U)
            {
               c_Help = " READ_EEP_EXP REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " READ_EEP_EXP REQ  ADDRESS " +
                        m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) + "  NUMBYTES " +
                        m_GetByteAsStringFormat(orc_Msg.au8_Data[6]);
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_EEP_EXPEDITED:
            if (orc_Msg.u8_DLC < 7U)
            {
               c_Help = " WRITE_EEP_EXP REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " WRITE_EEP_EXP REQ  ADDRESS " +
                        m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) + "  DATA [";
               if (orc_Msg.u8_DLC == 7U)
               {
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[6]);
               }
               else
               {
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[6]) + "," +
                            m_GetByteAsStringFormat(orc_Msg.au8_Data[7]);
               }
               c_Help += "]";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_EEP_SEG_START:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = " READ_EEP_SEG_STA REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " READ_EEP_SEG_STA REQ  ADDRESS " +
                        m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) + "  NUMBYTES " +
                        m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_EEP_SEG_FLOW:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = " READ_EEP_SEG_FLO REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " READ_EEP_SEG_FLO REQ  ADDRESS " +
                        m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) + "  NUMLEFT " +
                        m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_EEP_SEG_START:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = " WRITE_EEP_SEG_STA REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " WRITE_EEP_SEG_STA REQ  ADDRESS " +
                        m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) + "  NUMBYTES " +
                        m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_EEP_SEG_DATA:
            if (orc_Msg.u8_DLC < 4U)
            {
               c_Help = " WRITE_EEP_SEG_DAT REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " WRITE_EEP_SEG_DAT REQ  BLK " + m_GetByteAsStringFormat(orc_Msg.au8_Data[2]) + "  DATA [";

               for (u8_Loop = 3U; u8_Loop < orc_Msg.u8_DLC; u8_Loop++)
               {
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Loop]);
                  if (u8_Loop < (orc_Msg.u8_DLC - 1))
                  {
                     c_Help += ",";
                  }
               }
               c_Help += "]";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_UPDATE_LIST_CHECKSUM:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = " UPD_LIST_CHECK REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " UPD_LIST_CHECK REQ  IDX " + m_GetByteAsStringFormat(orc_Msg.au8_Data[2]);
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_CHECK_LIST_CHECKSUM:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = " CHK_LIST_CHECK REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " CHK_LIST_CHECK REQ  IDX " + m_GetByteAsStringFormat(orc_Msg.au8_Data[2]);
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_LIST_DEFAULT:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = " WRITE_LIST_DEFAULT REQ  (*DLC Error*)";
            }
            else
            {
               c_Help = " WRITE_LIST_DEFAULT REQ  IDX " + m_GetByteAsStringFormat(orc_Msg.au8_Data[2]);
            }
            break;
         default:
            c_Text += " unknown service";
            break;
         }
      }
      else
      {
         //it's a response
         c_Text = "IVA <- " + m_GetByteAsStringFormat(u8_ServerID);
         u8_Service = static_cast<uint8>(orc_Msg.au8_Data[1] & 0x7FU);
         q_IsError  = (orc_Msg.au8_Data[1] & 0x80U) != 0U;

         //which service ?
         switch (u8_Service)
         {
         case JOB_LB_IVA_LOGON:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = " LOGON RSP   (*DLC Error*)";
            }
            else
            {
               if (q_IsError == false)
               {
                  c_Help = " LOGON RSP  CHK " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                          "  VER " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[4])) +
                          "  NUMVARS " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
               }
               else
               {
                  c_Help = " LOGON RSP  ERR " + m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                          "  VER " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[4])) +
                          "  NUMVARS " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
               }
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_LOGOFF:
            if ((orc_Msg.u8_DLC == 2U) && (q_IsError == false))
            {
               c_Help = " LOGOFF RSP  OK";
            }
            else if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " LOGOFF RSP  ERR " + m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else
            {
               c_Help = " LOGOFF RSP   (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_SERVICE_READ:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " SERVICE_READ RSP  ERR " +
                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC >= 5U) && (q_IsError == false))
            {
               c_Help = " SERVICE_READ RSP  INDEX " +
                        m_IVAServiceIndexToString(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) + "  DATA [";
               for (u8_Loop = 4U; u8_Loop < orc_Msg.u8_DLC; u8_Loop++)
               {
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Loop]);
                  if (u8_Loop < (orc_Msg.u8_DLC - 1))
                  {
                     c_Help += ",";
                  }
               }
               c_Help += "]";
            }
            else
            {
               c_Help = " SERVICE_READ RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_SERVICE_WRITE:
            if (orc_Msg.u8_DLC == 4U)
            {
               if (q_IsError == true)
               {
                  c_Help = " SERVICE_WRITE RSP  ERR " +
                                           m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               else
               {
                  c_Help = " SERVICE_WRITE RSP  INDEX " +
                                           m_IVAServiceIndexToString(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
            }
            else
            {
               c_Help = " SERVICE_WRITE RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_INDEX_EXPEDITED:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " READ_INDEX_EXP RSP  ERR " +
                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC >= 5U) && (q_IsError == false))
            {
               c_Help = m_KFXTextAndIndexToString(" READ_INDEX_EXP RSP ",
                                        m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                                       "  VALUE ";
               switch (orc_Msg.u8_DLC)
               {
               case 5U:
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[4]);
                  break;
               case 6U:
                  c_Help += m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
                  break;
               case 8U:
                  c_Help += m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
                  break;
               default:
                  return "";
               }
            }
            else
            {
               c_Help = " READ_INDEX_EXP RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_INDEX_EXPEDITED:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = " WRITE_INDEX_EXP RSP   (*DLC Error*)";
            }
            else
            {
               if (q_IsError == true)
               {
                  c_Help = " WRITE_INDEX_EXP RSP  ERR " +
                                           m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               else
               {
                  c_Help = m_KFXTextAndIndexToString(" WRITE_INDEX_EXP RSP ",
                                                       m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_INDEX_SEG_START:
            if (orc_Msg.u8_DLC < 4U)
            {
               c_Help = " READ_INDEX_SEG_DAT RSP  (*DLC Error*)";
            }
            else
            {
               if (q_IsError == true)
               {
                  c_Help = " READ_INDEX_SEG_DAT RSP  ERR " +
                                           m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               else
               {
                  c_Help = " READ_INDEX_SEG_DAT RSP  BLK " +
                                           m_GetByteAsStringFormat(orc_Msg.au8_Data[2]) + "  DATA [";
                  for (u8_Loop = 3U; u8_Loop < orc_Msg.u8_DLC; u8_Loop++)
                  {
                     c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Loop]);
                     if (u8_Loop < (orc_Msg.u8_DLC - 1))
                     {
                        c_Help += ",";
                     }
                  }
                  c_Help += "]";

                  if (orc_Msg.u8_DLC == 4U) //could also be a first response containg the address !
                  {
                     c_Help += m_KFXTextAndIndexToString(" (OR:", m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                                                           ")";
                  }
               }
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_INDEX_SEG_START:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " WRITE_INDEX_SEG_FLO RSP  ERR " +
                                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC == 8U) && (q_IsError == false))
            {
               c_Help = " WRITE_INDEX_SEG_FLO RSP  INDEX " +
                                        m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                                      "  NUMLEFT " + m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            }
            else
            {
               c_Help = " WRITE_INDEX_SEG_FLO RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_INDEX_SEG_DATA:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " WRITE_INDEX_SEG_FIN RSP  ERR " +
                                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC == 8U) && (q_IsError == false))
            {
               c_Help = " WRITE_INDEX_SEG_FIN RSP  INDEX " +
                                        m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                                     "  NUMTOTAL " + m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            }
            else
            {
               c_Help = " WRITE_INDEX_SEG_FIN RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_EEP_EXPEDITED:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " READ_EEP_EXP RSP  ERR " +
                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC >= 7) && (q_IsError == false))
            {
               c_Help = " READ_EEP_EXP RSP  ADDRESS " +
                                        m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) + "  DATA [";
               if (orc_Msg.u8_DLC == 7U)
               {
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[6]) + "]";
               }
               if (orc_Msg.u8_DLC == 8U)
               {
                  c_Help += (m_GetByteAsStringFormat(orc_Msg.au8_Data[6]) + "," +
                             m_GetByteAsStringFormat(orc_Msg.au8_Data[7]) + "]");
               }
            }
            else
            {
               c_Help = " READ_EEP_EXP RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_EEP_EXPEDITED:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " WRITE_EEP_EXP RSP  ERR " +
                                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if (orc_Msg.u8_DLC < 7U)
            {
               c_Help = " WRITE_EEP_EXP RSP  (*DLC Error*)";
            }
            else
            {
               c_Help = " WRITE_EEP_EXP RSP  ADDRESS " +
                        m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) + "  DATA ";
               if (orc_Msg.u8_DLC == 7U)
               {
                  c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[6]);
               }
               else
               {
                  c_Help += (m_GetByteAsStringFormat(orc_Msg.au8_Data[6]) + " " +
                             m_GetByteAsStringFormat(orc_Msg.au8_Data[7]));
               }
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_READ_EEP_SEG_START:
            if (orc_Msg.u8_DLC < 4U)
            {
               c_Help = " READ_EEP_SEG_DAT RSP  (*DLC Error*)";
            }
            else
            {
               if (q_IsError == true)
               {
                  c_Help = " READ_EEP_SEG_DAT RSP  ERR " +
                                           m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               else
               {
                  c_Help = " READ_EEP_SEG_DAT RSP  BLK " +
                                           m_GetByteAsStringFormat(orc_Msg.au8_Data[2]) + "  DATA [";
                  for (u8_Loop = 3U; u8_Loop < orc_Msg.u8_DLC; u8_Loop++)
                  {
                     c_Help += m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Loop]);
                     if (u8_Loop < (orc_Msg.u8_DLC - 1))
                     {
                        c_Help += ",";
                     }
                  }

                  c_Help += "]";

                  if (orc_Msg.u8_DLC == 6U) //could also be a first response containg the address !
                  {
                     c_Help += " (OR: ADDRESS " +
                               m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) + ")";
                  }
               }
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_EEP_SEG_START:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " WRITE_EEP_SEG_FLO RSP  ERR " +
                                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC == 8U) && (q_IsError == false))
            {
               c_Help = " WRITE_EEP_SEG_FLO RSP  ADDRESS " +
                                        m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) +
                                      "  NUMLEFT " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
            }
            else
            {
               c_Help = " WRITE_EEP_SEG_FLO RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_EEP_SEG_DATA:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " WRITE_EEP_SEG_FIN RSP  ERR " +
                                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC == 8U) && (q_IsError == false))
            {
               c_Help = " WRITE_EEP_SEG_FIN RSP  ADDRESS " +
                                        m_GetValueDecHex(m_BytesToDwordLowHigh(&orc_Msg.au8_Data[2])) +
                                     "  NUMTOTAL " + m_GetValueDecHex(m_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
            }
            else
            {
               c_Help = " WRITE_INDEX_SEG_FIN RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_UPDATE_LIST_CHECKSUM:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " UPD_LIST_CHECK RSP  ERR " +
                                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC == 3U) && (q_IsError == false))
            {
               c_Help = " UPD_LIST_CHECK RSP  IDX " + m_GetByteAsStringFormat(orc_Msg.au8_Data[2]);
            }
            else
            {
               c_Help = " UPD_LIST_CHECK RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_CHECK_LIST_CHECKSUM:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " CHK_LIST_CHECK RSP  ERR " + m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC == 3U) && (q_IsError == false))
            {
               c_Help = " CHK_LIST_CHECK RSP  IDX " + m_GetByteAsStringFormat(orc_Msg.au8_Data[2]) + "  OK ";
            }
            else
            {
               c_Help = " CHK_LIST_CHECK RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         case JOB_LB_IVA_WRITE_LIST_DEFAULT:
            if ((orc_Msg.u8_DLC == 4U) && (q_IsError == true))
            {
               c_Help = " WRITE_LIST_DEFAULT RSP  ERR " +
                                        m_IVAGetErrorFromCode(m_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
            }
            else if ((orc_Msg.u8_DLC == 4U) && (q_IsError == false))
            {
               c_Help = " WRITE_LIST_DEFAULT RSP  IDX " +
                                        m_GetByteAsStringFormat(orc_Msg.au8_Data[2]) + "  OK " +
                                        m_GetByteAsStringFormat(orc_Msg.au8_Data[3]);
            }
            else
            {
               c_Help = " WRITE_LIST_DEFAULT RSP  (*DLC Error*)";
            }
            c_Text += c_Help;
            break;
         default:
            c_Text += " unknown service";
            break;
         }
      }
   }
   else if ((orc_Msg.u32_ID <= 0x07FU) && (orc_Msg.u8_RTR == 0U) &&
            ((orc_Msg.au8_Data[0] & 0x18U) == 0x00U) && (orc_Msg.u8_DLC >= 2U))
   {
      //DB0 has reserved bits set to 0 as specified: try to interpret as SIP11 !
      c_Text = m_SIP11MessageToString(orc_Msg);
   }
   else
   {
      //nothing we know about
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_IVAGetErrorFromCode(const uint16 ou16_ErrorCode) const
{
   C_SCLString c_Error;
   switch (ou16_ErrorCode)
   {
   case CA_NO_ERR:
      c_Error = "CA_NO_ERR";
      break;
   case CA_UNKNOWN_ERR:
      c_Error = "CA_UNKNOWN_ERR";
      break;
   case CA_WARN:
      c_Error = "CA_WARN";
      break;
   case CA_DEFAULT:
      c_Error = "CA_DEFAULT";
      break;
   case CA_BUSY:
      c_Error = "CA_BUSY";
      break;
   case CA_RANGE:
      c_Error = "CA_RANGE";
      break;
   case CA_OVERFLOW:
      c_Error = "CA_OVERFLOW";
      break;
   case CA_RD_WR:
      c_Error = "CA_RD_WR";
      break;
   case CA_NOACT:
      c_Error = "CA_NOACT";
      break;
   case CA_COM:
      c_Error = "CA_COM";
      break;
   case CA_CONFIG:
      c_Error = "CA_CONFIG";
      break;
   case CA_CHECKSUM:
      c_Error = "CA_CHECKSUM";
      break;
   default:
      c_Error = m_GetValueDecHex(ou16_ErrorCode);
      break;
   }
   return c_Error;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_IVAServiceIndexToString(const uint16 ou16_ServiceIndex) const
{
   C_SCLString c_Index;
   switch (ou16_ServiceIndex)
   {
   case IVA_SERVICE_PROT_VER:
      c_Index = "PROT_VER";
      break;
   case IVA_SERVICE_IMPL_VER:
      c_Index = "IMPL_VER";
      break;
   case IVA_SERVICE_DEVICE_TYPE1:
      c_Index = "DEVICE_TYPE1";
      break;
   case IVA_SERVICE_DEVICE_TYPE2:
      c_Index = "DEVICE_TYPE2";
      break;
   case IVA_SERVICE_DEVICE_TYPE3:
      c_Index = "DEVICE_TYPE3";
      break;
   case IVA_SERVICE_DEVICE_TYPE4:
      c_Index = "DEVICE_TYPE4";
      break;
   case IVA_SERVICE_OD_VER:
      c_Index = "OD_VER";
      break;
   case IVA_SERVICE_BSMAX_SERVER:
      c_Index = "BSMAX_SERVER";
      break;
   case IVA_SERVICE_STMIN_SERVER:
      c_Index = "STMIN_SERVER";
      break;
   case IVA_SERVICE_BSMAX_CLIENT:
      c_Index = "BSMAX_CLIENT";
      break;
   case IVA_SERVICE_STMIN_CLIENT:
      c_Index = "STMIN_CLIENT";
      break;
   default:
      c_Index = m_GetValueDecHex(ou16_ServiceIndex);
      break;
   }
   return c_Index;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIPMessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SCLString c_Text;
   sint32 s32_Loop;

   uint8 u8_TCFrameType;
   uint8 u8_TCBlockCounter;
   uint8 u8_ServerID;
   uint16 u16_DataSize;

   bool q_IsRequest;

   if (orc_Msg.u8_RTR == 1U)
   {
      return "";
   }

   //get server ID:
   u8_ServerID = static_cast<uint8>((orc_Msg.u32_ID & SIP_TPC_MASK_NSID) >> SIP_TPC_MSG_POS_NSID);

   //request or response ?
   q_IsRequest = ((orc_Msg.u32_ID & SIP_TPC_MASK_TCRQ) ==
           (static_cast<uint32>(SIP_TPC_TCRQ_IS_REQUEST) << SIP_TPC_MSG_POS_TCRQ));

   //which frame type ?
   u8_TCFrameType = static_cast<uint8>((orc_Msg.u32_ID & SIP_TPC_MASK_TCFT) >> SIP_TPC_MSG_POS_TCFT);

   //request or response ?
   if (q_IsRequest == true)
   {
      c_Text = "SIP -> " + m_GetByteAsStringFormat(u8_ServerID);
   }
   else
   {
      c_Text = "SIP <- " + m_GetByteAsStringFormat(u8_ServerID);
   }

   switch (u8_TCFrameType)
   {
   case SIP_TPC_TCFT_EXPEDITED_SINGLE:
      c_Text += " SF";

      u16_DataSize = static_cast<uint16>((orc_Msg.u32_ID & SIP_TPC_MASK_ADS_SINGLE_SEGCON) >> SIP_TPC_MSG_POS_ADS_SINGLE);
      if (orc_Msg.u8_DLC < (u16_DataSize + 1))
      {
         c_Text += " (*DLC Error*)";
      }
      else
      {
         //there have to be at least three bytes in CAN TP
         u16_DataSize = static_cast<uint16>(u16_DataSize + 3U);

         c_Text += " SIZE " + m_GetWordAsStringFormat(u16_DataSize) + "  ";
         c_Text += m_SIPGetApplData(static_cast<uint8>(orc_Msg.u32_ID >> 8), static_cast<uint8>(orc_Msg.u32_ID),
                                    &orc_Msg.au8_Data[0], static_cast<uint8>(u16_DataSize), q_IsRequest);
      }
      break;
   case SIP_TPC_TCFT_SEGMENTED_FIRST:
      c_Text += " FF";

      u16_DataSize = static_cast<uint16>((orc_Msg.u32_ID & SIP_TPC_MASK_ADS_SEGMENTED) >> SIP_TPC_MSG_POS_ADS_SEGMENTED);
      if (u16_DataSize <= SIP_TPC_SEGMENTED_FIRST_MAX_DATA_SIZE)
      {
         //baka: we don't need a segmented transfer for that !
         c_Text += " (*DLC Error*)";
      }
      else if (orc_Msg.u8_DLC != 8U)
      {
         c_Text += " (*DLC Error*)";
      }
      else
      {
         c_Text += " SIZE " + m_GetWordAsStringFormat(u16_DataSize) + "  ";
         c_Text += m_SIPGetApplData(static_cast<uint8>(orc_Msg.u32_ID), orc_Msg.au8_Data[0],
                                    &orc_Msg.au8_Data[1], SIP_TPC_SEGMENTED_FIRST_MAX_DATA_SIZE, q_IsRequest);
      }
      break;
   case SIP_TPC_TCFT_SEGMENTED_CONSECUTIVE:
      c_Text += " CF";
      u8_TCBlockCounter =
                   static_cast<uint8>((orc_Msg.u32_ID & SIP_TPC_MASK_ADS_SINGLE_SEGCON) >> SIP_TPC_MSG_POS_ADS_SINGLE);
      //we accept all DLCs as we do not know how many bytes of the payload are expected ...
      c_Text += " BLK " + m_GetValueDecHex(u8_TCBlockCounter) + "  DATA [";
      c_Text += (m_GetByteAsStringFormat(static_cast<uint8>(orc_Msg.u32_ID >> 8)) + ",");
      c_Text += (m_GetByteAsStringFormat(static_cast<uint8>(orc_Msg.u32_ID)));
      if (orc_Msg.u8_DLC > 0U)
      {
          c_Text += ",";
      }
      for (s32_Loop = 0; s32_Loop < orc_Msg.u8_DLC; s32_Loop++)
      {
         c_Text += (m_GetByteAsStringFormat(orc_Msg.au8_Data[s32_Loop]));
         if (s32_Loop != (orc_Msg.u8_DLC - 1))
         {
            c_Text += ",";
         }
      }
      c_Text += "]";
      break;
   case SIP_TPC_TCFT_ERROR:
      c_Text += " EF  ERROR (" + m_SIPGetTPErrorFromCode(static_cast<uint8>(orc_Msg.u32_ID >> 8)) + ")";
      break;
   default:
      c_Text += " (*TP Error: undefined Frame Type*)";
      break;
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIP11MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SCLString c_Text;
   uint8 u8_Loop;

   uint8 u8_TCFrameType;
   uint8 u8_TCBlockCounter;
   uint8 u8_ServerID;
   uint16 u16_DataSize;

   bool q_IsRequest;

   //get server ID:
   u8_ServerID = static_cast<uint8>((orc_Msg.u32_ID & SIP_TPC11_MASK_NSID) >> SIP_TPC11_MSG_POS_NSID);

   //request or response ?
   q_IsRequest = ((orc_Msg.u32_ID & SIP_TPC11_MASK_TCRQ) == SIP_TPC_TCRQ_IS_REQUEST);

   //which frame type ?
   u8_TCFrameType = static_cast<uint8>((orc_Msg.au8_Data[0] & SIP_TPC11_MASK_TCFT) >> SIP_TPC11_MSG_POS_TCFT);

   //request or response ?
   if (q_IsRequest == true)
   {
      c_Text = "SIP11 -> " + m_GetByteAsStringFormat(u8_ServerID);
   }
   else
   {
      c_Text = "SIP11 <- " + m_GetByteAsStringFormat(u8_ServerID);
   }

   switch (u8_TCFrameType)
   {
   case SIP_TPC_TCFT_EXPEDITED_SINGLE:
      c_Text += " SF";

      u16_DataSize = static_cast<uint16>(orc_Msg.au8_Data[0] & SIP_TPC11_MASK_ADS_SINGLE_SEGCON);
      if (orc_Msg.u8_DLC < (u16_DataSize + 1))
      {
         c_Text += " (*DLC Error*)";
      }
      else
      {
         c_Text += " SIZE " + m_GetWordAsStringFormat(u16_DataSize) + "  ";
         c_Text += m_SIPGetApplData(orc_Msg.au8_Data[1], orc_Msg.au8_Data[2], &orc_Msg.au8_Data[3],
                                    static_cast<uint8>(u16_DataSize), q_IsRequest);
      }
      break;
   case SIP_TPC_TCFT_SEGMENTED_FIRST:
      c_Text += " FF";

      u16_DataSize = static_cast<uint16>((orc_Msg.au8_Data[0] & SIP_TPC11_MASK_ADS_SEGMENTED_MSB) + orc_Msg.au8_Data[1]);

      if (u16_DataSize <= SIP_TPC11_SEGMENTED_FIRST_MAX_DATA_SIZE)
      {
         //baka: we don't need a segmented transfer for that !
         c_Text += " (*DLC Error*)";
      }
      else if (orc_Msg.u8_DLC != 8U)
      {
         c_Text += " (*DLC Error*)";
      }
      else
      {
         c_Text += " SIZE " + m_GetWordAsStringFormat(u16_DataSize) + "  ";
         c_Text += m_SIPGetApplData(orc_Msg.au8_Data[2], orc_Msg.au8_Data[3], &orc_Msg.au8_Data[4],
                                    SIP_TPC11_SEGMENTED_FIRST_MAX_DATA_SIZE, q_IsRequest);
      }
      break;
   case SIP_TPC_TCFT_SEGMENTED_CONSECUTIVE:
      c_Text += " CF";
      u8_TCBlockCounter = (orc_Msg.au8_Data[0] & SIP_TPC11_MASK_ADS_SINGLE_SEGCON);
      //we accept all DLCs as we do not know how many bytes of the payload are expected ...
      c_Text += " BLK " + m_GetValueDecHex(u8_TCBlockCounter) + "  DATA [";
      for (u8_Loop = 1U; u8_Loop < orc_Msg.u8_DLC; u8_Loop++)
      {
         c_Text += (m_GetByteAsStringFormat(orc_Msg.au8_Data[u8_Loop]));
         if (u8_Loop != (orc_Msg.u8_DLC - 1))
         {
            c_Text += ",";
         }
      }
      c_Text += "]";
      break;
   case SIP_TPC_TCFT_ERROR:
      c_Text += " EF  ERROR (" + m_SIPGetTPErrorFromCode(orc_Msg.au8_Data[1]) + ")";
      break;
   default:
      c_Text += " (*TP Error: undefined Frame Type*)";
      break;
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
//\param[in]  ou8_Byte1                1st byte of service (contains AERF, ACLI, AAT)
//\param[in]  ou8_Byte2                1st byte following AAT (if ACLI == 1: first byte of ASH
//                                                             if ACLI == 0: ALIT)
//\param[in]  opu8_FollowingBytes      subsequent bytes (if ACLI == 1: size = (ou8_NumBytesInThisFrame - 2)
//                                      if ACLI == 0: size = (ou8_NumBytesInThisFrame - 4); first byte contains ALII
//\param[in]  ou8_NumBytesInThisFrame  number of bytes in this CAN frame
//\param[in]  oq_IsRequest             true:  service is a request   \n
//                                     false: service is a response
C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIPGetApplData(const uint8 ou8_Byte1, const uint8 ou8_Byte2,
                                                        const uint8 * const opu8_FollowingBytes,
                                                        const uint8 ou8_NumBytesInThisFrame,
                                                        const bool oq_IsRequest) const
{
   C_SCLString c_Text;
   uint8 u8_Index;
   bool q_AERF;    //error flag
   bool q_ACLI;    //contains location information
   uint8 u8_AAT;   //access type
   uint8 u8_ALIT;  //location information type
   uint8 u8_ALII;  //location information index
   SCLDynamicArray<C_CMONProtocolSIPPayload> * pc_Payload;
   uint8 u8_NumBytes;
   uint8 u8_ASH_Byte1;
   const uint8 * pu8_ASH_Byte2;

   q_AERF = ((ou8_Byte1 & 0x80U) != 0U);
   q_ACLI = ((ou8_Byte1 & 0x40U) != 0U);
   u8_AAT = static_cast<uint8>(ou8_Byte1 & 0x3FU);

   if (q_AERF == false)
   {
      c_Text += "OK  ";
   }
   else
   {
      c_Text += "ERR  ";
   }

   u8_NumBytes = ou8_NumBytesInThisFrame;

   c_Text += m_SIPGetAccessType(u8_AAT);
   if (q_ACLI == true)
   {
      if (u8_NumBytes < 3U) //we need at least AAT + ALIT + ALII
      {
         c_Text += " (*APP error: data size too little*)";
         return c_Text;
      }
      u8_ALIT = ou8_Byte2;
      u8_ALII = opu8_FollowingBytes[0];
      c_Text += " LOC " + m_GetByteAsStringFormat(u8_ALIT) + "." + m_GetByteAsStringFormat(u8_ALII);
      u8_NumBytes = static_cast<uint8>(u8_NumBytes - 2U);
      u8_ASH_Byte1  = opu8_FollowingBytes[1];
      pu8_ASH_Byte2 = &opu8_FollowingBytes[2];
   }
   else
   {
      u8_ASH_Byte1 = ou8_Byte2;
      pu8_ASH_Byte2 = &opu8_FollowingBytes[0];
   }

   switch (u8_AAT)
   {
   case SIP_APP_AAT_READ:
   case SIP_APP_AAT_WRITE:
   case SIP_APP_AAT_WRITE_QUOTED:
   case SIP_APP_AAT_READ_CYCLIC:
   case SIP_APP_AAT_READ_STOP:
   case SIP_APP_AAT_DEFINE_DYNAMIC_SERVICE:
      if (u8_NumBytes < 4U) //we need at least AAT + ASH
      {
         c_Text += " (*APP error: data size too little*)";
      }
      else
      {
         //decode header:
         c_Text += m_SIPDecodeSFFFReadWriteHeader(u8_ASH_Byte1, pu8_ASH_Byte2[0], pu8_ASH_Byte2[1], &pc_Payload);
         if (q_AERF == true)
         {
            if (u8_NumBytes < 5U)
            {
               c_Text += " (*APP error: wrong data size*)";
            }
            else
            {
               c_Text += "  CODE " +  m_SIPGetAppErrorFromCode(pu8_ASH_Byte2[2]);
            }
         }
         else
         {
            if ((oq_IsRequest == true) && (u8_AAT == SIP_APP_AAT_READ_CYCLIC))
            {
               if (u8_NumBytes < 6U)
               {
                  c_Text += " (*APP error: wrong data size*)";
               }
               else
               {
                  c_Text += "  TIME " + m_GetWordAsStringFormat(m_BytesToWordLowHigh(&pu8_ASH_Byte2[2])) + "ms";
               }
            }
            else
            {
               if (u8_NumBytes > 4U) //only if there is data ...
               {
                  tgl_assert(pc_Payload != NULL);
                  if (pc_Payload == NULL)
                  {
                     c_Text += "<internal error>";
                  }
                  else
                  {
                     c_Text += m_SIPDecodeData(*pc_Payload, static_cast<uint8>(u8_NumBytes - 4U), &pu8_ASH_Byte2[2]);
                  }
               }
            }
         }
      }
      break;
   case SIP_APP_AAT_READ_STOP_ALL:
      if (u8_NumBytes < 3U) //we need at least AAT + ASH
      {
         c_Text += " (*APP error: wrong data size*)";
      }
      else
      {
         c_Text += "  RES1 " + m_GetByteAsStringFormat(u8_ASH_Byte1) +
                   "  RES2 " + m_GetByteAsStringFormat(*pu8_ASH_Byte2);
      }
      break;
   case SIP_APP_AAT_READ_MEMORY:
      if (u8_NumBytes < 6U) //we need at least AAT + ASH
      {
         c_Text += " (*APP error: wrong data size*)";
      }
      else
      {
         c_Text += (" " + m_SIPGetMemoryRWTypeAndIndex(static_cast<uint8>((u8_ASH_Byte1) >> 5),
                                                       static_cast<uint8>((u8_ASH_Byte1) & 0x1FU)) +
                    " ADD " + m_GetValueDecHex(m_BytesToDwordLowHigh(&pu8_ASH_Byte2[0])));
         if (oq_IsRequest == true)
         {
            //add size:
            if (u8_NumBytes < 8U)
            {
               //either wrong data size or SIP11 protocol (maximum of 6 bytes; SIZE will be in next message)
            }
            else
            {
               c_Text += (" SIZE " + m_GetValueDecHex(m_BytesToWordLowHigh(&pu8_ASH_Byte2[4])));
            }
         }
         else
         {
            if (q_AERF == true)
            {
               if (u8_NumBytes < 7U)
               {
                  //either wrong data size or SIP11 protocol (maximum of 6 bytes; CODE will be in next message)
               }
               else
               {
                  c_Text += "  CODE " +  m_SIPGetAppErrorFromCode(pu8_ASH_Byte2[4]);
               }
            }
            else if (u8_NumBytes > 6U)
            {
               //add data:
               c_Text += " DATA [";
               for (u8_Index = 0U; u8_Index < (u8_NumBytes - 6); u8_Index++)
               {
                  c_Text += (m_GetByteAsStringFormat(pu8_ASH_Byte2[u8_Index + 4]));
                  if (u8_Index != (u8_NumBytes - 7))
                  {
                     c_Text += ",";
                  }
               }
               c_Text += "]";
            }
            else
            {
              //nothing to do ...
            }
         }
      }
      break;
   case SIP_APP_AAT_WRITE_MEMORY:
      if (u8_NumBytes < 6U) //we need at least AAT + ASH
      {
         c_Text += " (*APP error: wrong data size*)";
      }
      else
      {
         c_Text += (" " + m_SIPGetMemoryRWTypeAndIndex(static_cast<uint8>((u8_ASH_Byte1) >> 5),
                                                       static_cast<uint8>((u8_ASH_Byte1) & 0x1FU)) +
                    " ADD " + m_GetValueDecHex(m_BytesToDwordLowHigh(&pu8_ASH_Byte2[0])));
         if (oq_IsRequest == true)
         {
            //add size:
            if (u8_NumBytes < 8U)
            {
               //either wrong data size or SIP11 protocol (maximum of 6 bytes; SIZE will be in next message)
            }
            else
            {
               c_Text += (" SIZE " + m_GetValueDecHex(m_BytesToWordLowHigh(&pu8_ASH_Byte2[4])));

               //add data:
               if (u8_NumBytes > 8U)
               {
                  c_Text += " DATA [";
                  for (u8_Index = 0U; u8_Index < (u8_NumBytes - 8); u8_Index++)
                  {
                     c_Text += (m_GetByteAsStringFormat(pu8_ASH_Byte2[u8_Index + 6]));
                     if (u8_Index != (u8_NumBytes - 9))
                     {
                        c_Text += ",";
                     }
                  }
                  c_Text += "]";
               }
            }
         }
         else
         {
            //response:
            if (q_AERF == true)
            {
               if (u8_NumBytes < 7U)
               {
                  //either wrong data size or SIP11 protocol (maximum of 6 bytes; CODE will be in next message)
               }
               else
               {
                  c_Text += "  CODE " +  m_SIPGetAppErrorFromCode(pu8_ASH_Byte2[4]);
               }
            }
         }
      }
      break;
   case SIP_APP_AAT_READ_OD:
      if (u8_NumBytes < 4U) //we need at least AAT + ASH
      {
         c_Text += " (*APP error: wrong data size*)";
      }
      else
      {
         c_Text += (" " + m_SIPGetODRWTypeAndIndex(u8_ASH_Byte1, m_BytesToWordLowHigh(&pu8_ASH_Byte2[0])));
         if (oq_IsRequest == true)
         {
            //add size:
            if (u8_NumBytes < 6U)
            {
               c_Text += " (*APP error: wrong data size*)";
            }
            else
            {
               c_Text += (" SIZE " + m_GetValueDecHex(m_BytesToWordLowHigh(&pu8_ASH_Byte2[2])));
            }
         }
         else
         {
            if (q_AERF == true)
            {
               if (u8_NumBytes < 5U)
               {
                  c_Text += " (*APP error: wrong data size*)";
               }
               else
               {
                  c_Text += "  CODE " +  m_SIPGetAppErrorFromCode(pu8_ASH_Byte2[2]);
               }
            }
            else if (u8_NumBytes > 4U)
            {
               //add data:
               c_Text += " DATA [";
               for (u8_Index = 0U; u8_Index < (u8_NumBytes - 4); u8_Index++)
               {
                  c_Text += (m_GetByteAsStringFormat(pu8_ASH_Byte2[u8_Index + 2U]));
                  if (u8_Index != (u8_NumBytes - 5))
                  {
                     c_Text += ",";
                  }
               }
               c_Text += "]";
            }
            else
            {
               //nothing to do ...
            }
         }
      }
      break;
   case SIP_APP_AAT_WRITE_OD:
      if (u8_NumBytes < 4U) //we need at least AAT + ASH
      {
         c_Text += " (*APP error: wrong data size*)";
      }
      else
      {
         c_Text += (" " + m_SIPGetODRWTypeAndIndex(u8_ASH_Byte1, m_BytesToWordLowHigh(&pu8_ASH_Byte2[0])));
         if (oq_IsRequest == true)
         {
            //add size:
            if (u8_NumBytes < 6U)
            {
               c_Text += " (*APP error: wrong data size*)";
            }
            else
            {
               c_Text += (" SIZE " + m_GetValueDecHex(m_BytesToWordLowHigh(&pu8_ASH_Byte2[2])));

               //add data:
               if (u8_NumBytes > 6U)
               {
                  c_Text += " DATA [";
                  for (u8_Index = 0; u8_Index < (u8_NumBytes - 6); u8_Index++)
                  {
                     c_Text += (m_GetByteAsStringFormat(pu8_ASH_Byte2[u8_Index + 4U]));
                     if (u8_Index != (u8_NumBytes - 7))
                     {
                        c_Text += ",";
                     }
                  }
                  c_Text += "]";
               }
            }
         }
         else
         {
            //response:
            if (q_AERF == true)
            {
               if (u8_NumBytes < 5U)
               {
                  c_Text += " (*APP error: wrong data size*)";
               }
               else
               {
                  c_Text += "  CODE " +  m_SIPGetAppErrorFromCode(pu8_ASH_Byte2[4]);
               }
            }
         }
      }
      break;
   default: //nothing we can interpret
      if (u8_NumBytes > 1U)
      {
         if (q_AERF == true)
         {
            if (u8_NumBytes < 2U)
            {
               c_Text += " (*APP error: wrong data size*)";
            }
            else
            {
               c_Text += "  CODE: " +  m_SIPGetAppErrorFromCode(u8_ASH_Byte1);
            }
         }
         else
         {
            c_Text += "  DATA [";
            c_Text += m_GetByteAsStringFormat(u8_ASH_Byte1);
            if (u8_NumBytes > 2U)
            {
               if (u8_NumBytes > 3U)
               {
                  c_Text += ",";
               }
               for (u8_Index = 0U; u8_Index < (u8_NumBytes - 2); u8_Index++)
               {
                  c_Text += (m_GetByteAsStringFormat(pu8_ASH_Byte2[u8_Index]));
                  if (u8_Index != (u8_NumBytes - 3))
                  {
                     c_Text += ",";
                  }
               }
            }
            c_Text += "]";
         }
      }

      break;
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIPGetAppErrorFromCode(const uint8 ou8_ErrorCode) const
{
   C_SCLString c_Error;
   switch (ou8_ErrorCode)
   {
   case SIP_APP_ERR_NO_ERR:
      c_Error = "NO_ERROR";
      break;
   case SIP_APP_ERR_UNEXPECTED_FLAG:
      c_Error = "UNEXPECTED_FLAG";
      break;
   case SIP_APP_ERR_ACCESS_TYPE:
      c_Error = "ACCESS_TYPE";
      break;
   case SIP_APP_ERR_SERVICE_AREA:
      c_Error = "SERVICE_AREA";
      break;
   case SIP_APP_ERR_SERVICE_NAME:
      c_Error = "SERVICE_NAME";
      break;
   case SIP_APP_ERR_SERVICE_PARA:
      c_Error = "SERVICE_PARA";
      break;
   case SIP_APP_ERR_READ_WRITE:
      c_Error = "READ_WRITE";
      break;
   case SIP_APP_ERR_DYNAMIC_OVERFLOW:
      c_Error = "DYNAMIC_OVERFLOW";
      break;
   case SIP_APP_ERR_CYCLIC_OVERFLOW:
      c_Error = "CYCLIC_OVERFLOW";
      break;
   case SIP_APP_ERR_DATASIZE:
      c_Error = "DATASIZE";
      break;
   case SIP_APP_ERR_SERVICE:
      c_Error = "SERVICE";
      break;
   case SIP_APP_ERR_SERVICE_ALIT:
      c_Error = "SERVICE_ALIT";
      break;
   default:
      c_Error = m_GetValueDecHex(ou8_ErrorCode);
      break;
   }
   return c_Error;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIPGetTPErrorFromCode(const uint8 ou8_ErrorCode) const
{
   C_SCLString c_Error;
   switch (ou8_ErrorCode)
   {
   case SIP_TPC_ERROR_NO_ERROR:
      c_Error = "NO_ERROR";
      break;
   case SIP_TPC_ERROR_INVALID_DATA_SIZE:
      c_Error = "INVALID_DATA_SIZE";
      break;
   case SIP_TPC_ERROR_STATE:
      c_Error = "STATE";
      break;
   case SIP_TPC_ERROR_DLC:
      c_Error = "DLC";
      break;
   case SIP_TPC_ERROR_BLOCK_COUNTER:
      c_Error = "BLOCK_COUNTER";
      break;
   case SIP_TPC_ERROR_FRAME_TYPE:
      c_Error = "FRAME_TYPE";
      break;
   default:
      c_Error = m_GetValueDecHex(ou8_ErrorCode);
      break;
   }
   return c_Error;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIPGetAccessType(const uint8 ou8_AccessType) const
{
   C_SCLString c_Type;
   switch (ou8_AccessType)
   {
   case SIP_APP_AAT_READ:
      c_Type = "READ";
      break;
   case SIP_APP_AAT_WRITE:
      c_Type = "WRITE";
      break;
   case SIP_APP_AAT_WRITE_QUOTED:
      c_Type = "WRITE_QUOTED";
      break;
   case SIP_APP_AAT_READ_CYCLIC:
      c_Type = "READ_CYCLIC";
      break;
   case SIP_APP_AAT_READ_STOP:
      c_Type = "READ_STOP";
      break;
   case SIP_APP_AAT_READ_STOP_ALL:
      c_Type = "READ_STOP_ALL";
      break;
   case SIP_APP_AAT_DEFINE_DYNAMIC_SERVICE:
      c_Type = "DEFINE_DYNAMIC";
      break;
   case SIP_APP_AAT_READ_MEMORY:
      c_Type = "READ_MEMORY";
      break;
   case SIP_APP_AAT_WRITE_MEMORY:
      c_Type = "WRITE_MEMORY";
      break;
   case SIP_APP_AAT_READ_OD:
      c_Type = "READ_OD";
      break;
   case SIP_APP_AAT_WRITE_OD:
      c_Type = "WRITE_OD";
      break;
   default:
      c_Type = "ACC " + m_GetValueDecHex(ou8_AccessType);
      break;
   }
   return c_Type;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIPGetMemoryRWTypeAndIndex(const uint8 ou8_MemType,
                                                                    const uint8 ou8_MemIndex) const
{
   C_SCLString c_Help;
   //memory types
   switch (ou8_MemType)
   {
   case SIP_APP_MEM_TYPE_EEPROM:
      c_Help = "MEM EEPROM";
      //memory index
      switch (ou8_MemIndex)
      {
      case SIP_APP_MEM_EEP_INDEX_SYSTEM:
         c_Help += " IDX SYSTEM";
         break;
      case SIP_APP_MEM_EEP_INDEX_USER:
         c_Help += " IDX USER";
         break;
      default:
         c_Help += " IDX " + m_GetValueDecHex(ou8_MemIndex);
         break;
      }
      break;
   case SIP_APP_MEM_TYPE_FLASH:
      c_Help = "MEM FLASH";
      //memory index
      switch (ou8_MemIndex)
      {
      case 0:
         c_Help += " IDX MAIN";
         break;
      default:
         c_Help += " IDX " + m_GetValueDecHex(ou8_MemIndex);
         break;
      }
      break;
   default:
      c_Help = ("MEM " + m_GetValueDecHex(ou8_MemType) + " IDX " + m_GetValueDecHex(ou8_MemIndex));
      break;
   }
   return c_Help;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIPGetODRWTypeAndIndex(const uint8 ou8_ODType,
                                                                  const uint16 ou16_VariableIndex) const
{
   C_SCLString c_Help;
   //ID types
   switch (ou8_ODType)
   {
   case SIP_APP_OBJECT_DICTIONARY_TYPE_IV:
      c_Help = "OD IVA";
      //variable index
      c_Help += (" LIS" + m_GetByteAsStringFormat(static_cast<uint8>((ou16_VariableIndex & 0xFC00U) >> 10)) + ".VAR" +
                          m_GetWordAsStringFormat(static_cast<uint16>(ou16_VariableIndex & 0x3FFU)));
      break;
   default:
      //we don't know about the variable index format ...
      c_Help = ("OD " + m_GetValueDecHex(ou8_ODType) + " IDX" + m_GetValueDecHex(ou16_VariableIndex));
      break;
   }
   return c_Help;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name of protocol as string

   \return
   Text representation of protocol name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_CMONProtocolSHIP_IP_IVA::GetProtocolName(void) const
{
   return "STW SHIP-IP!/IVA";
}

//----------------------------------------------------------------------------------------------------------------------

void C_CMONProtocolSIPPayload::Set(const C_SCLString & orc_Name, const uint8 ou8_Size, const bool oq_Signed)
{
   c_Name = orc_Name;
   q_Signed = oq_Signed;
   u8_Size = ou8_Size;
}

//----------------------------------------------------------------------------------------------------------------------

//checking for correct length is the job of the caller !
C_SCLString C_CMONProtocolSIPPayload::Decode(const uint8 * const opu8_Data, const bool oq_Decimal) const
{
   C_SCLString c_Text = " " + c_Name + " ";
   C_SCLString c_Help;
   uint32 u32_Value;
   sint32 s32_Value;
   bool q_AddAsArray = false;
   if (q_Signed == false)
   {
      switch (u8_Size)
      {
      case 1:
         u32_Value = (*opu8_Data);
         break;
      case 2:
         u32_Value = static_cast<uint16>(opu8_Data[0] + ((static_cast<uint16>(opu8_Data[1])) << 8));
         break;
      case 4:
         u32_Value =   static_cast<uint32>(opu8_Data[0] +
                     ((static_cast<uint32>(opu8_Data[1])) << 8) +
                     ((static_cast<uint32>(opu8_Data[2])) << 16) +
                     ((static_cast<uint32>(opu8_Data[3])) << 24));
         break;
      default:
         u32_Value = 0U;
         q_AddAsArray = true;
         break;
      }

      if (q_AddAsArray == false)
      {
         if (oq_Decimal == true)
         {
            (void)c_Help.PrintFormatted("%u", u32_Value);
         }
         else
         {
            (void)c_Help.PrintFormatted("%X", u32_Value);
         }
         c_Text += c_Help;
      }
   }
   else
   {
      switch (u8_Size)
      {
      case 1U:
         s32_Value = (*opu8_Data);
         break;
      case 2U:
         s32_Value = static_cast<sint16>(opu8_Data[0] + ((static_cast<uint16>(opu8_Data[1])) << 8));
         break;
      case 4U:
         s32_Value =   static_cast<sint32>(opu8_Data[0] +
                     ((static_cast<uint32>(opu8_Data[1])) << 8) +
                     ((static_cast<uint32>(opu8_Data[2])) << 16) +
                     ((static_cast<uint32>(opu8_Data[3])) << 24));
         break;
      default:
         q_AddAsArray = true;
         s32_Value = 0;
         break;
      }

      if (q_AddAsArray == false)
      {
         if (oq_Decimal == true)
         {
            (void)c_Help.PrintFormatted("%d", s32_Value);
         }
         else
         {
            if (s32_Value < 0)
            {
               (void)c_Help.PrintFormatted("-%X", std::abs(s32_Value));
            }
            else
            {
               (void)c_Help.PrintFormatted("%X", s32_Value);
            }
         }
         c_Text += c_Help;
      }
   }

   if (q_AddAsArray == true)
   {

      uint8 u8_Index;
      c_Text += "[";
      for (u8_Index = 0U; u8_Index < u8_Size; u8_Index++)
      {
         if (oq_Decimal == true)
         {
            (void)c_Help.PrintFormatted("%03d", opu8_Data[u8_Index]);
         }
         else
         {
            (void)c_Help.PrintFormatted(" %02X", opu8_Data[u8_Index]);
         }

         c_Text += c_Help;
         if (u8_Index != (u8_Size - 1))
         {
            c_Text += ",";
         }
      }
      c_Text += "]";
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CMONProtocolSIPASPDescription::Set(const C_SCLString & orc_ShortName, const uint16 ou16_Length)
{
   c_ShortName = orc_ShortName;
   u16_Length  = ou16_Length;
}

//----------------------------------------------------------------------------------------------------------------------

void C_CMONProtocolSIPASNDescription::Set(const C_SCLString & orc_ShortName, const uint8 ou8_NumASPs,
                                          const C_SCLString & orc_ASPPrefix, const uint16 ou16_Length)
{
   c_ShortName = orc_ShortName;
   c_ASPPrefix = orc_ASPPrefix;
   u16_Length  = ou16_Length;
   c_ASPs.SetLength(ou8_NumASPs);
}

//----------------------------------------------------------------------------------------------------------------------
//init service table for interpretation of SHIP-IP! services
//Ideally this whole function will be XSLT-generated. So we don't need any manual changes when new services are defined.
//On the other hand this implementation only needs to be done once for each service, in contrast to having to
// generate the target specific service interpretation layer on the server side for each target.
//When editing this table it is strongly recommended to run this function with Borland Codeguard activated to make
// sure there is no accidental out of bounds indexing.
void C_CMONProtocolSHIP_IP_IVA::hm_InitServiceTable(void)
{
   hmc_Services.SetLength(SIP_ASA_ETHERNET + 1);

   hmc_Services[SIP_ASA_PROTOCOL].c_ShortName = "PROTOCOL";
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs.SetLength(SIP_ASN_PROT_SERVICE_VERSION + 1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_VERSION].Set("VERSION", SIP_ASP_PROT_VERSION_GET + 1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_VERSION].c_ASPs[SIP_ASP_PROT_VERSION_GET].Set("GET", SIP_PSIZE_PROT_VERSION_GET);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_VERSION].c_ASPs[SIP_ASP_PROT_VERSION_GET].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_VERSION].c_ASPs[SIP_ASP_PROT_VERSION_GET].c_Payload[0].Set("VERSION", 2, false);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXBUFFERSIZE].Set("MAXBUFFERSIZE", SIP_ASP_PROT_MAXBUFFERSIZE_GET + 1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXBUFFERSIZE].c_ASPs[SIP_ASP_PROT_MAXBUFFERSIZE_GET].Set("GET", SIP_PSIZE_PROT_MAXBUFFERSIZE_GET);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXBUFFERSIZE].c_ASPs[SIP_ASP_PROT_MAXBUFFERSIZE_GET].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXBUFFERSIZE].c_ASPs[SIP_ASP_PROT_MAXBUFFERSIZE_GET].c_Payload[0].Set("SIZE", 2, false);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXNUMCYCLICTRANSMISSIONS].Set("MAXNUMCYCLICTRANSMISSIONS", SIP_ASP_PROT_MAXNUMCYCLICTRANSMISSIONS_GET + 1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXNUMCYCLICTRANSMISSIONS].c_ASPs[SIP_ASP_PROT_MAXNUMCYCLICTRANSMISSIONS_GET].Set("GET", SIP_PSIZE_PROT_MAXNUMCYCLICTRANSMISSIONS_GET);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXNUMCYCLICTRANSMISSIONS].c_ASPs[SIP_ASP_PROT_MAXNUMCYCLICTRANSMISSIONS_GET].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXNUMCYCLICTRANSMISSIONS].c_ASPs[SIP_ASP_PROT_MAXNUMCYCLICTRANSMISSIONS_GET].c_Payload[0].Set("NUM", 2, false);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXNUMDYNAMICCONFIGURATIONS].Set("MAXNUMDYNAMICCONFIGURATIONS", SIP_ASP_PROT_MAXNUMDYNAMICCONFIGURATIONS_GET + 1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXNUMDYNAMICCONFIGURATIONS].c_ASPs[SIP_ASP_PROT_MAXNUMDYNAMICCONFIGURATIONS_GET].Set("GET", SIP_PSIZE_PROT_MAXNUMDYNAMICCONFIGURATIONS_GET);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXNUMDYNAMICCONFIGURATIONS].c_ASPs[SIP_ASP_PROT_MAXNUMDYNAMICCONFIGURATIONS_GET].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_MAXNUMDYNAMICCONFIGURATIONS].c_ASPs[SIP_ASP_PROT_MAXNUMDYNAMICCONFIGURATIONS_GET].c_Payload[0].Set("NUM", 2, false);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_PARAMETER].Set("PARAMETER", SIP_ASP_PROT_PARAMETER_BITRATE + 1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_PARAMETER].c_ASPs[SIP_ASP_PROT_PARAMETER_BITRATE].Set("BITRATE", SIP_PSIZE_PROT_PARAMETER_BITRATE);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_PARAMETER].c_ASPs[SIP_ASP_PROT_PARAMETER_BITRATE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_PARAMETER].c_ASPs[SIP_ASP_PROT_PARAMETER_BITRATE].c_Payload[0].Set("BITRATE", 4, false);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_SERVICE_VERSION].Set("SERVICE_VERSION", SIP_ASP_PROT_SERVICE_VERSION_GET + 1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_SERVICE_VERSION].c_ASPs[SIP_ASP_PROT_SERVICE_VERSION_GET].Set("GET", SIP_PSIZE_PROT_SERVICE_VERSION_GET);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_SERVICE_VERSION].c_ASPs[SIP_ASP_PROT_SERVICE_VERSION_GET].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_PROTOCOL].c_ASNs[SIP_ASN_PROT_SERVICE_VERSION].c_ASPs[SIP_ASP_PROT_SERVICE_VERSION_GET].c_Payload[0].Set("VERSION", 2, false);

   hmc_Services[SIP_ASA_APPLICATION].c_ShortName = "APPLICATION";
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs.SetLength(SIP_ASN_APP_DESTROY + 1);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_VERSION].Set("VERSION", SIP_ASP_APP_VERSION_GET + 1);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_VERSION].c_ASPs[SIP_ASP_APP_VERSION_GET].Set("GET", SIP_PSIZE_APP_VERSION_GET);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_VERSION].c_ASPs[SIP_ASP_APP_VERSION_GET].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_VERSION].c_ASPs[SIP_ASP_APP_VERSION_GET].c_Payload[0].Set("VERSION", 2, false);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_BIOS].Set("BIOS", SIP_ASP_APP_BIOS_VERSION + 1);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_BIOS].c_ASPs[SIP_ASP_APP_BIOS_VERSION].Set("VERSION", SIP_PSIZE_APP_BIOS_VERSION);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_BIOS].c_ASPs[SIP_ASP_APP_BIOS_VERSION].c_Payload.SetLength(3);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_BIOS].c_ASPs[SIP_ASP_APP_BIOS_VERSION].c_Payload[0].Set("MA", 1, false);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_BIOS].c_ASPs[SIP_ASP_APP_BIOS_VERSION].c_Payload[1].Set("MI", 1, false);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_BIOS].c_ASPs[SIP_ASP_APP_BIOS_VERSION].c_Payload[2].Set("REL", 2, false);
   hmc_Services[SIP_ASA_APPLICATION].c_ASNs[SIP_ASN_APP_DESTROY].Set("DESTROY", 0, "CH", 0);

   hmc_Services[SIP_ASA_SYSTEM].c_ShortName = "SYSTEM";
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs.SetLength(SIP_ASN_SYS_ADC_FILTER + 1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].Set("INFO", SIP_ASP_SYS_INFO_STATUS_INIT + 1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_SERIAL_NUMBER].Set("SERIAL_NUM", SIP_PSIZE_SYS_INFO_SERIAL_NUMBER);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_SERIAL_NUMBER].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_SERIAL_NUMBER].c_Payload[0].Set("SNR", 6, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_ARTICLE_NUMBER].Set("ARTICLE_NUM", SIP_PSIZE_SYS_INFO_ARTICLE_NUMBER);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_ARTICLE_NUMBER].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_ARTICLE_NUMBER].c_Payload[0].Set("NUM", 4, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_DEVICE_ID].Set("DEVICE_ID", SIP_PSIZE_SYS_INFO_DEVICE_ID);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_DEVICE_ID].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_DEVICE_ID].c_Payload[0].Set("ID", 17, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_RAM_SIZE].Set("RAM_SIZE", SIP_PSIZE_SYS_INFO_RAM_SIZE);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_RAM_SIZE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_RAM_SIZE].c_Payload[0].Set("SIZE_KB", 4, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_ROM_SIZE].Set("ROM_SIZE", SIP_PSIZE_SYS_INFO_ROM_SIZE);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_ROM_SIZE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_ROM_SIZE].c_Payload[0].Set("SIZE_KB", 4, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_EEPROM_SIZE].Set("EEPROM_SIZE", SIP_PSIZE_SYS_INFO_EEPROM_SIZE);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_EEPROM_SIZE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_EEPROM_SIZE].c_Payload[0].Set("SIZE_B", 4, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_STATUS_INIT].Set("STATUS_INIT", SIP_PSIZE_SYS_INFO_STATUS_INIT);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_STATUS_INIT].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_STATUS_INIT].c_Payload[0].Set("STATUS", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_INFO].c_ASPs[SIP_ASP_SYS_INFO_STATUS_INIT].c_Payload[1].Set("INFORMATION", 4, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_RESET].Set("RESET", SIP_ASP_SYS_RESET_EXECUTE + 1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_RESET].c_ASPs[SIP_ASP_SYS_RESET_EXECUTE].Set("EXECUTE", 0);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_IGNITION].Set("IGNITION", SIP_ASP_SYS_IGNITION_STATUS + 1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_IGNITION].c_ASPs[SIP_ASP_SYS_IGNITION_CONFIG].Set("ONOFF", SIP_PSIZE_SYS_IGNITION_CONFIG);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_IGNITION].c_ASPs[SIP_ASP_SYS_IGNITION_CONFIG].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_IGNITION].c_ASPs[SIP_ASP_SYS_IGNITION_CONFIG].c_Payload[0].Set("STATE", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_IGNITION].c_ASPs[SIP_ASP_SYS_IGNITION_STATUS].Set("ONOFF", SIP_PSIZE_SYS_IGNITION_STATUS);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_IGNITION].c_ASPs[SIP_ASP_SYS_IGNITION_STATUS].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_IGNITION].c_ASPs[SIP_ASP_SYS_IGNITION_STATUS].c_Payload[0].Set("STATUS", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_BEEPER].Set("BEEPER", 0, "CH", SIP_PSIZE_SYS_BEEPER);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_BEEPER].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_BEEPER].c_Payload[0].Set("ONOFF", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_BEEPER].c_Payload[1].Set("FREQU_HZ", 2, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_LED].Set("LED", 0, "CH", SIP_PSIZE_SYS_LED);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_LED].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_LED].c_Payload[0].Set("ONOFF", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_LED].c_Payload[1].Set("COLOR", 2, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_TEMPERATURE].Set("TEMPERATURE", SIP_ASP_SYS_TEMPERATURE); // ASPE service! SYSTEM 0, CPU 1, free channels 2-250
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_TEMPERATURE].c_ASPs[SIP_ASP_SYS_TEMPERATURE_SYSTEM].Set("SYSTEM", SIP_PSIZE_SYS_TEMPERATURE_SYSTEM);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_TEMPERATURE].c_ASPs[SIP_ASP_SYS_TEMPERATURE_SYSTEM].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_TEMPERATURE].c_ASPs[SIP_ASP_SYS_TEMPERATURE_SYSTEM].c_Payload[0].Set("TEMPERATURE_1_10_C", 2, true);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_TEMPERATURE].c_ASPs[SIP_ASP_SYS_TEMPERATURE_CPU].Set("CPU", SIP_PSIZE_SYS_TEMPERATURE_CPU);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_TEMPERATURE].c_ASPs[SIP_ASP_SYS_TEMPERATURE_CPU].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_TEMPERATURE].c_ASPs[SIP_ASP_SYS_TEMPERATURE_CPU].c_Payload[0].Set("TEMPERATURE_1_10_C", 2, true);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_RELAY].Set("RELAY", 0, "CH", SIP_PSIZE_SYS_RELAY);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_RELAY].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_RELAY].c_Payload[0].Set("ONOFF", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_SENSOR_SUPPLY].Set("SENSOR_SUPPLY", 0, "CH", SIP_PSIZE_SYS_SENSOR_SUPPLY);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_SENSOR_SUPPLY].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_SENSOR_SUPPLY].c_Payload[0].Set("VOLTAGE_mV", 4, true);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_BUTTON].Set("BUTTON", 0, "CH", SIP_PSIZE_SYS_BUTTON);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_BUTTON].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_BUTTON].c_Payload[0].Set("ONOFF", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_VALUE_SCALED_32BIT].Set("ADC_VALUE_SCALED_32BIT", 0, "CH", SIP_PSIZE_SYS_ADC_VALUE_SCALED_32BIT);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_VALUE_SCALED_32BIT].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_VALUE_SCALED_32BIT].c_Payload[0].Set("VALUE", 4, true);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].Set("DIAG", SIP_ASP_SYS_DIAG_LAST_RESET_REASON + 1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].c_ASPs[SIP_ASP_SYS_DIAG_VOLTAGE_UNDER].Set("VOLTAGE_UNDER", SIP_PSIZE_SYS_DIAG_VOLTAGE_UNDER);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].c_ASPs[SIP_ASP_SYS_DIAG_VOLTAGE_UNDER].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].c_ASPs[SIP_ASP_SYS_DIAG_VOLTAGE_UNDER].c_Payload[0].Set("UNDERVOLTAGE", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].c_ASPs[SIP_ASP_SYS_DIAG_VOLTAGE_OVER].Set("VOLTAGE_OVER", SIP_PSIZE_SYS_DIAG_VOLTAGE_OVER);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].c_ASPs[SIP_ASP_SYS_DIAG_VOLTAGE_OVER].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].c_ASPs[SIP_ASP_SYS_DIAG_VOLTAGE_OVER].c_Payload[0].Set("OVERVOLTAGE", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].c_ASPs[SIP_ASP_SYS_DIAG_LAST_RESET_REASON].Set("LAST_RESET_REASON", SIP_PSIZE_SYS_DIAG_LAST_RESET_REASON);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].c_ASPs[SIP_ASP_SYS_DIAG_LAST_RESET_REASON].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_DIAG].c_ASPs[SIP_ASP_SYS_DIAG_LAST_RESET_REASON].c_Payload[0].Set("STATUS", 4, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_CALIBRATION].Set("ADC_CALIBRATION", 0, "CH", 0);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_CALIBRATION_STATE].Set("ADC_CALIBRATION_STATE", 0, "CH", SIP_PSIZE_SYS_ADC_CALIBRATION_STATE);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_CALIBRATION_STATE].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_CALIBRATION_STATE].c_Payload[0].Set("STATE", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_CALIBRATION_STATE].c_Payload[1].Set("USED", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_VALUE_16BIT].Set("ADC_VALUE_16BIT", 0, "CH", SIP_PSIZE_SYS_ADC_VALUE_16BIT);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_VALUE_16BIT].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_VALUE_16BIT].c_Payload[0].Set("VALUE", 2, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_RELAY_ERROR_COUNTERS].Set("RELAY_ERROR_COUNTERS", 0, "CH", SIP_PSIZE_SYS_RELAY_ERROR_COUNTERS);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_RELAY_ERROR_COUNTERS].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_RELAY_ERROR_COUNTERS].c_Payload[0].Set("ERROR_COUNTERS", 8, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_RELAY_RESET_ERROR_COUNTERS].Set("RELAY_RESET_ERROR_COUNTERS", 0, "CH", 0);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_COMPONENT_COUNT].Set("COMPONENT_COUNT", SIP_ASP_SYS_COMPONENT_COUNT_GET + 1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_COMPONENT_COUNT].c_ASPs[SIP_ASP_SYS_COMPONENT_COUNT_GET].Set("GET", SIP_PSIZE_SYS_COMPONENT_COUNT_GET);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_COMPONENT_COUNT].c_ASPs[SIP_ASP_SYS_COMPONENT_COUNT_GET].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_COMPONENT_COUNT].c_ASPs[SIP_ASP_SYS_COMPONENT_COUNT_GET].c_Payload[0].Set("NUMBER_OF_COMPONENTS", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_COMPONENT_VERSION].Set("COMPONENT_VERSION", 0, "CH", SIP_PSIZE_SYS_COMPONENT_VERSION);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_COMPONENT_VERSION].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_COMPONENT_VERSION].c_Payload[0].Set("NAME", 20, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_COMPONENT_VERSION].c_Payload[1].Set("VERSION", 15, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_WAKEUP].Set("WAKEUP", 0, "CH", SIP_PSIZE_SYS_WAKEUP);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_WAKEUP].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_WAKEUP].c_Payload[0].Set("FUNCTION", 1, false);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_CLOSE].Set("CLOSE", SIP_ASP_SYS_CLOSE_EXECUTE + 1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_CLOSE].c_ASPs[SIP_ASP_SYS_CLOSE_EXECUTE].Set("EXECUTE", 0);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_FILTER].Set("ADC_FILTER", 0, "CH", SIP_PSIZE_SYS_ADC_FILTER);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_FILTER].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SYSTEM].c_ASNs[SIP_ASN_SYS_ADC_FILTER].c_Payload[0].Set("TIME", 4, false);

   hmc_Services[SIP_ASA_INPUT].c_ShortName = "INPUT";
   hmc_Services[SIP_ASA_INPUT].c_ASNs.SetLength(SIP_ASN_I_VALUE_UNSCALED + 1);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_INIT].Set("INIT", 0, "CH", SIP_PSIZE_I_INIT);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_INIT].c_Payload.SetLength(3);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_INIT].c_Payload[0].Set("TYPE", 2, false);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_INIT].c_Payload[1].Set("FILTER_PERIOD", 2, false);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_INIT].c_Payload[2].Set("CIRCUIT", 4, false);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_VALUE].Set("VALUE", 0, "CH", SIP_PSIZE_I_VALUE);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_VALUE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_VALUE].c_Payload[0].Set("VALUE", 4, true);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_COUNTER].Set("COUNTER", 0, "CH", SIP_PSIZE_I_COUNTER);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_COUNTER].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_COUNTER].c_Payload[0].Set("COUNTER", 4, true);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_DUTY_CYCLE].Set("DUTY_CYCLE", 0, "CH", SIP_PSIZE_I_DUTY_CYCLE);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_DUTY_CYCLE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_DUTY_CYCLE].c_Payload[0].Set("DUTY_CYCLE_1_1000", 4, false);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_STATE].Set("STATE", 0, "CH", SIP_PSIZE_I_STATE);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_STATE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_STATE].c_Payload[0].Set("ONOFF", 1, false);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_EVENT_COUNTER].Set("EVENT_COUNTER", 0, "CH", SIP_PSIZE_I_EVENT_COUNTER);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_EVENT_COUNTER].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_EVENT_COUNTER].c_Payload[0].Set("VALUE", 2, false);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_TEST_COMPARATOR].Set("TEST_COMPARATOR", 0, "CH", SIP_PSIZE_I_TEST_COMPARATOR);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_TEST_COMPARATOR].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_TEST_COMPARATOR].c_Payload[0].Set("STATUS", 1, false);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_VALUE_UNSCALED].Set("VALUE", 0, "CH", SIP_PSIZE_I_VALUE_UNSCALED);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_VALUE_UNSCALED].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INPUT].c_ASNs[SIP_ASN_I_VALUE_UNSCALED].c_Payload[0].Set("VALUE", 2, false);

   hmc_Services[SIP_ASA_OUTPUT].c_ShortName = "OUTPUT";
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs.SetLength(SIP_ASN_O_OVERCURRENT_TESTMODE + 1);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_INIT].Set("INIT", 0, "CH", SIP_PSIZE_O_INIT);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_INIT].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_INIT].c_Payload[0].Set("TYPE", 2, false);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_INIT].c_Payload[1].Set("CIRCUIT", 4, false);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_VALUE].Set("VALUE", 0, "CH", SIP_PSIZE_O_VALUE);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_VALUE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_VALUE].c_Payload[0].Set("VALUE", 4, true);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_FREQUENCY].Set("FREQUENCY", 0, "CH", SIP_PSIZE_O_FREQUENCY);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_FREQUENCY].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_FREQUENCY].c_Payload[0].Set("FREQUENCY_mHz", 4, false);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_STATE].Set("STATE", 0, "CH", SIP_PSIZE_O_STATE);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_STATE].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_STATE].c_Payload[0].Set("ONOFF", 1, false);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_STATE].c_Payload[1].Set("OVERCURRENT", 1, false);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CURRENT].Set("CURRENT", 0, "CH", SIP_PSIZE_O_CURRENT);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CURRENT].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CURRENT].c_Payload[0].Set("CURRENT_uA", 4, true);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_VOLTAGE].Set("VOLTAGE", 0, "CH", SIP_PSIZE_O_VOLTAGE);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_VOLTAGE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_VOLTAGE].c_Payload[0].Set("VOLTAGE_mV", 4, true);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CONTROL_PARAMETER].Set("CONTROL_PARAMETER", 0, "CH", SIP_PSIZE_O_CONTROL_PARAMETER);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CONTROL_PARAMETER].c_Payload.SetLength(5);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CONTROL_PARAMETER].c_Payload[0].Set("SETPOINT", 4, true);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CONTROL_PARAMETER].c_Payload[1].Set("P_VALUE_mKR", 4, true);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CONTROL_PARAMETER].c_Payload[2].Set("I_VALUE_mS", 2, true);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CONTROL_PARAMETER].c_Payload[3].Set("D_VALUE_mS", 2, true);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_CONTROL_PARAMETER].c_Payload[4].Set("SAMPLE_TIME_mS", 2, true);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_EVENT_ERROR].Set("EVENT_ERROR", 0, "CH", SIP_PSIZE_O_EVENT_ERROR);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_EVENT_ERROR].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_EVENT_ERROR].c_Payload[0].Set("VALUE", 2, false);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_STATE_RESET].Set("STATE_RESET", 0, "CH", 0);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_OVERCURRENT_TESTMODE].Set("OVERCURRENT_TESTMODE", 0, "CH", SIP_PSIZE_O_OVERCURRENT_TESTMODE);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_OVERCURRENT_TESTMODE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_OUTPUT].c_ASNs[SIP_ASN_O_OVERCURRENT_TESTMODE].c_Payload[0].Set("FUNCTION", 1, false);

   hmc_Services[SIP_ASA_SERIAL].c_ShortName = "SERIAL";
   hmc_Services[SIP_ASA_SERIAL].c_ASNs.SetLength(SIP_ASN_SER_CLOSE + 1);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_INIT].Set("INIT", 0, "CH", SIP_PSIZE_SER_INIT);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_INIT].c_Payload.SetLength(3);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_INIT].c_Payload[0].Set("BITRATE", 4, false);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_INIT].c_Payload[1].Set("MODE", 2, false);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_INIT].c_Payload[2].Set("STOPBITS", 1, false);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_RX_BUFFER].Set("RX_BUFFER", 0, "CH", SIP_PSIZE_SER_RX_BUFFER);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_RX_BUFFER].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_RX_BUFFER].c_Payload[0].Set("NUM_BYTES", 1, false);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_RX_BUFFER].c_Payload[1].Set("DATA", 8, false);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_TX_BUFFER].Set("TX_BUFFER", 0, "CH", SIP_PSIZE_SER_TX_BUFFER);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_TX_BUFFER].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_TX_BUFFER].c_Payload[0].Set("NUM_BYTES", 1, false);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_TX_BUFFER].c_Payload[1].Set("DATA", 8, false);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_CLEAR_RX_BUFFER].Set("CLEAR_RX_BUFFER", 0, "CH", 0);
   hmc_Services[SIP_ASA_SERIAL].c_ASNs[SIP_ASN_SER_CLOSE].Set("CLOSE", 0, "CH", 0);

   hmc_Services[SIP_ASA_CAN].c_ShortName = "CAN";
   hmc_Services[SIP_ASA_CAN].c_ASNs.SetLength(SIP_ASN_CAN_CLOSE + 1);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_INIT].Set("INIT", 0, "CH", SIP_PSIZE_CAN_INIT);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_INIT].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_INIT].c_Payload[0].Set("BITRATE_bpS", 4, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_BUS_STATE].Set("BUS_STATE", 0, "CH", SIP_PSIZE_CAN_BUS_STATE);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_BUS_STATE].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_BUS_STATE].c_Payload[0].Set("STATE", 1, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_ID].Set("RX_ID", 0, "CH", SIP_PSIZE_CAN_RX_ID);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_ID].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_ID].c_Payload[0].Set("ID", 4, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_ID].c_Payload[1].Set("XTD", 1, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_MESSAGE].Set("RX_MESSAGE", 0, "CH", SIP_PSIZE_CAN_RX_MESSAGE);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_MESSAGE].c_Payload.SetLength(5);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_MESSAGE].c_Payload[0].Set("VALID", 1, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_MESSAGE].c_Payload[1].Set("ID", 4, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_MESSAGE].c_Payload[2].Set("XTD", 1, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_MESSAGE].c_Payload[3].Set("DLC", 1, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_RX_MESSAGE].c_Payload[4].Set("DATA", 8, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_TX_MESSAGE].Set("TX_MESSAGE", 0, "CH", SIP_PSIZE_CAN_TX_MESSAGE);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_TX_MESSAGE].c_Payload.SetLength(4);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_TX_MESSAGE].c_Payload[0].Set("ID", 4, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_TX_MESSAGE].c_Payload[1].Set("XTD", 1, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_TX_MESSAGE].c_Payload[2].Set("DLC", 1, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_TX_MESSAGE].c_Payload[3].Set("DATA", 8, false);
   hmc_Services[SIP_ASA_CAN].c_ASNs[SIP_ASN_CAN_CLOSE].Set("CLOSE", 0, "CH", 0);

   hmc_Services[SIP_ASA_MEMORY].c_ShortName = "MEMORY";
   hmc_Services[SIP_ASA_MEMORY].c_ASNs.SetLength(SIP_ASN_MEM_TEST_FAILURE_DETAILS + 1);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST].Set("TEST", 0, "CH", SIP_PSIZE_MEM_TEST);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST].c_Payload.SetLength(5);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST].c_Payload[0].Set("TYPE", 1, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST].c_Payload[1].Set("CONNECTION", 1, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST].c_Payload[2].Set("START", 4, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST].c_Payload[3].Set("SIZE", 4, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST].c_Payload[4].Set("MODE", 1, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_INFO].Set("INFO", SIP_ASP_MEM_INFO_FLASH_LOADER_VERSION + 1);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_INFO].c_ASPs[SIP_ASP_MEM_INFO_FLASH_LOADER_VERSION].Set("FLASH_LOADER_VERSION", SIP_PSIZE_MEM_INFO_FLASH_LOADER_VERSION);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_INFO].c_ASPs[SIP_ASP_MEM_INFO_FLASH_LOADER_VERSION].c_Payload.SetLength(4);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_INFO].c_ASPs[SIP_ASP_MEM_INFO_FLASH_LOADER_VERSION].c_Payload[0].Set("MAJORVERSION", 1, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_INFO].c_ASPs[SIP_ASP_MEM_INFO_FLASH_LOADER_VERSION].c_Payload[1].Set("MINORVERSION1", 1, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_INFO].c_ASPs[SIP_ASP_MEM_INFO_FLASH_LOADER_VERSION].c_Payload[2].Set("MINORVERSION2", 1, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_INFO].c_ASPs[SIP_ASP_MEM_INFO_FLASH_LOADER_VERSION].c_Payload[3].Set("RELEASE", 2, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST_FAILURE_DETAILS].Set("TEST_FAILURE_DETAILS", SIP_ASP_MEM_TEST_FAILURE_DETAILS_GET + 1);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST_FAILURE_DETAILS].c_ASPs[SIP_ASP_MEM_TEST_FAILURE_DETAILS_GET].Set("GET", SIP_PSIZE_MEM_TEST_FAILURE_DETAILS_GET);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST_FAILURE_DETAILS].c_ASPs[SIP_ASP_MEM_TEST_FAILURE_DETAILS_GET].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST_FAILURE_DETAILS].c_ASPs[SIP_ASP_MEM_TEST_FAILURE_DETAILS_GET].c_Payload[0].Set("FAILURE_REASON", 1, false);
   hmc_Services[SIP_ASA_MEMORY].c_ASNs[SIP_ASN_MEM_TEST_FAILURE_DETAILS].c_ASPs[SIP_ASP_MEM_TEST_FAILURE_DETAILS_GET].c_Payload[1].Set("ADDRESS", 4, false);

   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ShortName = "IVA";
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs.SetLength(SIP_ASN_IVA_DEFAULTS + 1);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].Set("SERVICE", SIP_ASP_IVA_SERVICE_OBJECT_DICTIONARY_VERSION + 1);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].c_ASPs[SIP_ASP_IVA_SERVICE_LOGON].Set("LOGON", SIP_PSIZE_IVA_SERVICE_LOGON);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].c_ASPs[SIP_ASP_IVA_SERVICE_LOGON].c_Payload.SetLength(3);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].c_ASPs[SIP_ASP_IVA_SERVICE_LOGON].c_Payload[0].Set("CHECKSUM", 2, false);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].c_ASPs[SIP_ASP_IVA_SERVICE_LOGON].c_Payload[1].Set("VERSION", 2, false);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].c_ASPs[SIP_ASP_IVA_SERVICE_LOGON].c_Payload[2].Set("NUM_BYTES", 2, false);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].c_ASPs[SIP_ASP_IVA_SERVICE_LOGOFF].Set("LOGOFF", 0);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].c_ASPs[SIP_ASP_IVA_SERVICE_OBJECT_DICTIONARY_VERSION].Set("OD_VERSION", SIP_PSIZE_IVA_SERVICE_OBJECT_DICTIONARY_VERSION);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].c_ASPs[SIP_ASP_IVA_SERVICE_OBJECT_DICTIONARY_VERSION].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_SERVICE].c_ASPs[SIP_ASP_IVA_SERVICE_OBJECT_DICTIONARY_VERSION].c_Payload[0].Set("VERSION", 2, false);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_CHECKSUM_CHECK].Set("CHECKSUM_CHECK", 0, "LIST", SIP_PSIZE_IVA_CHECKSUM_CHECK);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_CHECKSUM_CHECK].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_CHECKSUM_CHECK].c_Payload[0].Set("OK", 1, false);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_CHECKSUM_UPDATE].Set("CHECKSUM_UPDATE", 0, "LIST", 0);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_DEFAULTS].Set("DEFAULTS", 0, "LIST", SIP_PSIZE_IVA_DEFAULTS);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_DEFAULTS].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_INDEX_VARIABLE_ACCESS].c_ASNs[SIP_ASN_IVA_DEFAULTS].c_Payload[0].Set("SET_INDEX", 1, false);

   hmc_Services[SIP_ASA_TEMPORARY_SERVICE].c_ShortName = "TEMPORARY";
   hmc_Services[SIP_ASA_TEMPORARY_SERVICE].c_ASNs.SetLength(0);

   hmc_Services[SIP_ASA_SUPERVISOR].c_ShortName = "SUPERVISOR";
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs.SetLength(SIP_ASN_SSV_ERROR_REACTION + 1);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_INIT_VOLTAGE].Set("INIT_VOLTAGE", 0, "CH", SIP_PSIZE_SSV_INIT_VOLTAGE);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_INIT_VOLTAGE].c_Payload.SetLength(3);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_INIT_VOLTAGE].c_Payload[0].Set("VOLTAGE_TYPE", 1, false);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_INIT_VOLTAGE].c_Payload[1].Set("VOLTAGE_MAX", 4, false);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_INIT_VOLTAGE].c_Payload[2].Set("VOLTAGE_MIX", 4, false);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_STATUS].Set("STATUS", 0, "CH", SIP_PSIZE_SSV_STATUS);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_STATUS].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_STATUS].c_Payload[0].Set("STATUS", 4, false);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_TEST_TIME].Set("TEST_TIME", 0, "CH", 0);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_ERROR_REACTION].Set("ERROR_REACTION", 0, "CH", SIP_PSIZE_SSV_ERROR_REACTION);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_ERROR_REACTION].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_SUPERVISOR].c_ASNs[SIP_ASN_SSV_ERROR_REACTION].c_Payload[0].Set("REACTION", 1, false);

   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ShortName = "REAL_TIME_CLOCK";
   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ASNs.SetLength(SIP_ASN_RTC_TIME_DATE + 1);
   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ASNs[SIP_ASN_RTC_TIME_DATE].Set("TIME_DATE", 0, "CH", SIP_PSIZE_RTC_TIME_DATE);
   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ASNs[SIP_ASN_RTC_TIME_DATE].c_Payload.SetLength(6);
   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ASNs[SIP_ASN_RTC_TIME_DATE].c_Payload[0].Set("SECOND", 1, false);
   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ASNs[SIP_ASN_RTC_TIME_DATE].c_Payload[1].Set("MINUTE", 1, false);
   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ASNs[SIP_ASN_RTC_TIME_DATE].c_Payload[2].Set("HOUR", 1, false);
   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ASNs[SIP_ASN_RTC_TIME_DATE].c_Payload[3].Set("DAY", 1, false);
   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ASNs[SIP_ASN_RTC_TIME_DATE].c_Payload[4].Set("MONTH", 1, false);
   hmc_Services[SIP_ASA_REAL_TIME_CLOCK].c_ASNs[SIP_ASN_RTC_TIME_DATE].c_Payload[5].Set("YEAR", 1, false);

   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ShortName = "BACKGROUND_FUNCTION";
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs.SetLength(SIP_ASN_BGF_PARAMETER + 1);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_START].Set("START", 0, "CH", SIP_PSIZE_BGF_START);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_START].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_START].c_Payload[0].Set("PARAMETER", 12, false);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_STOP].Set("STOP", 0, "CH", SIP_PSIZE_BGF_STOP);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_STOP].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_STOP].c_Payload[0].Set("STATUS", 1, false);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_STOP].c_Payload[1].Set("RESULT", 31, false);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_STATUS].Set("STATUS", 0, "CH", SIP_PSIZE_BGF_STATUS);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_STATUS].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_STATUS].c_Payload[0].Set("DATA", 4, false);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_SETUP].Set("SETUP", SIP_ASP_BGF_SETUP_COUNT + 1);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_SETUP].c_ASPs[SIP_ASP_BGF_SETUP_COUNT].Set("COUNT", SIP_PSIZE_BGF_SETUP_COUNT);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_SETUP].c_ASPs[SIP_ASP_BGF_SETUP_COUNT].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_SETUP].c_ASPs[SIP_ASP_BGF_SETUP_COUNT].c_Payload[0].Set("COUNT", 1, false);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_NAME].Set("NAME", 0, "CH", SIP_PSIZE_BGF_NAME);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_NAME].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_NAME].c_Payload[0].Set("NAME_OF_FUNCTION", 22, false);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_PARAMETER].Set("PARAMETER", 0, "CH", SIP_PSIZE_BGF_PARAMETER);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_PARAMETER].c_Payload.SetLength(1);
   hmc_Services[SIP_ASA_BACKGROUND_FUNCTION].c_ASNs[SIP_ASN_BGF_PARAMETER].c_Payload[0].Set("PARAMETERS", 92, false);

   hmc_Services[SIP_ASA_ETHERNET].c_ShortName = "ETHERNET";
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs.SetLength(SIP_ASN_ETH_STATE + 1);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_INIT].Set("INIT", 0, "CH", SIP_PSIZE_ETH_INIT);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_INIT].c_Payload.SetLength(2);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_INIT].c_Payload[0].Set("SPEED", 1, false);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_INIT].c_Payload[1].Set("DUPLEX", 1, false);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_CLOSE].Set("CLOSE", 0, "CH", 0);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_STATE].Set("STATE", 0, "CH", SIP_PSIZE_ETH_STATE);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_STATE].c_Payload.SetLength(4);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_STATE].c_Payload[0].Set("LINK_STATE", 1, false);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_STATE].c_Payload[1].Set("SPEED", 1, false);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_STATE].c_Payload[2].Set("DUPLEX", 1, false);
   hmc_Services[SIP_ASA_ETHERNET].c_ASNs[SIP_ASN_ETH_STATE].c_Payload[3].Set("POLARITY", 1, false);
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIPDecodeData(const SCLDynamicArray<C_CMONProtocolSIPPayload> & orc_Payload,
                                                         const uint8 ou8_NumBytesInPayload,
                                                         const uint8 * const opu8_Payload) const
{
   //decode opu8_Payload
   uint8 u8_Payload;
   uint16 u16_Offset = 0U;
   C_SCLString c_Text;

   for (u8_Payload = 0U; u8_Payload < orc_Payload.GetLength(); u8_Payload++)
   {
      //do we have enough data ?
      if ((u16_Offset + orc_Payload[u8_Payload].u8_Size) <= ou8_NumBytesInPayload)
      {
         c_Text += orc_Payload[u8_Payload].Decode(&opu8_Payload[u16_Offset], mq_Decimal);
         u16_Offset += orc_Payload[u8_Payload].u8_Size;
      }
      else
      {
         break;
      }
   }
   //any leftovers ?
   if (u16_Offset < ou8_NumBytesInPayload)
   {
      //add as "DATA":
      uint8 u8_Index;
      c_Text += "  DATA [";
      for (u8_Index = static_cast<uint8>(u16_Offset); u8_Index < ou8_NumBytesInPayload; u8_Index++)
      {
        c_Text += (m_GetByteAsStringFormat(opu8_Payload[u8_Index]));
        if (u8_Index != (ou8_NumBytesInPayload - 1))
        {
           c_Text += ",";
        }
      }
      c_Text += "]";
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
//\param[in]  ou8_ASA                  service header area
//\param[in]  ou8_ASN                  service header name
//\param[in]  ou8_ASP                  service header parameter
//\param[out] oppc_Payload             address of pointer to payload description
C_SCLString C_CMONProtocolSHIP_IP_IVA::m_SIPDecodeSFFFReadWriteHeader(const uint8 ou8_ASA, const uint8 ou8_ASN,
                                                                      const uint8 ou8_ASP,
                                                               SCLDynamicArray<C_CMONProtocolSIPPayload> ** const oppc_Payload
                                                               ) const
{
   C_SCLString c_Text;
   static SCLDynamicArray<C_CMONProtocolSIPPayload> hc_Dummy; //dummy with no entries
   (*oppc_Payload) = &hc_Dummy;
   if (ou8_ASA < hmc_Services.GetLength())
   {
      //we know the ASA !
      c_Text = " " + hmc_Services[ou8_ASA].c_ShortName + ".";
      if (ou8_ASN < hmc_Services[ou8_ASA].c_ASNs.GetLength())
      {
         //we even know the ASN !
         c_Text += (hmc_Services[ou8_ASA].c_ASNs[ou8_ASN].c_ShortName + ".");
         //are there ASPs ?
         if (hmc_Services[ou8_ASA].c_ASNs[ou8_ASN].c_ASPs.GetLength() == 0)
         {
            //no, just channel numbers:
            c_Text += (hmc_Services[ou8_ASA].c_ASNs[ou8_ASN].c_ASPPrefix + m_GetByteAsStringFormat(ou8_ASP));
            (*oppc_Payload) = &hmc_Services[ou8_ASA].c_ASNs[ou8_ASN].c_Payload;
         }
         else
         {
            //does the ASP exist ?
            if (ou8_ASP < hmc_Services[ou8_ASA].c_ASNs[ou8_ASN].c_ASPs.GetLength())
            {
               c_Text += hmc_Services[ou8_ASA].c_ASNs[ou8_ASN].c_ASPs[ou8_ASP].c_ShortName;
               (*oppc_Payload) = &hmc_Services[ou8_ASA].c_ASNs[ou8_ASN].c_ASPs[ou8_ASP].c_Payload;
            }
            else
            {
               //just print the value:
               c_Text += ("SP" + m_GetByteAsStringFormat(ou8_ASP));
            }
         }

      }
      else
      {
         //just print the value:
         c_Text += ("SN" + m_GetByteAsStringFormat(ou8_ASN) + ".SP" + m_GetByteAsStringFormat(ou8_ASP));
      }
   }
   else
   {
      //bohemian villages to us ...
      c_Text += (" SA" + m_GetByteAsStringFormat(ou8_ASA) +
                 ".SN" + m_GetByteAsStringFormat(ou8_ASN) +
                 ".SP" + m_GetByteAsStringFormat(ou8_ASP));
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------

