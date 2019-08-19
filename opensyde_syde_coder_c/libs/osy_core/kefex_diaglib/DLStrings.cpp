//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib string table

   DiagLib string table containing English language strings.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "stwtypes.h"

#define STR_TABLE_INCLUDE

#include "CSCLResourceStrings.h"
#include "DLLocalize.h"  //string indexes
#include "DLStrings.h" //strings

//----------------------------------------------------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_scl;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

//resource string table
const C_SCLResourceString gac_DIAG_LIB_RESOURCE_STRINGS[] =
{
  //Common texts
   { STR_ERROR                            , STR_TEXT_ERROR },
   { STR_ERROR_EXCL                       , STR_TEXT_ERROR_EXCL },
   { STR_INFO                             , STR_TEXT_INFO },
   { STR_WARNING                          , STR_TEXT_WARNING },
   { STR_CONFIRMATION                     , STR_TEXT_CONFIRMATION },
   { STR_ERR_NO_NUMBER                    , STR_TEXT_ERR_NO_NUMBER },
   { STR_FILENAME                         , STR_TEXT_FILENAME },
   { STR_USE_EXISTING                     , STR_TEXT_USE_EXISTING },
   { STR_VALUE                            , STR_TEXT_VALUE },
   { STR_FILE                             , STR_TEXT_FILE },
   { STR_FILESIZE                         , STR_TEXT_FILESIZE },
   { STR_TIMESTAMP                        , STR_TEXT_TIMESTAMP },
   { STR_ERR_FILE_WRITE                   , STR_TEXT_ERR_FILE_WRITE },
   { STR_ERR_BITRATE_NOT_INT              , STR_TEXT_ERR_BITRATE_NOT_INT },
   { STR_ERR_PROT_WRITE                   , STR_TEXT_ERR_PROT_WRITE },
   { STR_ERR_DBS_WRONG                    , STR_TEXT_ERR_DBS_WRONG },
   { STR_ERR_NOT_DECIMAL                  , STR_TEXT_ERR_NOT_DECIMAL },

  //CKFXTraceFile
   { STR_TF_ERR_WRITING                   , STR_TEXT_TF_ERR_WRITING },
   { STR_TF_ERR_CREATE                    , STR_TEXT_TF_ERR_CREATE },
   { STR_TF_ERR_APPEND                    , STR_TEXT_TF_ERR_APPEND },

  //CPropEditDSList/DSVar
   { STR_DS_ERR_NO_DATA_SOURCE            , STR_TEXT_DS_ERR_NO_DATA_SOURCE },
   { STR_DS_ERR_NO_VALID_LISTS            , STR_TEXT_DS_ERR_NO_VALID_LISTS },

  //CPropEditFileName
   { STR_FN_FILTER_ALL_FILES              , STR_TEXT_FN_FILTER_ALL_FILES },

  //DlgCMONMain
   { STR_CM_TIME_ABSOLUTE                 , STR_TEXT_CM_TIME_ABSOLUTE },
   { STR_CM_TIME_RELATIVE                 , STR_TEXT_CM_TIME_RELATIVE },
   { STR_CM_BASE_HEX                      , STR_TEXT_CM_BASE_HEX },
   { STR_CM_BASE_DEC                      , STR_TEXT_CM_BASE_DEC },
   { STR_CM_MODE_STATIC                   , STR_TEXT_CM_MODE_STATIC },
   { STR_CM_MODE_CONT                     , STR_TEXT_CM_MODE_CONT },
   { STR_CM_PROT_OFF                      , STR_TEXT_CM_PROT_OFF },
   { STR_CM_PROT_ON                       , STR_TEXT_CM_PROT_ON },
   { STR_CM_L7_NONE                       , STR_TEXT_CM_L7_NONE },
   { STR_CM_L7_CAN_OPEN                   , STR_TEXT_CM_L7_CAN_OPEN },
   { STR_CM_L7_KEFEX                      , STR_TEXT_CM_L7_KEFEX },
   { STR_CM_L7_PPP                        , STR_TEXT_CM_L7_PPP },
   { STR_CM_ERR_GET_OS_VER                , STR_TEXT_CM_ERR_GET_OS_VER },
   { STR_CM_ERR_OS_VER                    , STR_TEXT_CM_ERR_OS_VER },
   { STR_CM_ERR_L7_PROT_INVALID           , STR_TEXT_CM_ERR_L7_PROT_INVALID },
   { STR_CM_ERR_NO_DONGLE                 , STR_TEXT_CM_ERR_NO_DONGLE },
   { STR_CM_ERR_OPEN_DLL                  , STR_TEXT_CM_ERR_OPEN_DLL },
   { STR_CM_ERR_CLOSE_DLL                 , STR_TEXT_CM_ERR_CLOSE_DLL },
   { STR_CM_ERR_INIT_CAN                  , STR_TEXT_CM_ERR_INIT_CAN },
   { STR_CM_ERR_CAN_STATUS                , STR_TEXT_CM_ERR_CAN_STATUS },
   { STR_CM_CAPTION                       , STR_TEXT_CM_CAPTION },
//   { STR_CM_CAPTION_PROT                  , STR_TEXT_CM_CAPTION_PROT },
   { STR_CM_ERR_PROT_CREATE               , STR_TEXT_CM_ERR_PROT_CREATE },
   { STR_CM_TX_ERRORS                     , STR_TEXT_CM_TX_ERRORS },
   { STR_CM_CAPTION_KFX_PROT              , STR_TEXT_CM_CAPTION_KFX_PROT },
   { STR_CM_PROMPT_KFX_PROT               , STR_TEXT_CM_PROMPT_KFX_PROT },
   { STR_CM_DLL_INFO_MANUF                , STR_TEXT_CM_DLL_INFO_MANUF },
   { STR_CM_DLL_INFO_DEVICE               , STR_TEXT_CM_DLL_INFO_DEVICE },
   { STR_CM_DLL_INFO_HW_VER               , STR_TEXT_CM_DLL_INFO_HW_VER },
   { STR_CM_DLL_INFO_DLL_VER              , STR_TEXT_CM_DLL_INFO_DLL_VER },
   { STR_CM_DLL_INFO_XTD                  , STR_TEXT_CM_DLL_INFO_XTD },
   { STR_CM_DLL_INFO_NO_XTD               , STR_TEXT_CM_DLL_INFO_NO_XTD },
   { STR_CM_CAPTION_ABOUT                 , STR_TEXT_CM_CAPTION_ABOUT },

  //DlgCMONSend
   { STR_CS_ERR_ID_TOO_GREAT              , STR_TEXT_CS_ERR_ID_TOO_GREAT },
   { STR_CS_ERR_DLC_TOO_GREAT             , STR_TEXT_CS_ERR_DLC_TOO_GREAT },
   { STR_CS_ERR_DBS_TOO_GREAT             , STR_TEXT_CS_ERR_DBS_TOO_GREAT },
   { STR_CS_ERR_MAX_NUM_MSGS              , STR_TEXT_CS_ERR_MAX_NUM_MSGS },
   { STR_CS_ERR_FILE_READ                 , STR_TEXT_CS_ERR_FILE_READ },
   { STR_CS_ERR_NO_FOCUS                  , STR_TEXT_CS_ERR_NO_FOCUS },
   { STR_CS_ERR_ID_WRONG                  , STR_TEXT_CS_ERR_ID_WRONG },
   { STR_CS_ERR_DLC_WRONG                 , STR_TEXT_CS_ERR_DLC_WRONG },
   { STR_CS_ERR_DBS_WRONG                 , STR_TEXT_CS_ERR_DBS_WRONG },
   { STR_CS_ERR_INTERVAL_WRONG            , STR_TEXT_CS_ERR_INTERVAL_WRONG },
   { STR_CS_ERR_OFFSET_WRONG              , STR_TEXT_CS_ERR_OFFSET_WRONG },
   { STR_CS_ERR_KEY_WRONG                 , STR_TEXT_CS_ERR_KEY_WRONG },

  //DlgKFXComSetup
   { STR_CS_ERR_VALUES_NOT_NUMERIC        , STR_TEXT_CS_ERR_VALUES_NOT_NUMERIC },
   { STR_CS_ERR_INVALID_BASE_ID           , STR_TEXT_CS_ERR_INVALID_BASE_ID },
   { STR_CS_ERR_VALUE_RANGE               , STR_TEXT_CS_ERR_VALUE_RANGE },

  //DlgKFXEditPasswords
   { STR_EP_ERR_NO_MATCH                  , STR_TEXT_EP_ERR_NO_MATCH },

  //DlgKFXIndexedInputListEditor
   { STR_IILE_NO_INTEGER                  , STR_TEXT_IILE_NO_INTEGER },

  //DlgKFXIndexedListEditor
   { STR_ILE_VALUE_RANGE                  , STR_TEXT_ILE_VALUE_RANGE },
   { STR_ILE_VALUE_WRONG                  , STR_TEXT_ILE_VALUE_WRONG },

  //DlgKFXIndexedStringEditor
   { STR_ISE_STRING                       , STR_TEXT_ISE_STRING },
   { STR_ISE_VALUE_WRONG                  , STR_TEXT_ISE_VALUE_WRONG },

  //DlgRAMEditChart
   { STR_REC_LEGEND                       , STR_TEXT_REC_LEGEND },
   { STR_REC_BUFFER_OVERRUN               , STR_TEXT_REC_BUFFER_OVERRUN },
   { STR_REC_MEASUREMENT_STOPPED          , STR_TEXT_REC_MEASUREMENT_STOPPED },
   { STR_REC_SERIES                       , STR_TEXT_REC_SERIES },
   { STR_REC_ERR_BUFFER_CREATE            , STR_TEXT_REC_ERR_BUFFER_CREATE },
   { STR_REC_ERR_BUFFER_WRITE             , STR_TEXT_REC_ERR_BUFFER_WRITE },
   { STR_REC_ERR_BUFFER_READ              , STR_TEXT_REC_ERR_BUFFER_READ },
   { STR_REC_ERR_NO_VALUES                , STR_TEXT_REC_ERR_NO_VALUES },
   { STR_REC_ERR_BUFFER_FILE              , STR_TEXT_REC_ERR_BUFFER_FILE },
   { STR_REC_ERR_READ_ONLY                , STR_TEXT_REC_ERR_READ_ONLY },
   { STR_REC_STATUS_READING               , STR_TEXT_REC_STATUS_READING },
   { STR_REC_STATUS_UPDATING              , STR_TEXT_REC_STATUS_UPDATING },
   { STR_REC_MAX_NUM_POINTS               , STR_TEXT_REC_MAX_NUM_POINTS },
   { STR_REC_WRITTEN                      , STR_TEXT_REC_WRITTEN },
   { STR_REC_VARIABLE                     , STR_TEXT_REC_VARIABLE },
   { STR_REC_DEVICE                       , STR_TEXT_REC_DEVICE },
   { STR_REC_DATE                         , STR_TEXT_REC_DATE },
   { STR_REC_SENT_TO_PRINTER              , STR_TEXT_REC_SENT_TO_PRINTER },
   { STR_REC_COPIED_TO_CLIP               , STR_TEXT_REC_COPIED_TO_CLIP },
   { STR_REC_STOP_TRACE                   , STR_TEXT_REC_STOP_TRACE },

  //DlgXFLBBBStream
   { STR_FB_STW_PASSWD_OK                 , STR_TEXT_FB_STW_PASSWD_OK },
   { STR_FB_CUST_PASSWD_OK                , STR_TEXT_FB_CUST_PASSWD_OK },
   { STR_FB_THANKS                        , STR_TEXT_FB_THANKS },
   { STR_FB_WRONG_PASSWD                  , STR_TEXT_FB_WRONG_PASSWD },

  //DlgXFLMain
  //Statusmemo messages
   { STR_FM_ERR_COMP_ID_LENGTH            , STR_TEXT_FM_ERR_COMP_ID_LENGTH },
   { STR_FM_ERR_WAKEUP_FAILED             , STR_TEXT_FM_ERR_WAKEUP_FAILED },
//  STR_FM_ERR_SNR_FORMAT     "Error: SNR must be in the format: 00.112233.4455 !"
   { STR_FM_ERR_NODE_NOT_FOUND            , STR_TEXT_FM_ERR_NODE_NOT_FOUND },
   { STR_FM_ERR_DLL                       , STR_TEXT_FM_ERR_DLL },
   { STR_FM_ERR_OPEN_CAN                  , STR_TEXT_FM_ERR_OPEN_CAN },
//  STR_FM_ERR_DLL_NOT_FOUND  "Error: Communication driver DLL not found"
   { STR_FM_ERR_LOCALID                   , STR_TEXT_FM_ERR_LOCALID },
   { STR_FM_ERR_COMPANYID                 , STR_TEXT_FM_ERR_COMPANYID },
   { STR_FM_ERR_SNR                       , STR_TEXT_FM_ERR_SNR },
   { STR_FM_ERR_WAKEUP                    , STR_TEXT_FM_ERR_WAKEUP },
//  STR_FM_ERR_DIVERT_STREAM_OFF  "Error: Divert Stream OFF !"
   { STR_FM_ERR_DIVERT_STREAM             , STR_TEXT_FM_ERR_DIVERT_STREAM },
   { STR_FM_ERR_WAKEUP_BBB                , STR_TEXT_FM_ERR_WAKEUP_BBB },
//  STR_FM_ERR_HF_UNDEFINED   "Error: Undefined error in Hex file !"
//  STR_FM_ERR_HF_LINE_TOO_LONG "Error: Hex line too long !"
//  STR_FM_ERR_READ_NO_RESPONSE "Error during reading: No response from node"
//  STR_FM_ERR_READ_INVALID_ADDRESS "Error during reading: Invalid address"
//  STR_FM_ERR_READ_CHECKSUM  "Error during reading: Checksum error"
//  STR_FM_ERR_READ_UNDEF     "Error during reading: Undefined error"
   { STR_FM_ERR_VERIFY_ERR                , STR_TEXT_FM_ERR_VERIFY_ERR },
   { STR_FM_ERR_VERIFY_ABORT              , STR_TEXT_FM_ERR_VERIFY_ABORT },
//  STR_FM_ERR_VERIFY_UNEXP   "Error: Unexpected Hex file command !"
   { STR_FM_VERIFY_MATCH                  , STR_TEXT_FM_VERIFY_MATCH },
   { STR_FM_VERIFY_TOTAL_DIFFERENCES      , STR_TEXT_FM_VERIFY_TOTAL_DIFFERENCES },
   { STR_FM_RESETTING                     , STR_TEXT_FM_RESETTING },
   { STR_FM_TURN_ON                       , STR_TEXT_FM_TURN_ON },
   { STR_FM_RESETTING_S                   , STR_TEXT_FM_RESETTING_S },
   { STR_FM_TURN_ON_S                     , STR_TEXT_FM_TURN_ON_S },
   { STR_FM_CORRECT_CID                   , STR_TEXT_FM_CORRECT_CID },
   { STR_FM_NO_NODES_FOUND                , STR_TEXT_FM_NO_NODES_FOUND },
   { STR_FM_WAKEUP_SNR_OK                 , STR_TEXT_FM_WAKEUP_SNR_OK },
//  STR_FM_COMPARED_LINE      "Compared line"
   { STR_FM_DIVERT_STREAM_OFF_OK          , STR_TEXT_FM_DIVERT_STREAM_OFF_OK },
   { STR_FM_DIVERT_STREAM_OK              , STR_TEXT_FM_DIVERT_STREAM_OK },
   { STR_FM_WAKEUP_BBB                    , STR_TEXT_FM_WAKEUP_BBB },
   { STR_FM_WAKEUP_BBB_OK                 , STR_TEXT_FM_WAKEUP_BBB_OK },
   { STR_FM_DIVERT_STREAM_BBB             , STR_TEXT_FM_DIVERT_STREAM_BBB },
//  STR_FM_PREPARING_DL       "Preparing download ..."
   { STR_FM_WRITING                       , STR_TEXT_FM_WRITING },
   { STR_FM_LINE                          , STR_TEXT_FM_LINE },
   { STR_FM_FINISHED                      , STR_TEXT_FM_FINISHED },
   
   { STR_FM_CONNECTED_NODE                , STR_TEXT_FM_CONNECTED_NODE             },
   { STR_FM_FLASHING_FINISHED             , STR_TEXT_FM_FLASHING_FINISHED          },
   { STR_FM_ERR_DISPATCHER_NA             , STR_TEXT_FM_ERR_DISPATCHER_NA          },
   { STR_FM_ERR_VERSION_MISMATCH          , STR_TEXT_FM_ERR_VERSION_MISMATCH       },
   { STR_FM_ERR_XFL_EXCHANGE              , STR_TEXT_FM_ERR_XFL_EXCHANGE           },
   { STR_FM_XFL_EXCHANGE                  , STR_TEXT_FM_XFL_EXCHANGE               },
   { STR_FM_ERR_FL_PROTOCOL_VERSION       , STR_TEXT_FM_ERR_FL_PROTOCOL_VERSION    },
   { STR_FM_FL_PROTOCOL_VERSION           , STR_TEXT_FM_FL_PROTOCOL_VERSION        },
   { STR_FM_ERR_READ_IMPL_INFO            , STR_TEXT_FM_ERR_READ_IMPL_INFO         },
   { STR_FM_ERR_READ_FINGER_PRINT         , STR_TEXT_FM_ERR_READ_FINGER_PRINT      },
   { STR_FM_ERR_FINGER_PRINT_NA           , STR_TEXT_FM_ERR_FINGER_PRINT_NA        },
   { STR_FM_ERR_USER_ABORT                , STR_TEXT_FM_ERR_USER_ABORT             },
   { STR_FM_ERR_RD_SNR_CDN_WAKE           , STR_TEXT_FM_ERR_RD_SNR_CDN_WAKE        },
   { STR_FM_TXT_SET_PARA_PT1              , STR_TEXT_FM_TXT_SET_PARA_PT1           },
   { STR_FM_TXT_SET_PARA_PT2              , STR_TEXT_FM_TXT_SET_PARA_PT2           },
   { STR_FM_ERR_SET_DIVERT_STREAM_PARA    , STR_TEXT_FM_ERR_SET_DIVERT_STREAM_PARA },
   { STR_FM_ERR_RD_NUM_FLASH_IC           , STR_TEXT_FM_ERR_RD_NUM_FLASH_IC        },
   
  //message boxes
   { STR_FM_COMM_RS232_WARN               , STR_TEXT_FM_COMM_RS232_WARN },
   { STR_FM_ERR_INVALID_SNR               , STR_TEXT_FM_ERR_INVALID_SNR },
//  STR_FM_ERR_QUEUE_FULL         "CAN Hardware Error: Could not send message (possibly wrong baudrate) !!"
//  STR_FM_ERR_TOO_MANY_NODES     "Too many nodes with same ID !!"
   { STR_FM_ERR_NO_RESPONSE               , STR_TEXT_FM_ERR_NO_RESPONSE },
//  STR_FM_ERR_WRONG_CID          "Wrong company ID"
//  STR_FM_ERR_READ_SNR           "Unable to read SNR !!"
   { STR_FM_ERR_MULTI_ID                  , STR_TEXT_FM_ERR_MULTI_ID },
   { STR_FM_ERR_CREATE_FILE               , STR_TEXT_FM_ERR_CREATE_FILE },
   { STR_FM_ERR_SEND_ID_NOT_NUMBER        , STR_TEXT_FM_ERR_SEND_ID_NOT_NUMBER },
   { STR_FM_ERR_HEX_FILE_NO_EXIST         , STR_TEXT_FM_ERR_HEX_FILE_NO_EXIST },
   { STR_FM_ERR_RD_HF_EOF                 , STR_TEXT_FM_ERR_RD_HF_EOF },
   { STR_FM_ERR_RD_HF_LINE_SYNTAX         , STR_TEXT_FM_ERR_RD_HF_LINE_SYNTAX },
   { STR_FM_ERR_RD_HF_LINE_CHECKSUM       , STR_TEXT_FM_ERR_RD_HF_LINE_CHECKSUM },
   { STR_FM_ERR_RD_HF_LINE_COMMAND        , STR_TEXT_FM_ERR_RD_HF_LINE_COMMAND },
   { STR_FM_ERR_RD_HF_MEMORY              , STR_TEXT_FM_ERR_RD_HF_MEMORY },
   { STR_FM_ERR_RD_HF_OPEN_FILE           , STR_TEXT_FM_ERR_RD_HF_OPEN_FILE },
   { STR_FM_ERR_RD_HF_UNDEFINED           , STR_TEXT_FM_ERR_RD_HF_UNDEFINED },
   { STR_FM_NUM_BYTES_IN_FILE             , STR_TEXT_FM_NUM_BYTES_IN_FILE },
   { STR_FM_FILE_FILTERED                 , STR_TEXT_FM_FILE_FILTERED },
   { STR_FM_SAVE_CONFIGURATION            , STR_TEXT_FM_SAVE_CONFIGURATION },
   { STR_FM_ENTER_NAME                    , STR_TEXT_FM_ENTER_NAME },
   
   { STR_FM_ERR_RD_PSEC_INFO              , STR_TEXT_FM_ERR_RD_PSEC_INFO         },
   { STR_FM_ERR_RD_MEM_SIZE_IC            , STR_TEXT_FM_ERR_RD_MEM_SIZE_IC       },
   { STR_FM_ERR_RD_SEC_OFFSET_IC          , STR_TEXT_FM_ERR_RD_SEC_OFFSET_IC     },
   { STR_FM_ERR_RD_MAX_ERASE              , STR_TEXT_FM_ERR_RD_MAX_ERASE         },
   { STR_FM_ERR_RD_MAX_PRG_TIME           , STR_TEXT_FM_ERR_RD_MAX_PRG_TIME      },
   { STR_FM_ERR_RD_NUM_REGIONS            , STR_TEXT_FM_ERR_RD_NUM_REGIONS       },
   { STR_FM_ERR_RD_REGION_INFO_IC         , STR_TEXT_FM_ERR_RD_REGION_INFO_IC    },
   { STR_FM_ERR_TRANSMISSION_ERR          , STR_TEXT_FM_ERR_TRANSMISSION_ERR     },
   { STR_FM_ERR_DURING_FLASH_PROG         , STR_TEXT_FM_ERR_DURING_FLASH_PROG    },
   { STR_FM_ERR_DURING_DEL                , STR_TEXT_FM_ERR_DURING_DEL           },
   { STR_FM_ERR_BITRATE                   , STR_TEXT_FM_ERR_BITRATE              },
   { STR_FM_ERR_INVALID_PARA              , STR_TEXT_FM_ERR_INVALID_PARA         },
   { STR_FM_ERR_EEP_VERIFY                , STR_TEXT_FM_ERR_EEP_VERIFY           },
   { STR_FM_ERR_EEP_READ                  , STR_TEXT_FM_ERR_EEP_READ             },
   { STR_FM_ERR_DIVERT_STREAM_TRG_GW      , STR_TEXT_FM_ERR_DIVERT_STREAM_TRG_GW },
   { STR_FM_ERR_HEX_CMD_UNK               , STR_TEXT_FM_ERR_HEX_CMD_UNK          },
   { STR_FM_TXT_NUM_NODES                 , STR_TEXT_FM_TXT_NUM_NODES            },
   { STR_FM_TXT_NODE_WAKE_PT1             , STR_TEXT_FM_TXT_NODE_WAKE_PT1        },
   { STR_FM_TXT_NODE_WAKE_PT2             , STR_TEXT_FM_TXT_NODE_WAKE_PT2        },
   { STR_FM_TXT_NODE_SNR_PT1              , STR_TEXT_FM_TXT_NODE_SNR_PT1         },
   { STR_FM_TXT_NODE_SNR_PT2              , STR_TEXT_FM_TXT_NODE_SNR_PT2         },
   { STR_FM_TXT_SURP_NODE_AMBIG_ID        , STR_TEXT_FM_TXT_SURP_NODE_AMBIG_ID   },
   { STR_FM_ERR_DEV_ID_TYPE               , STR_TEXT_FM_ERR_DEV_ID_TYPE          },
   { STR_FM_ERR_WAKE_SNR_PT1              , STR_TEXT_FM_ERR_WAKE_SNR_PT1         },
   { STR_FM_ERR_WAKE_SNR_PT2              , STR_TEXT_FM_ERR_WAKE_SNR_PT2         },
   { STR_FM_ERR_WAKE_SNR_PT3              , STR_TEXT_FM_ERR_WAKE_SNR_PT3         },
   { STR_FM_ERR_RE_LOI                    , STR_TEXT_FM_ERR_RE_LOI               },
   { STR_FM_ERR_RE_PROG_DATE              , STR_TEXT_FM_ERR_RE_PROG_DATE         },
   { STR_FM_ERR_RE_PROG_TIME              , STR_TEXT_FM_ERR_RE_PROG_TIME         },
   { STR_FM_ERR_RE_USER_NAME              , STR_TEXT_FM_ERR_RE_USER_NAME         },
   { STR_FM_ERR_RE_CHECKSUM               , STR_TEXT_FM_ERR_RE_CHECKSUM          },
   { STR_FM_ERR_RE_DEV_INFO_ADDR          , STR_TEXT_FM_ERR_RE_DEV_INFO_ADDR     },
   { STR_FM_ERR_IMPL_SERVICES             , STR_TEXT_FM_ERR_IMPL_SERVICES        },
   { STR_FM_ERR_RD_HEX_REC_SUP_INF        , STR_TEXT_FM_ERR_RD_HEX_REC_SUP_INF   },
   { STR_FM_ERR_RD_CALC_CHKSM_BLK         , STR_TEXT_FM_ERR_RD_CALC_CHKSM_BLK    },
   { STR_FM_ERR_WR_RESET_RQ               , STR_TEXT_FM_ERR_WR_RESET_RQ          },
   
  //other
//  STR_FM_FOR_INTERNAL_USE      " for STW internal use only ["
   { STR_FM_ERR_ONE_FILE_ONLY             , STR_TEXT_FM_ERR_ONE_FILE_ONLY },
   { STR_FM_ERR_NO_HEXFILE                , STR_TEXT_FM_ERR_NO_HEXFILE },
   { STR_FM_ERR_INI_WRITE                 , STR_TEXT_FM_ERR_INI_WRITE },
//  STR_FB_WAKEUP_NODE          "Wakeup Node"
//  STR_FB_FLASH_NODE           "Flash Node"
//  STR_FB_SEARCH_ALL_NODES     "Search All Nodes"

  // DlgXFLSearchNodeSnr
//  STR_DFM_SNS_CAPTION        "Results ..."
//  STR_DFM_SNS_LOCID_SNR      "Flash/Local ID/Serial_number"
//  STR_DFM_SNS_FLASH          "Flash selected nodes"
//  STR_DFM_SNS_CHANGE_ID      "Change Local ID"
//  STR_DFM_SNS_SH_NO_ECU_SELECTED "No server selected ! Select a server with click on Checklist !"
//  STR_DFM_SNS_SELECT_ECU         "Select one of the listed servers by cursor click !"

  //DlgXFLSearch
   { STR_FS_ERR_LID_NOT_NUMERIC           , STR_TEXT_FS_ERR_LID_NOT_NUMERIC },
   { STR_FS_ERR_LID_WRONG                 , STR_TEXT_FS_ERR_LID_WRONG },
   { STR_FS_ERR_NO_WAKEUP                 , STR_TEXT_FS_ERR_NO_WAKEUP },
   { STR_FS_ERR_WRONG_CID                 , STR_TEXT_FS_ERR_WRONG_CID },
   { STR_FS_ERR_LID_NOCHANGE              , STR_TEXT_FS_ERR_LID_NOCHANGE },
   { STR_FS_MODE_THAN_ONE_NODE            , STR_TEXT_FS_MODE_THAN_ONE_NODE },
   { STR_FS_INPUT_NEW_LID                 , STR_TEXT_FS_INPUT_NEW_LID },
   { STR_FS_LOCAL_ID                      , STR_TEXT_FS_LOCAL_ID },
   { STR_FS_LOCAL_ID_CHANGED              , STR_TEXT_FS_LOCAL_ID_CHANGED },

  //DlgXFLWakeup
   { STR_FW_ERR_READ_SEC_COUNT            , STR_TEXT_FW_ERR_READ_SEC_COUNT },
   { STR_FW_ERR_READ_DEV_ID               , STR_TEXT_FW_ERR_READ_DEV_ID },
   { STR_FW_ERR_READ_ERASE_TO             , STR_TEXT_FW_ERR_READ_ERASE_TO },
   { STR_FW_ERR_READ_WRITE_TO             , STR_TEXT_FW_ERR_READ_WRITE_TO },
   { STR_FW_ERR_READ_DL_COUNT             , STR_TEXT_FW_ERR_READ_DL_COUNT },
   { STR_FW_ERR_READ_XFL_VER              , STR_TEXT_FW_ERR_READ_XFL_VER },
   { STR_FW_ERR_READ_LID                  , STR_TEXT_FW_ERR_READ_LID },
   { STR_FW_ERR_READ_SNR                  , STR_TEXT_FW_ERR_READ_SNR },
   { STR_FW_ERR_READ_BBB_SEC_COUNT        , STR_TEXT_FW_ERR_READ_BBB_SEC_COUNT },
   { STR_FW_ERR_READ_BBB_XFL_VER          , STR_TEXT_FW_ERR_READ_BBB_XFL_VER },
   { STR_FW_ERR_READ_BBB_LAST_USER        , STR_TEXT_FW_ERR_READ_BBB_LAST_USER },
   { STR_FW_ERR_DEV_ID_NOT_SUPP           , STR_TEXT_FW_ERR_DEV_ID_NOT_SUPP },
//   { STR_FW_ERR_DIVERT_NOT_SUPP           , STR_TEXT_FW_ERR_DIVERT_NOT_SUPP },
   { STR_FW_ERR_WAKEUP_BBB                , STR_TEXT_FW_ERR_WAKEUP_BBB },
   { STR_FW_ERR_DIVERT_FAILED             , STR_TEXT_FW_ERR_DIVERT_FAILED },
   { STR_FW_ERR_DIVERT_OFF_FAILED         , STR_TEXT_FW_ERR_DIVERT_OFF_FAILED },
   { STR_FW_ERR_LID_NOCHANGE              , STR_TEXT_FW_ERR_LID_NOCHANGE },
   { STR_FW_ERR_SEND_TO_SLEEP             , STR_TEXT_FW_ERR_SEND_TO_SLEEP },
   { STR_FW_ERR_NO_RESPONSE               , STR_TEXT_FW_ERR_NO_RESPONSE },
   { STR_FW_ERR_NO_SEC_SELECTED           , STR_TEXT_FW_ERR_NO_SEC_SELECTED },
   { STR_FW_ERR_SEC_COUNT_IVALID          , STR_TEXT_FW_ERR_SEC_COUNT_IVALID },
   { STR_FW_ERR_READ_CRC                  , STR_TEXT_FW_ERR_READ_CRC },
   { STR_FW_NUM_SECTORS                   , STR_TEXT_FW_NUM_SECTORS },
   { STR_FW_DEV_ID                        , STR_TEXT_FW_DEV_ID },
   { STR_FW_FLASHLOADER_VER               , STR_TEXT_FW_FLASHLOADER_VER },
   { STR_FW_DOWNLOAD_COUNT                , STR_TEXT_FW_DOWNLOAD_COUNT },
   { STR_FW_SNR                           , STR_TEXT_FW_SNR },
   { STR_FW_BBB_LAST_STW                  , STR_TEXT_FW_BBB_LAST_STW },
   { STR_FW_BBB_LAST_CUST                 , STR_TEXT_FW_BBB_LAST_CUST },
   { STR_FW_BBB_LAST_UNDEF                , STR_TEXT_FW_BBB_LAST_UNDEF },
   { STR_FW_BBB_LAST_XXX                  , STR_TEXT_FW_BBB_LAST_XXX },
   { STR_FW_LOCAL_ID_CHANGED              , STR_TEXT_FW_LOCAL_ID_CHANGED },
   { STR_FW_BITRATE_CHANGED               , STR_TEXT_FW_BITRATE_CHANGED },
   { STR_FW_SEND_ID_CHANGED               , STR_TEXT_FW_SEND_ID_CHANGED },
   { STR_FW_CRC_WRITTEN                   , STR_TEXT_FW_CRC_WRITTEN },
   { STR_FW_ERASE_TO_CHANGED              , STR_TEXT_FW_ERASE_TO_CHANGED },
   { STR_FW_WRITE_TO_CHANGED              , STR_TEXT_FW_WRITE_TO_CHANGED },
   { STR_FW_WARN_CLOSE                    , STR_TEXT_FW_WARN_CLOSE },
//   { STR_FW_CONFIRM_SET_COMP_MODE         , STR_TEXT_FW_CONFIRM_SET_COMP_MODE },
   { STR_FW_COMP_MODE_CHANGED             , STR_TEXT_FW_COMP_MODE_CHANGED },

  //XFL error responses
   { STR_XFL_ERR_NO_RESPONSE              , STR_TEXT_XFL_ERR_NO_RESPONSE },
//  STR_XFL_ERR_SET_BTR          "Bitrate could not be set !"
//  STR_XFL_ERR_SET_ID           "Send ID could not be set !"
//  STR_XFL_ERR_INVALID_BITRATE  "Invalid bitrate !!"
//  STR_XFL_ERR_INVALID_SECTOR   "Sector out of range !"
   { STR_XFL_ERR_INVALID_PARAMETER        , STR_TEXT_XFL_ERR_INVALID_PARAMETER },
   { STR_XFL_ERR_EEPROM_WRITE             , STR_TEXT_XFL_ERR_EEPROM_WRITE },
   { STR_XFL_ERR_UNDEFINED_CODE           , STR_TEXT_XFL_ERR_UNDEFINED_CODE },
   { STR_XFL_ERR_UNDEFINED_ERR            , STR_TEXT_XFL_ERR_UNDEFINED_ERR },
//  STR_XFL_ERR_WAKE_UP_SNR_LONG   "Error: wake up SNR not supported (LONG_SNR_MODE) !!"
//  STR_XFL_ERR_NUM_SELECTED_ECU   "Error: number of selected nodes must be 1 !"
//  STR_XFL_ERR_NO_INIT_PROGRAM_FLASH "Error: program flash components are not initialized !"
//  STR_XFL_ERR_SET_ONE_ECU_ACTIVE "Error: set one node ACTIVE and verify !"
//  STR_XFL_ERR_NODE_SLEEP         "Error: could not set node into sleep !"
//   { STR_XFL_ERR_READ_LID                 , STR_TEXT_XFL_ERR_READ_LID },
//  STR_XFL_ERR_READ_SNR           "Error: can not read Serial Number !"
   { STR_XFL_ERR_SNR_TO_CANDATA           , STR_TEXT_XFL_ERR_SNR_TO_CANDATA },
//  STR_XFL_ERR_NUM_ECU_BIG_0      "Error: number of found ECUs must be bigger than 0 !"

  //VisuComponents
   { STR_VIS_ERR_IWR                      , STR_TEXT_VIS_ERR_IWR },
   { STR_VIS_ERR_LOAD_BITMAP              , STR_TEXT_VIS_ERR_LOAD_BITMAP },
   { STR_VIS_ERR_LOAD_WAV                 , STR_TEXT_VIS_ERR_LOAD_WAV },
   { STR_VIS_ERR_LOAD                     , STR_TEXT_VIS_ERR_LOAD },
   { STR_VIS_EXT_NOT_BMP                  , STR_TEXT_VIS_EXT_NOT_BMP },
   { STR_VIS_EXT_NOT_WAV                  , STR_TEXT_VIS_EXT_NOT_WAV },
   { STR_VIS_ANGLE                        , STR_TEXT_VIS_ANGLE },
   { STR_VIS_SCALES                       , STR_TEXT_VIS_SCALES },

  //Data Sources
   { STR_DS_ERR_NO_VALID_NAME_IN_LIST     , STR_TEXT_DS_ERR_NO_VALID_NAME_IN_LIST },
//   { STR_DS_ERR_NO_LISTS_FOUND            , STR_TEXT_DS_ERR_NO_LISTS_FOUND },
   { STR_DS_ERR_NO_RAM_LISTS_FOUND        , STR_TEXT_DS_ERR_NO_RAM_LISTS_FOUND },
//  STR_DS_ERR_DEF_FILE_NO_EXIST     "DEF file does not exist !"
   { STR_DS_ERR_OPEN_COMMENTS_FILE        , STR_TEXT_DS_ERR_OPEN_COMMENTS_FILE },
   { STR_DS_ERR_READ_DATA_VERSION         , STR_TEXT_DS_ERR_READ_DATA_VERSION },
   { STR_DS_ERR_WRITE_COMMENTS_FILE       , STR_TEXT_DS_ERR_WRITE_COMMENTS_FILE },
   { STR_DS_ERR_UNKNOWN_LIST              , STR_TEXT_DS_ERR_UNKNOWN_LIST },
   { STR_DS_ERR_DOES_NOT_EXIST            , STR_TEXT_DS_ERR_DOES_NOT_EXIST },
   { STR_DS_ERR_DOES_NOT_MATCH_DEF        , STR_TEXT_DS_ERR_DOES_NOT_MATCH_DEF },
   { STR_DS_ERR_WILL_BE_DELETED           , STR_TEXT_DS_ERR_WILL_BE_DELETED },
   { STR_DS_ERR_CANNOT_DELETE             , STR_TEXT_DS_ERR_CANNOT_DELETE },
   { STR_DS_ERR_NO_READ_DUE_TO_LEVEL      , STR_TEXT_DS_ERR_NO_READ_DUE_TO_LEVEL },
   { STR_DS_ERR_NO_WRITE_DUE_TO_LEVEL     , STR_TEXT_DS_ERR_NO_WRITE_DUE_TO_LEVEL },
   { STR_DS_ERR_SENDING_REQUEST           , STR_TEXT_DS_ERR_SENDING_REQUEST },
   { STR_DS_ERR_NO_RESPONSE               , STR_TEXT_DS_ERR_NO_RESPONSE },
   { STR_DS_ERR_ERR_RESPONSE              , STR_TEXT_DS_ERR_ERR_RESPONSE },
   { STR_DS_ERR_PROJECT_NO_MATCH          , STR_TEXT_DS_ERR_PROJECT_NO_MATCH },
   { STR_DS_ERR_NUMERIC_TOO_LONG          , STR_TEXT_DS_ERR_NUMERIC_TOO_LONG },
   { STR_DS_ERR_NOT_NUMERIC               , STR_TEXT_DS_ERR_NOT_NUMERIC },
   { STR_DS_ERR_VALUE_OO_RANGE            , STR_TEXT_DS_ERR_VALUE_OO_RANGE },
   { STR_DS_ERR_STRING_LENGTH             , STR_TEXT_DS_ERR_STRING_LENGTH },
   { STR_DS_ERR_FALSE_RESPONSE            , STR_TEXT_DS_ERR_FALSE_RESPONSE },
   { STR_DS_ERR_VERIFY                    , STR_TEXT_DS_ERR_VERIFY },
   { STR_DS_ERR_USE_WRITE_LIST_CHANGED    , STR_TEXT_DS_ERR_USE_WRITE_LIST_CHANGED },
   { STR_DS_ERR_WRITING_CHECKSUM          , STR_TEXT_DS_ERR_WRITING_CHECKSUM },
   { STR_DS_ERR_INVALID_VALUE             , STR_TEXT_DS_ERR_INVALID_VALUE },
   { STR_DS_ERR_READ_NEW_VALUES           , STR_TEXT_DS_ERR_READ_NEW_VALUES },
//   { STR_DS_ERR_ERASE_FILE                , STR_TEXT_DS_ERR_ERASE_FILE },
   { STR_DS_ERR_FILE_NOT_FOUND            , STR_TEXT_DS_ERR_FILE_NOT_FOUND },
//   { STR_DS_ERR_VIEW_NO_MATCH             , STR_TEXT_DS_ERR_VIEW_NO_MATCH },
   { STR_DS_ERR_NOT_FOUND_CONTINUE        , STR_TEXT_DS_ERR_NOT_FOUND_CONTINUE },
   { STR_DS_ERR_IS_EMPTY                  , STR_TEXT_DS_ERR_IS_EMPTY },
   { STR_DS_ERR_OVERWRITE_OPENED          , STR_TEXT_DS_ERR_OVERWRITE_OPENED },
   { STR_DS_ERR_INVALID_PARAM_SIZE        , STR_TEXT_DS_ERR_INVALID_PARAM_SIZE },
   { STR_DS_ERR_INVALID_PARAM_TYPE        , STR_TEXT_DS_ERR_INVALID_PARAM_TYPE },
   { STR_DS_ERR_OVERWRITE_EXISITING       , STR_TEXT_DS_ERR_OVERWRITE_EXISITING },
   { STR_DS_ERR_CREATE_C_FILE             , STR_TEXT_DS_ERR_CREATE_C_FILE },
//  STR_DS_ERR_CREATE_H_FILE         "Could not create .H file !"
   { STR_DS_ERR_WRITE_TO_FILE             , STR_TEXT_DS_ERR_WRITE_TO_FILE },
   { STR_DS_ERR_DATA_VER_NOT_NUMERIC      , STR_TEXT_DS_ERR_DATA_VER_NOT_NUMERIC },
   { STR_DS_ERR_NO_BACKUP_POSSIBLE        , STR_TEXT_DS_ERR_NO_BACKUP_POSSIBLE },
//  { STR_DS_ERR_UNKNOWN_LIST_NAME         , STR_TEXT_DS_ERR_UNKNOWN_LIST_NAME },
//   { STR_DS_ERR_UNKNOWN_LIST_NAME_RAM     , STR_TEXT_DS_ERR_UNKNOWN_LIST_NAME_RAM },
//   { STR_DS_ERR_UNKNOWN_CRC_INFO          , STR_TEXT_DS_ERR_UNKNOWN_CRC_INFO },
//   { STR_DS_ERR_NO_LISTS_FOUND_FILE       , STR_TEXT_DS_ERR_NO_LISTS_FOUND_FILE },
   { STR_DS_ERR_COMM_FILE_NO_EXISTS       , STR_TEXT_DS_ERR_COMM_FILE_NO_EXISTS },
   { STR_DS_ERR_COMM_FILE_NO_MATCH        , STR_TEXT_DS_ERR_COMM_FILE_NO_MATCH },
   { STR_DS_ERR_COMM_TOO_MANY_LANG        , STR_TEXT_DS_ERR_COMM_TOO_MANY_LANG },
   { STR_DS_ERR_COMM_INVALID_LANGUAGE     , STR_TEXT_DS_ERR_COMM_INVALID_LANGUAGE },
//   { STR_DS_ERR_REFERENCES_REMOVE         , STR_TEXT_DS_ERR_REFERENCES_REMOVE },
//   { STR_DS_ERR_IMP_INVALID_TYPE          , STR_TEXT_DS_ERR_IMP_INVALID_TYPE },
//   { STR_DS_ERR_IMP_INVALID_STR_LEN       , STR_TEXT_DS_ERR_IMP_INVALID_STR_LEN },
//   { STR_DS_ERR_AARGHH                    , STR_TEXT_DS_ERR_AARGHH },
//   { STR_DS_ERR_SEND_ABORT_RQ             , STR_TEXT_DS_ERR_SEND_ABORT_RQ },
//   { STR_DS_ERR_ADD_CYCLIC_LIST           , STR_TEXT_DS_ERR_ADD_CYCLIC_LIST },

   { STR_DS_SCANNING_FOR_RAM              , STR_TEXT_DS_SCANNING_FOR_RAM },
   { STR_DS_ERR_PROJECT_NO_MATCH_DAT      , STR_TEXT_DS_ERR_PROJECT_NO_MATCH_DAT },

   { STR_DS_READING_COM_FILE              , STR_TEXT_DS_READING_COM_FILE },
   { STR_DS_READING_DATA_VER              , STR_TEXT_DS_READING_DATA_VER },

   { STR_DS_WRITING_RAM_FILE              , STR_TEXT_DS_WRITING_RAM_FILE },
   { STR_DS_WRITING_COM_FILE              , STR_TEXT_DS_WRITING_COM_FILE },
   { STR_DS_WRITING_DATA_VER              , STR_TEXT_DS_WRITING_DATA_VER },
   { STR_DS_FINISHED                      , STR_TEXT_DS_FINISHED },
   { STR_DS_ERROR_DDD                     , STR_TEXT_DS_ERROR_DDD },
   { STR_DS_READING_LIST                  , STR_TEXT_DS_READING_LIST },
   { STR_DS_WRITING_LIST                  , STR_TEXT_DS_WRITING_LIST },
   { STR_DS_COMPARE_LIST                  , STR_TEXT_DS_COMPARE_LIST },
//   { STR_DS_ADDRESS                       , STR_TEXT_DS_ADDRESS },
   { STR_DS_UNKNOWN_SIZE                  , STR_TEXT_DS_UNKNOWN_SIZE },
   { STR_DS_UNKNOWN_TYPE                  , STR_TEXT_DS_UNKNOWN_TYPE },
   { STR_DS_UPDATE_OF_TASK                , STR_TEXT_DS_UPDATE_OF_TASK },
   { STR_DS_CONFIRMED                     , STR_TEXT_DS_CONFIRMED },
   { STR_DS_INVALID_TYPE                  , STR_TEXT_DS_INVALID_TYPE },
   { STR_DS_INVALID_ACC_TYPE              , STR_TEXT_DS_INVALID_ACC_TYPE },
//   { STR_DS_INDEX                         , STR_TEXT_DS_INDEX },

  //KFXFlashloader
   { STR_XFL_ERR_INVALID_RESET_FORMAT     , STR_TEXT_XFL_ERR_INVALID_RESET_FORMAT },

  //KFXProcess
//   { STR_KFX_NO_DONGLE_PASSWORD           , STR_TEXT_KFX_NO_DONGLE_PASSWORD },
//   { STR_KFX_BGROUP_RANGE                 , STR_TEXT_KFX_BGROUP_RANGE },
//  STR_KFX_NEW_DEVICE_NAME      "Enter a device name for this project"
   { STR_KFX_DEF_FILTER                   , STR_TEXT_KFX_DEF_FILTER },
   { STR_KFX_SELECT_FILE_NAME             , STR_TEXT_KFX_SELECT_FILE_NAME },
   { STR_KFX_EXISTS_OVERWRITE             , STR_TEXT_KFX_EXISTS_OVERWRITE },
   { STR_KFX_OVERWRITE_ALL                , STR_TEXT_KFX_OVERWRITE_ALL },
   { STR_KFX_ERR_INVALID_DEF_FILE         , STR_TEXT_KFX_ERR_INVALID_DEF_FILE },
//  STR_KFX_NO_DYNAMIC_LANGUAGE  "Dynamic language switching is only possible using dynamic loading of packages (use VisLibRT.bpl) !"

  //XFLDownload
   { STR_FDL_VERSION_ERR                  , STR_TEXT_FDL_VERSION_ERR },
//  STR_FDL_VERSION_ERR_NO_SUPP  "Error: Flashloader version not supported !"
//  STR_FDL_VERSION_ERR_UNDEF_CODE "Error: Flashloader version could not be read: Undefined error code !"
//  STR_FDL_VERSION_ERR_UNDEF    "Error: Flashloader version could not be read: Undefined error !"
   { STR_FDL_DIVERT_ERR_NO_SUPP           , STR_TEXT_FDL_DIVERT_ERR_NO_SUPP },
   { STR_FDL_VERSION_BBB_ERR_NO_RESP      , STR_TEXT_FDL_VERSION_BBB_ERR_NO_RESP },
   { STR_FDL_VERSION_BBB_ERR_NO_SUPP      , STR_TEXT_FDL_VERSION_BBB_ERR_NO_SUPP },
   { STR_FDL_VERSION_BBB_ERR_UNDEF_CODE   , STR_TEXT_FDL_VERSION_BBB_ERR_UNDEF_CODE },
   { STR_FDL_VERSION_BBB_ERR_UNDEF        , STR_TEXT_FDL_VERSION_BBB_ERR_UNDEF },
   { STR_FDL_ERR_C_APPL_NOT_POSSIBLE      , STR_TEXT_FDL_ERR_C_APPL_NOT_POSSIBLE },
   { STR_FDL_ERR_CANOPEN_NOT_POSSIBLE     , STR_TEXT_FDL_ERR_CANOPEN_NOT_POSSIBLE },
   { STR_FDL_ERR_IECAPPL_NOT_POSSIBLE     , STR_TEXT_FDL_ERR_IECAPPL_NOT_POSSIBLE },
   { STR_FDL_ERR_IECRTS_NOT_POSSIBLE      , STR_TEXT_FDL_ERR_IECRTS_NOT_POSSIBLE },
   { STR_FDL_ERR_SEC_ERASE_INVALID        , STR_TEXT_FDL_ERR_SEC_ERASE_INVALID },
   { STR_FDL_ERR_WRITING_CRCS             , STR_TEXT_FDL_ERR_WRITING_CRCS },
   { STR_FDL_ERR_FLASHING                 , STR_TEXT_FDL_ERR_FLASHING },
   { STR_FDL_ERR_SEC_ERASE_INV2           , STR_TEXT_FDL_ERR_SEC_ERASE_INV2 },
   { STR_FDL_ERR_ERASE                    , STR_TEXT_FDL_ERR_ERASE },
//  STR_FDL_ERR_ERASE_NO_RESP    "No response from node"
//  STR_FDL_ERR_ERASE_NO_ACT     "Could not be erased"
//  STR_FDL_ERR_ERASE_NO_INVALID "Invalid sector number"
   { STR_FDL_ERR_ERASE_NO_CODE            , STR_TEXT_FDL_ERR_ERASE_NO_CODE },
//  STR_FDL_ERR_ERASE_NO_UNDEF_CODE "Undefined error code"
//  STR_FDL_ERR_ERASE_NO_UNDEF   "Undefined error"
//  STR_FDL_ERR_WRITE_NO_RESP    "Error writing: No response or TX error (line"
//  STR_FDL_ERR_WRITE_NO_ACT     "Error writing: Programming error (line"
   { STR_FDL_ERR_WRITE_CHECKSUM           , STR_TEXT_FDL_ERR_WRITE_CHECKSUM },
//  STR_FDL_ERR_WRITE_INV_COMM   "Error writing: Invalid command (line"
//  STR_FDL_ERR_WRITE_INV_ADDR   "Error writing: Invalid address (line"
//  STR_FDL_ERR_WRITE_CODE       "Error writing: Error code (line"
//  STR_FDL_ERR_WRITE_UNDEF_CODE "Error writing: Undefined error code (line"
//  STR_FDL_ERR_WRITE_VERIFY     "Error writing: Server reports checksum different from checksum in hex file (line"

   { STR_FDL_SEC_COUNT_ERR                , STR_TEXT_FDL_SEC_COUNT_ERR },
   { STR_FDL_NUM_SECTORS                  , STR_TEXT_FDL_NUM_SECTORS },
   { STR_FDL_WRITING_CRCS                 , STR_TEXT_FDL_WRITING_CRCS },
   { STR_FDL_CRCS_NO_SUPP                 , STR_TEXT_FDL_CRCS_NO_SUPP },
   { STR_FDL_TOTAL_TIME                   , STR_TEXT_FDL_TOTAL_TIME },
   { STR_FDL_FILE_NOT_FOUND               , STR_TEXT_FDL_FILE_NOT_FOUND },
   { STR_FDL_SEC_ERASE                    , STR_TEXT_FDL_SEC_ERASE },
   { STR_FDL_ERASING                      , STR_TEXT_FDL_ERASING },
   { STR_FDL_ERASING_SECTOR               , STR_TEXT_FDL_ERASING_SECTOR },
   { STR_FDL_ERASE_TIME                   , STR_TEXT_FDL_ERASE_TIME },
   { STR_FDL_BYTES                        , STR_TEXT_FDL_BYTES },
   { STR_FDL_BYTE                         , STR_TEXT_FDL_BYTE },
   { STR_FDL_DOWNLOAD_TIME                , STR_TEXT_FDL_DOWNLOAD_TIME },
   { STR_FDL_AVERAGE_RATE                 , STR_TEXT_FDL_AVERAGE_RATE },
   { STR_FDL_UNKNOWN_ERR                  , STR_TEXT_FDL_UNKNOWN_ERR },

   { STR_FDL_DFM_RGSELEC_NODE_RESET       , STR_TEXT_FDL_DFM_RGSELEC_NODE_RESET },
   { STR_FDL_DFM_RGSELEC_NET_RESET        , STR_TEXT_FDL_DFM_RGSELEC_NET_RESET },
   { STR_FDL_DFM_RGSELEC_STAY_SLEEP       , STR_TEXT_FDL_DFM_RGSELEC_STAY_SLEEP },
   { STR_FDL_DFM_RGSELEC_NODE_RETURN      , STR_TEXT_FDL_DFM_RGSELEC_NODE_RETURN },
   { STR_FDL_DFM_RGSELEC_NET_START        , STR_TEXT_FDL_DFM_RGSELEC_NET_START },
   
   { STR_FDL_SBC_SUPPORTED                , STR_TEXT_FDL_SBC_SUPPORTED                },
   { STR_FDL_BBC_SUPPORTED                , STR_TEXT_FDL_BBC_SUPPORTED                },
   { STR_FDL_CS_DEACTIVATED               , STR_TEXT_FDL_CS_DEACTIVATED               },
   { STR_FDL_ERR_FL_IMPLEMENTATION        , STR_TEXT_FDL_ERR_FL_IMPLEMENTATION        },
   { STR_FDL_ERR_HEX_ON_PROTECTED_S       , STR_TEXT_FDL_ERR_HEX_ON_PROTECTED_S       },
   { STR_FDL_HINT_USE_AUTO_MODE           , STR_TEXT_FDL_HINT_USE_AUTO_MODE           },
   { STR_FDL_ERR_ONLY_F_AUTO_UD           , STR_TEXT_FDL_ERR_ONLY_F_AUTO_UD           },
   { STR_FDL_ERR_FILETYPE_AUTO_NA         , STR_TEXT_FDL_ERR_FILETYPE_AUTO_NA         },
   { STR_FDL_ERR_INVALID_HEX_FILE         , STR_TEXT_FDL_ERR_INVALID_HEX_FILE         },
   { STR_FDL_ERR_WR_PROG_DATA             , STR_TEXT_FDL_ERR_WR_PROG_DATA             },
   { STR_FDL_ERR_WR_PROG_TIME             , STR_TEXT_FDL_ERR_WR_PROG_TIME             },
   { STR_FDL_ERR_RD_USER_NAME             , STR_TEXT_FDL_ERR_RD_USER_NAME             },
   { STR_FDL_ERR_WR_USER_NAME             , STR_TEXT_FDL_ERR_WR_USER_NAME             },
   { STR_FDL_ERR_WR_APPL_CHECKS           , STR_TEXT_FDL_ERR_WR_APPL_CHECKS           },
   { STR_FDL_TXT_REC_USER_N_P1            , STR_TEXT_FDL_TXT_REC_USER_N_P1            },
   { STR_FDL_TXT_REC_USER_N_P2            , STR_TEXT_FDL_TXT_REC_USER_N_P2            },
   { STR_FDL_TXT_IGNORE                   , STR_TEXT_FDL_TXT_IGNORE                   },
   { STR_FDL_ERR_RD_HEX_REC_INFO          , STR_TEXT_FDL_ERR_RD_HEX_REC_INFO          },
   { STR_FDL_TXT_CHG_HEX_REC_LENGTH       , STR_TEXT_FDL_TXT_CHG_HEX_REC_LENGTH       },
   { STR_FDL_TXT_DO_CHG_HEX_REC_LENGTH    , STR_TEXT_FDL_TXT_DO_CHG_HEX_REC_LENGTH    },
   { STR_FDL_ERR_ENTER_PROG_MODE          , STR_TEXT_FDL_ERR_ENTER_PROG_MODE          },
   { STR_FDL_TXT_WR_FLASH_DO              , STR_TEXT_FDL_TXT_WR_FLASH_DO              },
   { STR_FDL_ERR_WR_FLASH_DO_PT1          , STR_TEXT_FDL_ERR_WR_FLASH_DO_PT1          },
   { STR_FDL_ERR_WR_FLASH_DO_PT2          , STR_TEXT_FDL_ERR_WR_FLASH_DO_PT2          },
   { STR_FDL_ERR_USER_ABORT               , STR_TEXT_FDL_ERR_USER_ABORT               },
   { STR_FDL_WARN_COULD_NOT_CALC_APPL_CHK , STR_TEXT_FDL_WARN_COULD_NOT_CALC_APPL_CHK },
   { STR_FDL_WARN_COULD_NOT_WR_APPL_CHK   , STR_TEXT_FDL_WARN_COULD_NOT_WR_APPL_CHK   },
   { STR_FDL_ERR_CFG_DIC                  , STR_TEXT_FDL_ERR_CFG_DIC                  },
   { STR_FDL_TXT_ID_CHECK_PASSED          , STR_TEXT_FDL_TXT_ID_CHECK_PASSED          },
   { STR_FDL_TXT_ID_OF_HEX_FILE           , STR_TEXT_FDL_TXT_ID_OF_HEX_FILE           },
   { STR_FDL_TXT_ID_CHECKING              , STR_TEXT_FDL_TXT_ID_CHECKING              },
   { STR_FDL_TXT_ID_CHECK_NODE_HEX        , STR_TEXT_FDL_TXT_ID_CHECK_NODE_HEX        },
   { STR_FDL_TXT_ID_CHECK_ADDR_NA         , STR_TEXT_FDL_TXT_ID_CHECK_ADDR_NA         },
   { STR_FDL_ERR_ID_CHECK_RD_NODE         , STR_TEXT_FDL_ERR_ID_CHECK_RD_NODE         },
   { STR_FDL_ERR_ID_CHECK_ID_ADDR_NA      , STR_TEXT_FDL_ERR_ID_CHECK_ID_ADDR_NA      },
   { STR_FDL_ERR_ID_CHECK_ID_NA_HEX       , STR_TEXT_FDL_ERR_ID_CHECK_ID_NA_HEX       },
   { STR_FDL_ERR_ID_CHECK_ID_AMBIG_HEX    , STR_TEXT_FDL_ERR_ID_CHECK_ID_AMBIG_HEX    },
   { STR_FDL_ERR_ID_CHECK_ID_UNDEF        , STR_TEXT_FDL_ERR_ID_CHECK_ID_UNDEF        },
   { STR_FDL_ERR_ID_CHECK_IGNOR           , STR_TEXT_FDL_ERR_ID_CHECK_IGNOR           },
   { STR_FDL_TXT_ID_NA_CHECK_HEX_FILE     , STR_TEXT_FDL_TXT_ID_NA_CHECK_HEX_FILE     },
   { STR_FDL_ID_CHECK_MATCH               , STR_TEXT_FDL_ID_CHECK_MATCH               },
   { STR_FDL_ERR_ID_CHECK_MISMATCH_PT1    , STR_TEXT_FDL_ERR_ID_CHECK_MISMATCH_PT1    },
   { STR_FDL_ERR_ID_CHECK_MISMATCH_PT2    , STR_TEXT_FDL_ERR_ID_CHECK_MISMATCH_PT2    },
                                                                                
   { STR_FDL_TXT_WR_FLASH_RQ              , STR_TEXT_FDL_TXT_WR_FLASH_RQ              },
   
  //XFLUpload
//  STR_FUL_ERR_START_ADDR_INVALID "Start Address must be multiple of 32 !!"
   { STR_FUL_ERR_ADDR_OVERLAP             , STR_TEXT_FUL_ERR_ADDR_OVERLAP },
//  STR_FUL_ERR_NO_RESP        "Error during reading: No response from node"
   { STR_FUL_ERR_CHECKSUM                 , STR_TEXT_FUL_ERR_CHECKSUM },
//  STR_FUL_ERR_INVALID_ADDR   "Error during reading: Invalid address"
//  STR_FUL_ERR_UNDEFINED      "Error during reading: Undefined error"
   { STR_FUL_START_ADDR_CAPTION           , STR_TEXT_FUL_START_ADDR_CAPTION },
   { STR_FUL_START_ADDR_PROMPT            , STR_TEXT_FUL_START_ADDR_PROMPT },
   { STR_FUL_END_ADDR_CAPTION             , STR_TEXT_FUL_END_ADDR_CAPTION },
   { STR_FUL_END_ADDR_PROMPT              , STR_TEXT_FUL_END_ADDR_PROMPT },
   { STR_FUL_SPARE_IRAM                   , STR_TEXT_FUL_SPARE_IRAM },
   { STR_FUL_READING_FINISHED             , STR_TEXT_FUL_READING_FINISHED },
   { STR_FUL_READING_FINISHED_BOX         , STR_TEXT_FUL_READING_FINISHED_BOX },
   { STR_FUL_ERROR                        , STR_TEXT_FUL_ERROR },
//   { STR_FUL_ADDRESS_RANGE                , STR_TEXT_FUL_ADDRESS_RANGE },
   { STR_FUL_WRITING_TO_FILE              , STR_TEXT_FUL_WRITING_TO_FILE },
   { STR_FUL_CURRENT_ADDRESS              , STR_TEXT_FUL_CURRENT_ADDRESS },

  //FORM TEXTS
  //General Texts
   { STR_DFM_OK                           , STR_TEXT_DFM_OK },
   { STR_DFM_CANCEL                       , STR_TEXT_DFM_CANCEL },
   { STR_DFM_MENU_FILE                    , STR_TEXT_DFM_MENU_FILE },
   { STR_DFM_MENU_EXIT                    , STR_TEXT_DFM_MENU_EXIT },
   { STR_DFM_MENU_HELP                    , STR_TEXT_DFM_MENU_HELP },
   { STR_DFM_MENU_ABOUT                   , STR_TEXT_DFM_MENU_ABOUT },
   { STR_DFM_YES                          , STR_TEXT_DFM_YES },
   { STR_DFM_YES_ALL                      , STR_TEXT_DFM_YES_ALL },

  //DlgCANInterface
   { STR_DFM_CI_CAPTION                   , STR_TEXT_DFM_CI_CAPTION },
   { STR_DFM_CI_DLL                       , STR_TEXT_DFM_CI_DLL },
   { STR_DFM_CI_BITRATE                   , STR_TEXT_DFM_CI_BITRATE },

  //DlgCMONFilter
   { STR_DFM_CF_CAPTION                   , STR_TEXT_DFM_CF_CAPTION },
   { STR_DFM_CF_CONFIG                    , STR_TEXT_DFM_CF_CONFIG },
   { STR_DFM_CF_FILTER1                   , STR_TEXT_DFM_CF_FILTER1 },
   { STR_DFM_CF_FILTER2                   , STR_TEXT_DFM_CF_FILTER2 },
   { STR_DFM_CF_FILTER3                   , STR_TEXT_DFM_CF_FILTER3 },
   { STR_DFM_CF_FILTER4                   , STR_TEXT_DFM_CF_FILTER4 },
   { STR_DFM_CF_MIN                       , STR_TEXT_DFM_CF_MIN },
   { STR_DFM_CF_MAX                       , STR_TEXT_DFM_CF_MAX },
   { STR_DFM_CF_APPLY                     , STR_TEXT_DFM_CF_APPLY },
   { STR_DFM_CF_TYPE                      , STR_TEXT_DFM_CF_TYPE },
   { STR_DFM_CF_BLOCK                     , STR_TEXT_DFM_CF_BLOCK },
   { STR_DFM_CF_PASS                      , STR_TEXT_DFM_CF_PASS },

  //DlgCMONMain
   { STR_DFM_CM_HINT_SEND_CONFIG          , STR_TEXT_DFM_CM_HINT_SEND_CONFIG },
   { STR_DFM_CM_HINT_START                , STR_TEXT_DFM_CM_HINT_START },
   { STR_DFM_CM_HINT_STOP                 , STR_TEXT_DFM_CM_HINT_STOP },
   { STR_DFM_CM_HINT_TIME                 , STR_TEXT_DFM_CM_HINT_TIME },
   { STR_DFM_CM_HINT_BASE                 , STR_TEXT_DFM_CM_HINT_BASE },
   { STR_DFM_CM_HINT_MODE                 , STR_TEXT_DFM_CM_HINT_MODE },
   { STR_DFM_CM_HINT_PROT                 , STR_TEXT_DFM_CM_HINT_PROT },
   { STR_DFM_CM_HINT_L7                   , STR_TEXT_DFM_CM_HINT_L7 },
   { STR_DFM_CM_ACTIVE                    , STR_TEXT_DFM_CM_ACTIVE },
   { STR_DFM_CM_TX_ERRORS                 , STR_TEXT_DFM_CM_TX_ERRORS },
   { STR_DFM_CM_FPS_HINT                  , STR_TEXT_DFM_CM_FPS_HINT },
   { STR_DFM_CM_FPS                       , STR_TEXT_DFM_CM_FPS },
   { STR_DFM_CM_ASCII_FILTER              , STR_TEXT_DFM_CM_ASCII_FILTER },
   { STR_DFM_CM_MENU_START_PROT           , STR_TEXT_DFM_CM_MENU_START_PROT },
   { STR_DFM_CM_MENU_STOP_PROT            , STR_TEXT_DFM_CM_MENU_STOP_PROT },
   { STR_DFM_CM_MENU_ACTIONS              , STR_TEXT_DFM_CM_MENU_ACTIONS },
   { STR_DFM_CM_MENU_START                , STR_TEXT_DFM_CM_MENU_START },
   { STR_DFM_CM_MENU_STOP                 , STR_TEXT_DFM_CM_MENU_STOP },
   { STR_DFM_CM_MENU_CONFIG               , STR_TEXT_DFM_CM_MENU_CONFIG },
   { STR_DFM_CM_MENU_BUS_PARAMS           , STR_TEXT_DFM_CM_MENU_BUS_PARAMS },
   { STR_DFM_CM_MENU_CAN_DLL              , STR_TEXT_DFM_CM_MENU_CAN_DLL },
   { STR_DFM_CM_MENU_DLL_INFO             , STR_TEXT_DFM_CM_MENU_DLL_INFO },
   { STR_DFM_CM_MENU_SEND_CONFIG          , STR_TEXT_DFM_CM_MENU_SEND_CONFIG },
   { STR_DFM_CM_MENU_RX_FILTERS           , STR_TEXT_DFM_CM_MENU_RX_FILTERS },
   { STR_DFM_CM_MENU_CAN_STATUS           , STR_TEXT_DFM_CM_MENU_CAN_STATUS },
   { STR_DFM_CM_MENU_PROP_PARAMS          , STR_TEXT_DFM_CM_MENU_PROP_PARAMS },

  //DlgCMONSend
   { STR_DFM_CS_CAPTION                   , STR_TEXT_DFM_CS_CAPTION },
   { STR_DFM_CS_ADD                       , STR_TEXT_DFM_CS_ADD },
   { STR_DFM_CS_DELETE                    , STR_TEXT_DFM_CS_DELETE },
   { STR_DFM_CS_LOAD                      , STR_TEXT_DFM_CS_LOAD },
   { STR_DFM_CS_SAVE                      , STR_TEXT_DFM_CS_SAVE },
   { STR_DFM_CS_CLONE                     , STR_TEXT_DFM_CS_CLONE },
   { STR_DFM_CS_EXIT                      , STR_TEXT_DFM_CS_EXIT },
   { STR_DFM_CS_APPLY                     , STR_TEXT_DFM_CS_APPLY },
   { STR_DFM_CS_TRIGGER                   , STR_TEXT_DFM_CS_TRIGGER },
   { STR_DFM_CS_KEY                       , STR_TEXT_DFM_CS_KEY },
   { STR_DFM_CS_INTERVAL_HINT             , STR_TEXT_DFM_CS_INTERVAL_HINT },
   { STR_DFM_CS_INTERVAL_CAP              , STR_TEXT_DFM_CS_INTERVAL_CAP },
   { STR_DFM_CS_ACT_HINT                  , STR_TEXT_DFM_CS_ACT_HINT },
   { STR_DFM_CS_ACT_CAP                   , STR_TEXT_DFM_CS_ACT_CAP },
   { STR_DFM_CS_RTR_HINT                  , STR_TEXT_DFM_CS_RTR_HINT },
   { STR_DFM_CS_RTR_CAP                   , STR_TEXT_DFM_CS_RTR_CAP },
   { STR_DFM_CS_XTD_HINT                  , STR_TEXT_DFM_CS_XTD_HINT },
   { STR_DFM_CS_XTD_CAP                   , STR_TEXT_DFM_CS_XTD_CAP },
   { STR_DFM_CS_DB7_HINT                  , STR_TEXT_DFM_CS_DB7_HINT },
   { STR_DFM_CS_DB7_CAP                   , STR_TEXT_DFM_CS_DB7_CAP },
   { STR_DFM_CS_DB6_HINT                  , STR_TEXT_DFM_CS_DB6_HINT },
   { STR_DFM_CS_DB6_CAP                   , STR_TEXT_DFM_CS_DB6_CAP },
   { STR_DFM_CS_DB5_HINT                  , STR_TEXT_DFM_CS_DB5_HINT },
   { STR_DFM_CS_DB5_CAP                   , STR_TEXT_DFM_CS_DB5_CAP },
   { STR_DFM_CS_DB4_HINT                  , STR_TEXT_DFM_CS_DB4_HINT },
   { STR_DFM_CS_DB4_CAP                   , STR_TEXT_DFM_CS_DB4_CAP },
   { STR_DFM_CS_DB3_HINT                  , STR_TEXT_DFM_CS_DB3_HINT },
   { STR_DFM_CS_DB3_CAP                   , STR_TEXT_DFM_CS_DB3_CAP },
   { STR_DFM_CS_DB2_HINT                  , STR_TEXT_DFM_CS_DB2_HINT },
   { STR_DFM_CS_DB2_CAP                   , STR_TEXT_DFM_CS_DB2_CAP },
   { STR_DFM_CS_DB1_HINT                  , STR_TEXT_DFM_CS_DB1_HINT },
   { STR_DFM_CS_DB1_CAP                   , STR_TEXT_DFM_CS_DB1_CAP },
   { STR_DFM_CS_DB0_HINT                  , STR_TEXT_DFM_CS_DB0_HINT },
   { STR_DFM_CS_DB0_CAP                   , STR_TEXT_DFM_CS_DB0_CAP },
   { STR_DFM_CS_DLC_HINT                  , STR_TEXT_DFM_CS_DLC_HINT },
   { STR_DFM_CS_DLC_CAP                   , STR_TEXT_DFM_CS_DLC_CAP },
   { STR_DFM_CS_ID_HINT                   , STR_TEXT_DFM_CS_ID_HINT },
   { STR_DFM_CS_ID_CAP                    , STR_TEXT_DFM_CS_ID_CAP },
   { STR_DFM_CS_OFFSET_HINT               , STR_TEXT_DFM_CS_OFFSET_HINT },
   { STR_DFM_CS_OFFSET_CAP                , STR_TEXT_DFM_CS_OFFSET_CAP },
   { STR_DFM_CS_CSL_FILTER                , STR_TEXT_DFM_CS_CSL_FILTER },

  //DlgCMONStatus
   { STR_DFM_CST_CAPTION                  , STR_TEXT_DFM_CST_CAPTION },
   { STR_DFM_CST_GB_CAP                   , STR_TEXT_DFM_CST_GB_CAP },
   { STR_DFM_CST_BOFF                     , STR_TEXT_DFM_CST_BOFF },
   { STR_DFM_CST_BWARN                    , STR_TEXT_DFM_CST_BWARN },
   { STR_DFM_CST_TXOVER                   , STR_TEXT_DFM_CST_TXOVER },
   { STR_DFM_CST_RXOVER                   , STR_TEXT_DFM_CST_RXOVER },
   { STR_DFM_CST_CLOSE                    , STR_TEXT_DFM_CST_CLOSE },
   { STR_DFM_CST_CYCLIC                   , STR_TEXT_DFM_CST_CYCLIC },

  //DlgKFXCANMessageEditor/DlgXFLResetRQ
   { STR_DFM_CME_CAPTION                  , STR_TEXT_DFM_CME_CAPTION },
   { STR_DFM_CRM_CAPTION                  , STR_TEXT_DFM_CRM_CAPTION },
   { STR_DFM_CME_ID                       , STR_TEXT_DFM_CME_ID },
   { STR_DFM_CME_DLC                      , STR_TEXT_DFM_CME_DLC },
   { STR_DFM_CME_DB0                      , STR_TEXT_DFM_CME_DB0 },
   { STR_DFM_CME_DB1                      , STR_TEXT_DFM_CME_DB1 },
   { STR_DFM_CME_DB2                      , STR_TEXT_DFM_CME_DB2 },
   { STR_DFM_CME_DB3                      , STR_TEXT_DFM_CME_DB3 },
   { STR_DFM_CME_DB4                      , STR_TEXT_DFM_CME_DB4 },
   { STR_DFM_CME_DB5                      , STR_TEXT_DFM_CME_DB5 },
   { STR_DFM_CME_DB6                      , STR_TEXT_DFM_CME_DB6 },
   { STR_DFM_CME_DB7                      , STR_TEXT_DFM_CME_DB7 },

  //DlgKFXComSetup
   { STR_DFM_CSE_CAPTION                  , STR_TEXT_DFM_CSE_CAPTION },
   { STR_DFM_CSE_PROT                     , STR_TEXT_DFM_CSE_PROT },
//  STR_DFM_CSE_BASE_ID  "Base ID:"
//  STR_DFM_CSE_CLIENT   "Client (PC) Address:"
//  STR_DFM_CSE_SERVER   "Server (ECU) Address:"
   { STR_DFM_CSE_INTERFACE                , STR_TEXT_DFM_CSE_INTERFACE },
   { STR_DFM_CSE_HARDWARE                 , STR_TEXT_DFM_CSE_HARDWARE },
   { STR_DFM_CSE_CONFIG                   , STR_TEXT_DFM_CSE_CONFIG },

  //DlgXFLMain
  //DFM Texts
   { STR_DFM_XM_WAKEUPLOCALID_ESX         , STR_TEXT_DFM_XM_WAKEUPLOCALID_ESX },
   { STR_DFM_XM_READ_FROM_ESX_ESX         , STR_TEXT_DFM_XM_READ_FROM_ESX_ESX },
   { STR_DFM_XM_FLASHLOCALID_ESX          , STR_TEXT_DFM_XM_FLASHLOCALID_ESX },
   { STR_DFM_XM_WAKEUPLOCALID_BBB         , STR_TEXT_DFM_XM_WAKEUPLOCALID_BBB },
   { STR_DFM_XM_READ_FROM_ESX_BBB         , STR_TEXT_DFM_XM_READ_FROM_ESX_BBB },
   { STR_DFM_XM_FLASHLOCALID_BBB          , STR_TEXT_DFM_XM_FLASHLOCALID_BBB },
   { STR_DFM_XM_CAP_HEXFILE               , STR_TEXT_DFM_XM_CAP_HEXFILE },
   { STR_DFM_XM_C_APPLICATION             , STR_TEXT_DFM_XM_C_APPLICATION },
   { STR_DFM_XM_CANOPEN_CONFIG            , STR_TEXT_DFM_XM_CANOPEN_CONFIG },
   { STR_DFM_XM_IEC_APPLICATION           , STR_TEXT_DFM_XM_IEC_APPLICATION },
   { STR_DFM_XM_IEC_RTS                   , STR_TEXT_DFM_XM_IEC_RTS },
   { STR_DFM_XM_USER_DEFINED              , STR_TEXT_DFM_XM_USER_DEFINED },
   { STR_DFM_XM_BBB_APPLICATION           , STR_TEXT_DFM_XM_BBB_APPLICATION },
   { STR_DFM_XM_BBB_PARAMETERS            , STR_TEXT_DFM_XM_BBB_PARAMETERS },

   { STR_DFM_XM_CAPTION                   , STR_TEXT_DFM_XM_CAPTION },
   { STR_DFM_XM_CANDLL                    , STR_TEXT_DFM_XM_CANDLL },
   { STR_DFM_XM_SELECT                    , STR_TEXT_DFM_XM_SELECT },
   { STR_DFM_XM_VIEW                      , STR_TEXT_DFM_XM_VIEW },
   { STR_DFM_XM_STARTTIME                 , STR_TEXT_DFM_XM_STARTTIME },
   { STR_DFM_XM_DELAY                     , STR_TEXT_DFM_XM_DELAY },
   { STR_DFM_XM_CONFIG                    , STR_TEXT_DFM_XM_CONFIG },
   { STR_DFM_XM_DELAY_HINT                , STR_TEXT_DFM_XM_DELAY_HINT },
   { STR_DFM_XM_STIME_HINT                , STR_TEXT_DFM_XM_STIME_HINT },
   { STR_DFM_XM_EXIT                      , STR_TEXT_DFM_XM_EXIT },
   { STR_DFM_XM_SEARCH                    , STR_TEXT_DFM_XM_SEARCH },
   { STR_DFM_XM_TRIM                      , STR_TEXT_DFM_XM_TRIM },
   { STR_DFM_XM_WAKEUP_SNR                , STR_TEXT_DFM_XM_WAKEUP_SNR },
//  STR_DFM_XM_SW_BIN     "SW Bin Download"
//  STR_DFM_XM_SW_HEX     "SW Hex Download"
   { STR_DFM_XM_FPARAMS                   , STR_TEXT_DFM_XM_FPARAMS },
   { STR_DFM_XM_SENDID                    , STR_TEXT_DFM_XM_SENDID },
   { STR_DFM_XM_CID                       , STR_TEXT_DFM_XM_CID },
   { STR_DFM_XM_LID                       , STR_TEXT_DFM_XM_LID },
   { STR_DFM_XM_SNR                       , STR_TEXT_DFM_XM_SNR },
   { STR_DFM_XM_BR                        , STR_TEXT_DFM_XM_BR },
//  STR_DFM_XM_SW_BIN_ID  "Bin DL TX ID:"
//  STR_DFM_XM_SW_HEX_ID  "Hex DL TX ID:"
   { STR_DFM_XM_SEND_ID_HINT              , STR_TEXT_DFM_XM_SEND_ID_HINT },
   { STR_DFM_XM_CID_HINT                  , STR_TEXT_DFM_XM_CID_HINT },
   { STR_DFM_XM_LID_HINT                  , STR_TEXT_DFM_XM_LID_HINT },
   { STR_DFM_XM_FILETYPE                  , STR_TEXT_DFM_XM_FILETYPE },
   { STR_DFM_XM_SECTORS                   , STR_TEXT_DFM_XM_SECTORS },
   { STR_DFM_XM_RESET_RQ                  , STR_TEXT_DFM_XM_RESET_RQ },
   { STR_DFM_XM_RESET_MSG_HINT            , STR_TEXT_DFM_XM_RESET_MSG_HINT },
   { STR_DFM_XM_RESET_MSG_MSG             , STR_TEXT_DFM_XM_RESET_MSG_MSG },
   { STR_DFM_XM_RESET_MSG_SEND            , STR_TEXT_DFM_XM_RESET_MSG_SEND },
   { STR_DFM_XM_RESET_MSG_CFG             , STR_TEXT_DFM_XM_RESET_MSG_CFG },
//  STR_DFM_XM_DEBUG          "DEBUG Interface Setup"
   { STR_DFM_XM_FILTER_HEXFILES           , STR_TEXT_DFM_XM_FILTER_HEXFILES },
   { STR_DFM_XM_FILTER_CANDLLS            , STR_TEXT_DFM_XM_FILTER_CANDLLS },
   { STR_DFM_XM_FILTER_HEXFILE            , STR_TEXT_DFM_XM_FILTER_HEXFILE },
   { STR_DFM_XM_HEXFILE_SELECT            , STR_TEXT_DFM_XM_HEXFILE_SELECT },
   { STR_DFM_XM_DLL_SELECT                , STR_TEXT_DFM_XM_DLL_SELECT },
   { STR_DFM_XM_SAVE_CONFIG               , STR_TEXT_DFM_XM_SAVE_CONFIG },
   { STR_DFM_XM_MENU_ACTIONS              , STR_TEXT_DFM_XM_MENU_ACTIONS },
   { STR_DFM_XM_VERIFY                    , STR_TEXT_DFM_XM_VERIFY },
   { STR_DFM_XM_FLASH_SNR                 , STR_TEXT_DFM_XM_FLASH_SNR },
   { STR_DFM_XM_TARGET                    , STR_TEXT_DFM_XM_TARGET },
   { STR_DFM_XM_MAINBOARD                 , STR_TEXT_DFM_XM_MAINBOARD },
   { STR_DFM_XM_BBB                       , STR_TEXT_DFM_XM_BBB },

  //DlgKFXEditPasswords/DlgKFXPasswords
   { STR_DFM_EP_CAPTION                   , STR_TEXT_DFM_EP_CAPTION },
   { STR_DFM_EP_PASSWORD                  , STR_TEXT_DFM_EP_PASSWORD },
   { STR_DFM_EP_CONFIRM                   , STR_TEXT_DFM_EP_CONFIRM },
   { STR_DFM_EP_GROUP                     , STR_TEXT_DFM_EP_GROUP },
   { STR_DFM_EP_CB_GROUP0                 , STR_TEXT_DFM_EP_CB_GROUP0 },
   { STR_DFM_EP_CB_GROUP1                 , STR_TEXT_DFM_EP_CB_GROUP1 },
   { STR_DFM_EP_CB_GROUP2                 , STR_TEXT_DFM_EP_CB_GROUP2 },
   { STR_DFM_EP_CB_GROUP3                 , STR_TEXT_DFM_EP_CB_GROUP3 },
   { STR_DFM_EP_CB_GROUP4                 , STR_TEXT_DFM_EP_CB_GROUP4 },
   { STR_DFM_EP_CB_GROUP5                 , STR_TEXT_DFM_EP_CB_GROUP5 },
   { STR_DFM_EP_CB_GROUP6                 , STR_TEXT_DFM_EP_CB_GROUP6 },
   { STR_DFM_EP_CB_GROUP7                 , STR_TEXT_DFM_EP_CB_GROUP7 },
   { STR_DFM_EP_CB_GROUP8                 , STR_TEXT_DFM_EP_CB_GROUP8 },
   { STR_DFM_EP_CB_GROUP9                 , STR_TEXT_DFM_EP_CB_GROUP9 },
   { STR_DFM_EP_CB_PROGRAMMER             , STR_TEXT_DFM_EP_CB_PROGRAMMER },
   { STR_DFM_EP_APPLY                     , STR_TEXT_DFM_EP_APPLY },
   { STR_DFM_PWD_CAPTION                  , STR_TEXT_DFM_PWD_CAPTION },
   { STR_DFM_PWD_GB_CAPTION               , STR_TEXT_DFM_PWD_GB_CAPTION },

  //DlgKFXIndexedInputListEditor
   { STR_DFM_IILE_CAPTION                 , STR_TEXT_DFM_IILE_CAPTION },
   { STR_DFM_IILE_NEW                     , STR_TEXT_DFM_IILE_NEW },
   { STR_DFM_IILE_DELETE                  , STR_TEXT_DFM_IILE_DELETE },
   { STR_DFM_IILE_HELP1                   , STR_TEXT_DFM_IILE_HELP1 },
   { STR_DFM_IILE_HELP2                   , STR_TEXT_DFM_IILE_HELP2 },
   { STR_DFM_IILE_FILTER_BMPS             , STR_TEXT_DFM_IILE_FILTER_BMPS },

  //DlgKFXIndexedListEditor
   { STR_DFM_ILE_CAPTION                  , STR_TEXT_DFM_ILE_CAPTION },
   { STR_DFM_ILE_HELP1                    , STR_TEXT_DFM_ILE_HELP1 },
   { STR_DFM_ILE_HELP2                    , STR_TEXT_DFM_ILE_HELP2 },
   { STR_DFM_ILE_HELP3                    , STR_TEXT_DFM_ILE_HELP3 },
   { STR_DFM_ILE_HELP4                    , STR_TEXT_DFM_ILE_HELP4 },
   { STR_DFM_ILE_HELP5                    , STR_TEXT_DFM_ILE_HELP5 },

  //DlgKFXIndexedStringEditor
   { STR_DFM_ISE_CAPTION                  , STR_TEXT_DFM_ISE_CAPTION },
   { STR_DFM_ISE_HELP1                    , STR_TEXT_DFM_ISE_HELP1 },
   { STR_DFM_ISE_HELP2                    , STR_TEXT_DFM_ISE_HELP2 },

  //DlgXFLWakeup
   { STR_DFM_XW_CAPTION                   , STR_TEXT_DFM_XW_CAPTION },
   { STR_DFM_XW_CONFIG                    , STR_TEXT_DFM_XW_CONFIG },
   { STR_DFM_XW_SLEEP                     , STR_TEXT_DFM_XW_SLEEP },
   { STR_DFM_XW_RETURN                    , STR_TEXT_DFM_XW_RETURN },
   { STR_DFM_XW_RESET                     , STR_TEXT_DFM_XW_RESET },
   { STR_DFM_XW_SET                       , STR_TEXT_DFM_XW_SET },
   { STR_DFM_XW_FLASH_CRCS                , STR_TEXT_DFM_XW_FLASH_CRCS },
//  STR_DFM_XW_CMP             "CMP  Block Nr / Calc CRC / CRC in EE / Start Add / End Add"
//  STR_DFM_XW_SET_CRC         "Set CRC"
   { STR_DFM_XW_GET_CRCS                  , STR_TEXT_DFM_XW_GET_CRCS },
   { STR_DFM_XW_TO_FACTORS                , STR_TEXT_DFM_XW_TO_FACTORS },
   { STR_DFM_XW_TO_FACTOR_ERASE           , STR_TEXT_DFM_XW_TO_FACTOR_ERASE },
   { STR_DFM_XW_TO_FACTOR_WRITE           , STR_TEXT_DFM_XW_TO_FACTOR_WRITE },
   { STR_DFM_XW_TO_ERASE_HINT             , STR_TEXT_DFM_XW_TO_ERASE_HINT },
   { STR_DFM_XW_TO_WRITE_HINT             , STR_TEXT_DFM_XW_TO_WRITE_HINT },
   { STR_DFM_XW_INFORMATION               , STR_TEXT_DFM_XW_INFORMATION },
   { STR_DFM_XW_BBBINFO                   , STR_TEXT_DFM_XW_BBBINFO },
   { STR_DFM_XW_LAST_FLASHED              , STR_TEXT_DFM_XW_LAST_FLASHED },

  //DlgXFLSearch
   { STR_DFM_XS_CAPTION                   , STR_TEXT_DFM_XS_CAPTION },
   { STR_DFM_XS_NODE_IDS                  , STR_TEXT_DFM_XS_NODE_IDS },
//  STR_DFM_XS_SNRS            "SNRs:"
   { STR_DFM_XS_CHANGEID                  , STR_TEXT_DFM_XS_CHANGEID },

  //DlgXFLBBBDivertStream
   { STR_DFM_XBB_CAPTION                  , STR_TEXT_DFM_XBB_CAPTION },
   { STR_DFM_XBB_SETTINGS                 , STR_TEXT_DFM_XBB_SETTINGS },
   { STR_DFM_XBB_SLOT                     , STR_TEXT_DFM_XBB_SLOT },
   { STR_DFM_XBB_PASSWORD                 , STR_TEXT_DFM_XBB_PASSWORD },

  //DlgRVChart
   { STR_DFM_RVC_CAPTION                  , STR_TEXT_DFM_RVC_CAPTION },
   { STR_DFM_RVC_XAXIS_CAPTION            , STR_TEXT_DFM_RVC_XAXIS_CAPTION },
   { STR_DFM_RVC_HELP1                    , STR_TEXT_DFM_RVC_HELP1 },
   { STR_DFM_RVC_HELP2                    , STR_TEXT_DFM_RVC_HELP2 },
   { STR_DFM_RVC_HELP3                    , STR_TEXT_DFM_RVC_HELP3 },
   { STR_DFM_RVC_HELP4                    , STR_TEXT_DFM_RVC_HELP4 },
   { STR_DFM_RVC_HELP5                    , STR_TEXT_DFM_RVC_HELP5 },
   { STR_DFM_RVC_HELP6                    , STR_TEXT_DFM_RVC_HELP6 },
   { STR_DFM_RVC_HELP7                    , STR_TEXT_DFM_RVC_HELP7 },
   { STR_DFM_RVC_HELP8                    , STR_TEXT_DFM_RVC_HELP8 },
   { STR_DFM_RVC_HELP9                    , STR_TEXT_DFM_RVC_HELP9 },
   { STR_DFM_RVC_HELP10                   , STR_TEXT_DFM_RVC_HELP10 },
   { STR_DFM_RVC_HELP11                   , STR_TEXT_DFM_RVC_HELP11 },
   { STR_DFM_RVC_HELP12                   , STR_TEXT_DFM_RVC_HELP12 },
   { STR_DFM_RVC_HELP13                   , STR_TEXT_DFM_RVC_HELP13 },
   { STR_DFM_RVC_HELP14                   , STR_TEXT_DFM_RVC_HELP14 },
   { STR_DFM_RVC_HELP15                   , STR_TEXT_DFM_RVC_HELP15 },
   { STR_DFM_RVC_HELP16                   , STR_TEXT_DFM_RVC_HELP16 },
   { STR_DFM_RVC_HELP17                   , STR_TEXT_DFM_RVC_HELP17 },
   { STR_DFM_RVC_HELP18                   , STR_TEXT_DFM_RVC_HELP18 },
   { STR_DFM_RVC_HELP19                   , STR_TEXT_DFM_RVC_HELP19 },
   { STR_DFM_RVC_HELP20                   , STR_TEXT_DFM_RVC_HELP20 },
   { STR_DFM_RVC_HELP21                   , STR_TEXT_DFM_RVC_HELP21 },
   { STR_DFM_RVC_HELP22                   , STR_TEXT_DFM_RVC_HELP22 },
   { STR_DFM_RVC_HELP23                   , STR_TEXT_DFM_RVC_HELP23 },
   { STR_DFM_RVC_HELP24                   , STR_TEXT_DFM_RVC_HELP24 },
   { STR_DFM_RVC_HELP25                   , STR_TEXT_DFM_RVC_HELP25 },
   { STR_DFM_RVC_HELP26                   , STR_TEXT_DFM_RVC_HELP26 },
   { STR_DFM_RVC_HELP27                   , STR_TEXT_DFM_RVC_HELP27 },
   { STR_DFM_RVC_HELP28                   , STR_TEXT_DFM_RVC_HELP28 },
   { STR_DFM_RVC_HELP29                   , STR_TEXT_DFM_RVC_HELP29 },
   { STR_DFM_RVC_HELP30                   , STR_TEXT_DFM_RVC_HELP30 },
   { STR_DFM_RVC_NTV                      , STR_TEXT_DFM_RVC_NTV },
   { STR_DFM_RVC_SELECTED                 , STR_TEXT_DFM_RVC_SELECTED },
   { STR_DFM_RVC_VARIABLE                 , STR_TEXT_DFM_RVC_VARIABLE },
   { STR_DFM_RVC_TIMESTAMP                , STR_TEXT_DFM_RVC_TIMESTAMP },
   { STR_DFM_RVC_VALUE                    , STR_TEXT_DFM_RVC_VALUE },
   { STR_DFM_RVC_VISIBLE                  , STR_TEXT_DFM_RVC_VISIBLE },
   { STR_DFM_RVC_INTERVAL                 , STR_TEXT_DFM_RVC_INTERVAL },
   { STR_DFM_RVC_CB_DISPLAY               , STR_TEXT_DFM_RVC_CB_DISPLAY },
   { STR_DFM_RVC_CB_S1                    , STR_TEXT_DFM_RVC_CB_S1 },
   { STR_DFM_RVC_CB_S2                    , STR_TEXT_DFM_RVC_CB_S2 },
   { STR_DFM_RVC_CB_S3                    , STR_TEXT_DFM_RVC_CB_S3 },
   { STR_DFM_RVC_CB_S4                    , STR_TEXT_DFM_RVC_CB_S4 },
   { STR_DFM_RVC_CB_S5                    , STR_TEXT_DFM_RVC_CB_S5 },
   { STR_DFM_RVC_CB_S6                    , STR_TEXT_DFM_RVC_CB_S6 },
   { STR_DFM_RVC_ALL                      , STR_TEXT_DFM_RVC_ALL },
   { STR_DFM_RVC_SHOW_MARKERS             , STR_TEXT_DFM_RVC_SHOW_MARKERS },
   { STR_DFM_RVC_CURSORS                  , STR_TEXT_DFM_RVC_CURSORS },
   { STR_DFM_RVC_BLACK                    , STR_TEXT_DFM_RVC_BLACK },
   { STR_DFM_RVC_GREY                     , STR_TEXT_DFM_RVC_GREY },
   { STR_DFM_RVC_REFERENCE                , STR_TEXT_DFM_RVC_REFERENCE },
   { STR_DFM_RVC_STATUS                   , STR_TEXT_DFM_RVC_STATUS },
   { STR_DFM_RVC_START_HINT               , STR_TEXT_DFM_RVC_START_HINT },
   { STR_DFM_RVC_START_CAPTION            , STR_TEXT_DFM_RVC_START_CAPTION },
   { STR_DFM_RVC_START_TS_HINT            , STR_TEXT_DFM_RVC_START_TS_HINT },
   { STR_DFM_RVC_START_TS_CAPTION         , STR_TEXT_DFM_RVC_START_TS_CAPTION },
   { STR_DFM_RVC_STOP_HINT                , STR_TEXT_DFM_RVC_STOP_HINT },
   { STR_DFM_RVC_STOP_CAPTION             , STR_TEXT_DFM_RVC_STOP_CAPTION },
   { STR_DFM_RVC_CLEAR_HINT               , STR_TEXT_DFM_RVC_CLEAR_HINT },
   { STR_DFM_RVC_CLEAR_CAPTION            , STR_TEXT_DFM_RVC_CLEAR_CAPTION },
   { STR_DFM_RVC_SAVE_HINT                , STR_TEXT_DFM_RVC_SAVE_HINT },
   { STR_DFM_RVC_SAVE_CAPTION             , STR_TEXT_DFM_RVC_SAVE_CAPTION },
   { STR_DFM_RVC_LOAD_HINT                , STR_TEXT_DFM_RVC_LOAD_HINT },
   { STR_DFM_RVC_LOAD_CAPTION             , STR_TEXT_DFM_RVC_LOAD_CAPTION },
   { STR_DFM_RVC_PRINT_HINT               , STR_TEXT_DFM_RVC_PRINT_HINT },
   { STR_DFM_RVC_PRINT_CAPTION            , STR_TEXT_DFM_RVC_PRINT_CAPTION },
   { STR_DFM_RVC_CLIP_HINT                , STR_TEXT_DFM_RVC_CLIP_HINT },
   { STR_DFM_RVC_CLIP_CAPTION             , STR_TEXT_DFM_RVC_CLIP_CAPTION },
   { STR_DFM_RVC_RCH_FILTER               , STR_TEXT_DFM_RVC_RCH_FILTER },

  //DlgXFLFinished
   { STR_DFM_XFF_CAPTION                  , STR_TEXT_DFM_XFF_CAPTION },
   { STR_DFM_XFF_RG_CAPTION               , STR_TEXT_DFM_XFF_RG_CAPTION },
   { STR_DFM_XFF_RG_ITEM1                 , STR_TEXT_DFM_XFF_RG_ITEM1 },
   { STR_DFM_XFF_RG_ITEM2                 , STR_TEXT_DFM_XFF_RG_ITEM2 },
   { STR_DFM_XFF_RG_ITEM3                 , STR_TEXT_DFM_XFF_RG_ITEM3 },

  //new strings added after V1.0
   { STR_KFX_ERR_OPEN_KSP_FILE            , STR_TEXT_KFX_ERR_OPEN_KSP_FILE },

   { STR_KSP_ALREADY_EXISTS               , STR_TEXT_KSP_ALREADY_EXISTS },
   { STR_DS_ERR_ADDRESS_RESPONSE          , STR_TEXT_DS_ERR_ADDRESS_RESPONSE },
   { STR_FW_ERR_ID_TYPE_NOCHANGE          , STR_TEXT_FW_ERR_ID_TYPE_NOCHANGE },
   { STR_FW_ID_TYPE_CHANGED               , STR_TEXT_FW_ID_TYPE_CHANGED },
   { STR_FW_ERR_NO_XTD                    , STR_TEXT_FW_ERR_NO_XTD },
   { STR_FW_ERR_NO_XTD_CONTINUE           , STR_TEXT_FW_ERR_NO_XTD_CONTINUE },
   { STR_FW_INFO_RESET                    , STR_TEXT_FW_INFO_RESET },
   { STR_DS_ERR_TRANSFER                  , STR_TEXT_DS_ERR_TRANSFER },
   { STR_CM_ERR_INI_WRITE                 , STR_TEXT_CM_ERR_INI_WRITE },
   { STR_DS_ERR_INI_WRITE                 , STR_TEXT_DS_ERR_INI_WRITE },
   { STR_DS_ERR_PROJECT_INVALID           , STR_TEXT_DS_ERR_PROJECT_INVALID },
   { STR_DFM_CS_COMMENT_CAP               , STR_TEXT_DFM_CS_COMMENT_CAP },

   { STR_KSP_READ_ACCESS_DENIED           , STR_TEXT_KSP_READ_ACCESS_DENIED },
   { STR_KSP_WRITE_ACCESS_DENIED          , STR_TEXT_KSP_WRITE_ACCESS_DENIED },

   { STR_VERSION_CHECK                    , STR_TEXT_VERSION_CHECK }

};

uint16 gu16_DIAGLIB_NR_RES_STRNGS = sizeof(gac_DIAG_LIB_RESOURCE_STRINGS) / sizeof(C_SCLResourceString);

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

