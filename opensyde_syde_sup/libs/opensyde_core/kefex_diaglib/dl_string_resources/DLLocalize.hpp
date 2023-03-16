#ifndef DLLOCALIZE_H
   #define DLLOCALIZE_H

   //string resource index list
   //used by Windows .rc file AND target independent const table.
   //The .rc format string table is maintained as there are existing applications relying
   // on this format, using Windows translation-tools to path the executable.

   //todo when adding new strings:
   //- add the define here
   //- add the text to VisLibStrings.h
   //- add the set of index and text define to VisLibStrings.rc
   //- add the set of index and text define to the target indepentend const table -> DLStrings.cpp

   //Note: do not add "U" suffix to constants. GCC ressource compiler does not like that.

   //general texts
   #define STR_VERSION_CHECK            0

//   #define STR_BASE_GENERAL             0 //do not add "0" to strings to prevent PC-Lint warning
   #define STR_ERROR                  ( 1)
   #define STR_ERROR_EXCL             ( 2)
   #define STR_INFO                   ( 3)
   #define STR_WARNING                ( 4)
   #define STR_CONFIRMATION           ( 5)
   #define STR_ERR_NO_NUMBER          ( 6)
   #define STR_FILENAME               ( 7)
   #define STR_USE_EXISTING           ( 8)
   #define STR_VALUE                  ( 9)
   #define STR_ERR_FILE_WRITE         (10)
   #define STR_FILE                   (11)
   #define STR_FILESIZE               (12)
   #define STR_TIMESTAMP              (13)
   #define STR_ERR_BITRATE_NOT_INT    (14)
   #define STR_ERR_PROT_WRITE         (15)
   #define STR_ERR_DBS_WRONG          (16)
   #define STR_ERR_NOT_DECIMAL        (17)

   //CKFXTraceFile
   #define STR_BASE_TRACE_FILE        100
   #define STR_TF_ERR_WRITING          (1 + STR_BASE_TRACE_FILE)
   #define STR_TF_ERR_CREATE           (2 + STR_BASE_TRACE_FILE)
   #define STR_TF_ERR_APPEND           (3 + STR_BASE_TRACE_FILE)

   //CPropEditDSList
   #define STR_BASE_PROPEDITDSLIST    120
   #define STR_DS_ERR_NO_DATA_SOURCE   (1 + STR_BASE_PROPEDITDSLIST)
   #define STR_DS_ERR_NO_VALID_LISTS   (2 + STR_BASE_PROPEDITDSLIST)

   //CPropEditFileName
   #define STR_FN_FILTER_ALL_FILES    131

   //DlgCMONMain
   #define STR_BASE_CMONMAIN          140
   #define STR_CM_TIME_ABSOLUTE       ( 1 + STR_BASE_CMONMAIN)
   #define STR_CM_TIME_RELATIVE       ( 2 + STR_BASE_CMONMAIN)
   #define STR_CM_BASE_HEX            ( 3 + STR_BASE_CMONMAIN)
   #define STR_CM_BASE_DEC            ( 4 + STR_BASE_CMONMAIN)
   #define STR_CM_MODE_STATIC         ( 5 + STR_BASE_CMONMAIN)
   #define STR_CM_MODE_CONT           ( 6 + STR_BASE_CMONMAIN)
   #define STR_CM_PROT_OFF            ( 7 + STR_BASE_CMONMAIN)
   #define STR_CM_PROT_ON             ( 8 + STR_BASE_CMONMAIN)
   #define STR_CM_L7_NONE             ( 9 + STR_BASE_CMONMAIN)
   #define STR_CM_L7_CAN_OPEN         (10 + STR_BASE_CMONMAIN)
   #define STR_CM_L7_KEFEX            (11 + STR_BASE_CMONMAIN)
   #define STR_CM_L7_PPP              (12 + STR_BASE_CMONMAIN)
   #define STR_CM_ERR_GET_OS_VER      (13 + STR_BASE_CMONMAIN)
   #define STR_CM_ERR_OS_VER          (14 + STR_BASE_CMONMAIN)
   #define STR_CM_ERR_L7_PROT_INVALID (15 + STR_BASE_CMONMAIN)
   #define STR_CM_ERR_NO_DONGLE       (16 + STR_BASE_CMONMAIN)
   #define STR_CM_ERR_OPEN_DLL        (17 + STR_BASE_CMONMAIN)
   #define STR_CM_ERR_CLOSE_DLL       (18 + STR_BASE_CMONMAIN)
   #define STR_CM_ERR_INIT_CAN        (19 + STR_BASE_CMONMAIN)
   #define STR_CM_ERR_CAN_STATUS      (20 + STR_BASE_CMONMAIN)
   #define STR_CM_CAPTION             (21 + STR_BASE_CMONMAIN)
//   #define STR_CM_CAPTION_PROT        (22 + STR_BASE_CMONMAIN)
   #define STR_CM_TX_ERRORS           (23 + STR_BASE_CMONMAIN)
   #define STR_CM_ERR_PROT_CREATE     (24 + STR_BASE_CMONMAIN)
   #define STR_CM_CAPTION_KFX_PROT    (25 + STR_BASE_CMONMAIN)
   #define STR_CM_PROMPT_KFX_PROT     (26 + STR_BASE_CMONMAIN)
   #define STR_CM_DLL_INFO_MANUF      (27 + STR_BASE_CMONMAIN)
   #define STR_CM_DLL_INFO_DEVICE     (28 + STR_BASE_CMONMAIN)
   #define STR_CM_DLL_INFO_HW_VER     (29 + STR_BASE_CMONMAIN)
   #define STR_CM_DLL_INFO_DLL_VER    (30 + STR_BASE_CMONMAIN)
   #define STR_CM_DLL_INFO_XTD        (31 + STR_BASE_CMONMAIN)
   #define STR_CM_DLL_INFO_NO_XTD     (32 + STR_BASE_CMONMAIN)
   #define STR_CM_CAPTION_ABOUT       (33 + STR_BASE_CMONMAIN)

   //DlgCMONSend
   #define STR_CS_ERR_ID_TOO_GREAT    201
   #define STR_CS_ERR_DLC_TOO_GREAT   202
   #define STR_CS_ERR_DBS_TOO_GREAT   203
   #define STR_CS_ERR_MAX_NUM_MSGS    204
   #define STR_CS_ERR_FILE_READ       205
   #define STR_CS_ERR_NO_FOCUS        206
   #define STR_CS_ERR_ID_WRONG        207
   #define STR_CS_ERR_DLC_WRONG       208
   #define STR_CS_ERR_DBS_WRONG       209
   #define STR_CS_ERR_INTERVAL_WRONG  210
   #define STR_CS_ERR_OFFSET_WRONG    211
   #define STR_CS_ERR_KEY_WRONG       212

   //DlgKFXComSetup
   #define STR_CS_ERR_VALUES_NOT_NUMERIC  241
   #define STR_CS_ERR_INVALID_BASE_ID     242
   #define STR_CS_ERR_VALUE_RANGE         243

   //DlgKFXEditPasswords
   #define STR_EP_ERR_NO_MATCH        251

   //DlgKFXIndexedInputListEditor
   #define STR_IILE_NO_INTEGER        261

   //DlgKFXIndexedListEditor
   #define STR_ILE_VALUE_RANGE        271
   #define STR_ILE_VALUE_WRONG        281

   //DlgKFXIndexedStringEditor
   #define STR_ISE_STRING             291
   #define STR_ISE_VALUE_WRONG        292

   //DlgRAMEditChart
   #define STR_REC_LEGEND             301
   #define STR_REC_BUFFER_OVERRUN     302
   #define STR_REC_MEASUREMENT_STOPPED 303
   #define STR_REC_SERIES             304
   #define STR_REC_ERR_BUFFER_CREATE  305
   #define STR_REC_ERR_BUFFER_WRITE   306
   #define STR_REC_ERR_BUFFER_READ    307
   #define STR_REC_ERR_NO_VALUES      308
   #define STR_REC_ERR_BUFFER_FILE    309
   #define STR_REC_STATUS_READING     310
   #define STR_REC_STATUS_UPDATING    311
   #define STR_REC_MAX_NUM_POINTS     312
   #define STR_REC_WRITTEN            313
   #define STR_REC_VARIABLE           314
   #define STR_REC_DEVICE             315
   #define STR_REC_DATE               316
   #define STR_REC_SENT_TO_PRINTER    317
   #define STR_REC_COPIED_TO_CLIP     318
   #define STR_REC_STOP_TRACE         319
   #define STR_REC_ERR_READ_ONLY      320

   //DlgXFLBBBStream
   #define STR_FB_STW_PASSWD_OK       331
   #define STR_FB_CUST_PASSWD_OK      332
   #define STR_FB_THANKS              333
   #define STR_FB_WRONG_PASSWD        334

   //DlgXFLMain
   //StatusMemo messages
   #define STR_FM_ERR_COMP_ID_LENGTH  341
   #define STR_FM_ERR_WAKEUP_FAILED   342
//   #define STR_FM_ERR_SNR_FORMAT      343
   #define STR_FM_ERR_NODE_NOT_FOUND  344
   #define STR_FM_ERR_DLL             345
   #define STR_FM_ERR_OPEN_CAN        346
//   #define STR_FM_ERR_DLL_NOT_FOUND   347
   #define STR_FM_ERR_LOCALID         348
   #define STR_FM_ERR_COMPANYID       349
   #define STR_FM_ERR_SNR             350
   #define STR_FM_ERR_WAKEUP          351
//   #define STR_FM_ERR_DIVERT_STREAM_OFF  352
   #define STR_FM_ERR_DIVERT_STREAM   353
   #define STR_FM_ERR_WAKEUP_BBB      354
//   #define STR_FM_ERR_HF_UNDEFINED    355
//   #define STR_FM_ERR_HF_LINE_TOO_LONG 356
//   #define STR_FM_ERR_READ_NO_RESPONSE 357
//   #define STR_FM_ERR_READ_INVALID_ADDRESS 358
//   #define STR_FM_ERR_READ_CHECKSUM   359
//   #define STR_FM_ERR_READ_UNDEF      360
   #define STR_FM_ERR_VERIFY_ERR      361
   #define STR_FM_ERR_VERIFY_ABORT    362
//   #define STR_FM_ERR_VERIFY_UNEXP    363
   #define STR_FM_VERIFY_MATCH        364
   #define STR_FM_VERIFY_TOTAL_DIFFERENCES 365
   #define STR_FM_RESETTING           366
   #define STR_FM_TURN_ON             367
   #define STR_FM_RESETTING_S         368
   #define STR_FM_TURN_ON_S           369
   #define STR_FM_CORRECT_CID         370
   #define STR_FM_NO_NODES_FOUND      371
   #define STR_FM_WAKEUP_SNR_OK       372
//   #define STR_FM_COMPARED_LINE       373
   #define STR_FM_DIVERT_STREAM_OFF_OK 374
   #define STR_FM_DIVERT_STREAM_OK    375
   #define STR_FM_WAKEUP_BBB          376
   #define STR_FM_WAKEUP_BBB_OK       377
   #define STR_FM_DIVERT_STREAM_BBB   378
//   #define STR_FM_PREPARING_DL        379
   #define STR_FM_WRITING             380
   #define STR_FM_LINE                381
   #define STR_FM_FINISHED            382
   #define STR_FM_CONNECTED_NODE      383
   #define STR_FM_FLASHING_FINISHED   384
   #define STR_FM_ERR_DISPATCHER_NA   385
   #define STR_FM_ERR_VERSION_MISMATCH 386
   #define STR_FM_ERR_XFL_EXCHANGE    387
   #define STR_FM_XFL_EXCHANGE        388
   #define STR_FM_ERR_FL_PROTOCOL_VERSION 389
   #define STR_FM_FL_PROTOCOL_VERSION 390
   #define STR_FM_ERR_READ_IMPL_INFO  391
   #define STR_FM_ERR_READ_FINGER_PRINT 392
   #define STR_FM_ERR_FINGER_PRINT_NA 393
   #define STR_FM_ERR_USER_ABORT 394
   #define STR_FM_ERR_RD_SNR_CDN_WAKE 395
   #define STR_FM_TXT_SET_PARA_PT1 396
   #define STR_FM_TXT_SET_PARA_PT2 397
   #define STR_FM_ERR_SET_DIVERT_STREAM_PARA 398
   #define STR_FM_ERR_RD_NUM_FLASH_IC 399

   //Messageboxes
   #define STR_FM_COMM_RS232_WARN     401
   #define STR_FM_ERR_INVALID_SNR     402
//   #define STR_FM_ERR_QUEUE_FULL      403
//   #define STR_FM_ERR_TOO_MANY_NODES  404
   #define STR_FM_ERR_NO_RESPONSE     405
//   #define STR_FM_ERR_WRONG_CID       406
//   #define STR_FM_ERR_READ_SNR        407
   #define STR_FM_ERR_MULTI_ID        408
   #define STR_FM_ERR_CREATE_FILE     409
   #define STR_FM_ERR_SEND_ID_NOT_NUMBER 410
   #define STR_FM_ERR_HEX_FILE_NO_EXIST  411
   #define STR_FM_ERR_RD_HF_EOF       412
   #define STR_FM_ERR_RD_HF_LINE_SYNTAX  413
   #define STR_FM_ERR_RD_HF_LINE_CHECKSUM 414
   #define STR_FM_ERR_RD_HF_LINE_COMMAND 415
   #define STR_FM_ERR_RD_HF_MEMORY    416
   #define STR_FM_ERR_RD_HF_OPEN_FILE 417
   #define STR_FM_ERR_RD_HF_UNDEFINED 418
   #define STR_FM_NUM_BYTES_IN_FILE   419
   #define STR_FM_FILE_FILTERED       420
   #define STR_FM_SAVE_CONFIGURATION  421
   #define STR_FM_ENTER_NAME          422

   #define STR_FM_ERR_RD_PSEC_INFO    430
   #define STR_FM_ERR_RD_MEM_SIZE_IC  431
   #define STR_FM_ERR_RD_SEC_OFFSET_IC 432
   #define STR_FM_ERR_RD_MAX_ERASE    433
   #define STR_FM_ERR_RD_MAX_PRG_TIME 434
   #define STR_FM_ERR_RD_NUM_REGIONS  435
   #define STR_FM_ERR_RD_REGION_INFO_IC 436
   #define STR_FM_ERR_TRANSMISSION_ERR 437
   #define STR_FM_ERR_DURING_FLASH_PROG 438
   #define STR_FM_ERR_DURING_DEL      439
   #define STR_FM_ERR_BITRATE         440
   #define STR_FM_ERR_INVALID_PARA    441
   #define STR_FM_ERR_EEP_VERIFY      442
   #define STR_FM_ERR_EEP_READ        443
   #define STR_FM_ERR_DIVERT_STREAM_TRG_GW 444
   #define STR_FM_ERR_HEX_CMD_UNK     445
   #define STR_FM_TXT_NUM_NODES       446
   #define STR_FM_TXT_NODE_WAKE_PT1   447
   #define STR_FM_TXT_NODE_WAKE_PT2   448
   #define STR_FM_TXT_NODE_SNR_PT1    449
   #define STR_FM_TXT_NODE_SNR_PT2    450
   #define STR_FM_TXT_SURP_NODE_AMBIG_ID 451
   #define STR_FM_ERR_DEV_ID_TYPE     452
   #define STR_FM_ERR_WAKE_SNR_PT1    453
   #define STR_FM_ERR_WAKE_SNR_PT2    454
   #define STR_FM_ERR_WAKE_SNR_PT3    455
   #define STR_FM_ERR_RE_LOI          456
   #define STR_FM_ERR_RE_PROG_DATE    457
   #define STR_FM_ERR_RE_PROG_TIME    458
   #define STR_FM_ERR_RE_USER_NAME    459
   #define STR_FM_ERR_RE_CHECKSUM     460
   #define STR_FM_ERR_RE_DEV_INFO_ADDR 461
   #define STR_FM_ERR_IMPL_SERVICES   462
   #define STR_FM_ERR_RD_HEX_REC_SUP_INF 463
   #define STR_FM_ERR_RD_CALC_CHKSM_BLK 464
   #define STR_FM_ERR_WR_RESET_RQ     465

   //other
//   #define STR_FM_FOR_INTERNAL_USE          471
   #define STR_FM_ERR_ONE_FILE_ONLY         472
   #define STR_FM_ERR_NO_HEXFILE            473
   #define STR_FM_ERR_INI_WRITE             474
//   #define STR_FB_WAKEUP_NODE               475
//   #define STR_FB_FLASH_NODE                476
//   #define STR_FB_SEARCH_ALL_NODES          477

   // DlgXFLSearchNodeSnr
//   #define STR_DFM_SNS_CAPTION              490
//   #define STR_DFM_SNS_LOCID_SNR            491
//   #define STR_DFM_SNS_FLASH                492
//   #define STR_DFM_SNS_CHANGE_ID            493
//   #define STR_DFM_SNS_SH_NO_ECU_SELECTED   494
//   #define STR_DFM_SNS_SELECT_ECU           495

   //DlgXFLSearch
   #define STR_BASE_XFLSEARCH               500
   #define STR_FS_ERR_LID_NOT_NUMERIC       ( 1 + STR_BASE_XFLSEARCH)
   #define STR_FS_ERR_LID_WRONG             ( 2 + STR_BASE_XFLSEARCH)
   #define STR_FS_ERR_NO_WAKEUP             ( 3 + STR_BASE_XFLSEARCH)
   #define STR_FS_ERR_WRONG_CID             ( 4 + STR_BASE_XFLSEARCH)
   #define STR_FS_ERR_LID_NOCHANGE          ( 5 + STR_BASE_XFLSEARCH)
   #define STR_FS_MODE_THAN_ONE_NODE        ( 6 + STR_BASE_XFLSEARCH)
   #define STR_FS_INPUT_NEW_LID             ( 7 + STR_BASE_XFLSEARCH)
   #define STR_FS_LOCAL_ID                  ( 8 + STR_BASE_XFLSEARCH)
   #define STR_FS_LOCAL_ID_CHANGED          ( 9 + STR_BASE_XFLSEARCH)

   //DlgXFLWakeup
   #define STR_BASE_XFLWAKEUP               520
   #define STR_FW_ERR_READ_SEC_COUNT        ( 1 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_DEV_ID           ( 2 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_ERASE_TO         ( 3 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_WRITE_TO         ( 4 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_DL_COUNT         ( 5 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_XFL_VER          ( 6 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_LID              ( 7 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_SNR              ( 8 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_BBB_SEC_COUNT    ( 9 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_BBB_XFL_VER      (10 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_BBB_LAST_USER    (11 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_DEV_ID_NOT_SUPP       (12 + STR_BASE_XFLWAKEUP)
//   #define STR_FW_ERR_DIVERT_NOT_SUPP       (13 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_WAKEUP_BBB            (14 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_DIVERT_FAILED         (15 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_DIVERT_OFF_FAILED     (16 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_LID_NOCHANGE          (17 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_SEND_TO_SLEEP         (18 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_NO_RESPONSE           (19 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_NO_SEC_SELECTED       (20 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_SEC_COUNT_IVALID      (21 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERR_READ_CRC              (22 + STR_BASE_XFLWAKEUP)
   #define STR_FW_NUM_SECTORS               (23 + STR_BASE_XFLWAKEUP)
   #define STR_FW_DEV_ID                    (24 + STR_BASE_XFLWAKEUP)
   #define STR_FW_FLASHLOADER_VER           (25 + STR_BASE_XFLWAKEUP)
   #define STR_FW_DOWNLOAD_COUNT            (26 + STR_BASE_XFLWAKEUP)
   #define STR_FW_SNR                       (27 + STR_BASE_XFLWAKEUP)
   #define STR_FW_BBB_LAST_STW              (28 + STR_BASE_XFLWAKEUP)
   #define STR_FW_BBB_LAST_CUST             (29 + STR_BASE_XFLWAKEUP)
   #define STR_FW_BBB_LAST_UNDEF            (30 + STR_BASE_XFLWAKEUP)
   #define STR_FW_BBB_LAST_XXX              (31 + STR_BASE_XFLWAKEUP)
   #define STR_FW_LOCAL_ID_CHANGED          (32 + STR_BASE_XFLWAKEUP)
   #define STR_FW_BITRATE_CHANGED           (33 + STR_BASE_XFLWAKEUP)
   #define STR_FW_SEND_ID_CHANGED           (34 + STR_BASE_XFLWAKEUP)
   #define STR_FW_CRC_WRITTEN               (35 + STR_BASE_XFLWAKEUP)
   #define STR_FW_ERASE_TO_CHANGED          (36 + STR_BASE_XFLWAKEUP)
   #define STR_FW_WRITE_TO_CHANGED          (37 + STR_BASE_XFLWAKEUP)
   #define STR_FW_WARN_CLOSE                (38 + STR_BASE_XFLWAKEUP)
   #define STR_FW_CONFIRM_SET_COMP_MODE     (39 + STR_BASE_XFLWAKEUP)
   #define STR_FW_COMP_MODE_CHANGED         (40 + STR_BASE_XFLWAKEUP)

   //XFL error responses
   #define STR_BASE_XFL_ERR                 570
   #define STR_XFL_ERR_NO_RESPONSE           ( 1 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_SET_BTR               ( 2 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_SET_ID                ( 3 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_INVALID_BITRATE       ( 4 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_INVALID_SECTOR        ( 5 + STR_BASE_XFL_ERR)
   #define STR_XFL_ERR_INVALID_PARAMETER     ( 6 + STR_BASE_XFL_ERR)
   #define STR_XFL_ERR_EEPROM_WRITE          ( 7 + STR_BASE_XFL_ERR)
   #define STR_XFL_ERR_UNDEFINED_CODE        ( 8 + STR_BASE_XFL_ERR)
   #define STR_XFL_ERR_UNDEFINED_ERR         ( 9 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_WAKE_UP_SNR_LONG      (10 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_NUM_SELECTED_ECU      (11 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_NO_INIT_PROGRAM_FLASH (12 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_SET_ONE_ECU_ACTIVE    (13 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_NODE_SLEEP            (14 + STR_BASE_XFL_ERR)
   #define STR_XFL_ERR_READ_LID              (15 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_READ_SNR              (16 + STR_BASE_XFL_ERR)
   #define STR_XFL_ERR_SNR_TO_CANDATA	     (17 + STR_BASE_XFL_ERR)
//   #define STR_XFL_ERR_NUM_ECU_BIG_0         (18 + STR_BASE_XFL_ERR)

   //VisuComponents
   #define STR_VIS_ERR_IWR                  591
   #define STR_VIS_ERR_LOAD_BITMAP          592
   #define STR_VIS_ERR_LOAD                 593
   #define STR_VIS_EXT_NOT_BMP              594
   #define STR_VIS_ANGLE                    595
   #define STR_VIS_SCALES                   596
   #define STR_VIS_EXT_NOT_WAV              597
   #define STR_VIS_ERR_LOAD_WAV             598


   //Data Sources
   #define STR_BASE_DATASOURCES             600
   #define STR_DS_ERR_NO_VALID_NAME_IN_LIST ( 1 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_NO_LISTS_FOUND        ( 2 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_DEF_FILE_NO_EXIST     ( 3 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_OPEN_COMMENTS_FILE    ( 4 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_READ_DATA_VERSION     ( 5 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_WRITE_COMMENTS_FILE   ( 6 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_UNKNOWN_LIST          ( 7 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_DOES_NOT_EXIST        ( 8 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_DOES_NOT_MATCH_DEF    ( 9 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_WILL_BE_DELETED       (10 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_CANNOT_DELETE         (11 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_NO_READ_DUE_TO_LEVEL  (12 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_SENDING_REQUEST       (13 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_NO_RESPONSE           (14 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_ERR_RESPONSE          (15 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_PROJECT_NO_MATCH      (16 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_NUMERIC_TOO_LONG      (17 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_NOT_NUMERIC           (18 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_VALUE_OO_RANGE        (19 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_STRING_LENGTH         (20 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_FALSE_RESPONSE        (21 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_VERIFY                (22 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_USE_WRITE_LIST_CHANGED (23 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_WRITING_CHECKSUM      (24 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_INVALID_VALUE         (25 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_READ_NEW_VALUES       (26 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_ERASE_FILE            (27 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_FILE_NOT_FOUND        (28 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_VIEW_NO_MATCH         (29 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_NOT_FOUND_CONTINUE    (30 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_IS_EMPTY              (31 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_OVERWRITE_OPENED      (32 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_INVALID_PARAM_SIZE    (33 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_INVALID_PARAM_TYPE    (34 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_OVERWRITE_EXISITING   (35 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_CREATE_C_FILE         (36 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_CREATE_H_FILE         (37 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_WRITE_TO_FILE         (38 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_DATA_VER_NOT_NUMERIC  (39 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_NO_BACKUP_POSSIBLE    (40 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_UNKNOWN_LIST_NAME     (41 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_UNKNOWN_CRC_INFO      (42 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_NO_LISTS_FOUND_FILE   (43 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_COMM_FILE_NO_EXISTS   (44 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_COMM_FILE_NO_MATCH    (45 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_COMM_TOO_MANY_LANG    (46 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_COMM_INVALID_LANGUAGE (47 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_REFERENCES_REMOVE     (48 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_IMP_INVALID_TYPE      (49 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_IMP_INVALID_STR_LEN   (50 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_AARGHH                (51 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_NO_RAM_LISTS_FOUND    (52 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_SEND_ABORT_RQ         (53 + STR_BASE_DATASOURCES)
//   #define STR_DS_ERR_ADD_CYCLIC_LIST       (54 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_NO_WRITE_DUE_TO_LEVEL (55 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_UNKNOWN_LIST_NAME_RAM (56 + STR_BASE_DATASOURCES)
   #define STR_DS_ERR_PROJECT_NO_MATCH_DAT  (57 + STR_BASE_DATASOURCES)

   #define STR_DS_SCANNING_FOR_RAM          (72 + STR_BASE_DATASOURCES)

   #define STR_DS_WRITING_COM_FILE          (74 + STR_BASE_DATASOURCES)
   #define STR_DS_WRITING_DATA_VER          (75 + STR_BASE_DATASOURCES)
   #define STR_DS_FINISHED                  (76 + STR_BASE_DATASOURCES)
   #define STR_DS_ERROR_DDD                 (77 + STR_BASE_DATASOURCES)
   #define STR_DS_READING_LIST              (78 + STR_BASE_DATASOURCES)
   #define STR_DS_WRITING_LIST              (79 + STR_BASE_DATASOURCES)
   #define STR_DS_COMPARE_LIST              (80 + STR_BASE_DATASOURCES)
//   #define STR_DS_ADDRESS                   (81 + STR_BASE_DATASOURCES)
   #define STR_DS_UNKNOWN_SIZE              (82 + STR_BASE_DATASOURCES)
   #define STR_DS_UNKNOWN_TYPE              (83 + STR_BASE_DATASOURCES)
   #define STR_DS_UPDATE_OF_TASK            (84 + STR_BASE_DATASOURCES)
   #define STR_DS_CONFIRMED                 (85 + STR_BASE_DATASOURCES)
   #define STR_DS_INVALID_TYPE              (86 + STR_BASE_DATASOURCES)
   #define STR_DS_INVALID_ACC_TYPE          (87 + STR_BASE_DATASOURCES)
//   #define STR_DS_INDEX                     (88 + STR_BASE_DATASOURCES)
   #define STR_DS_READING_COM_FILE          (89 + STR_BASE_DATASOURCES)
   #define STR_DS_READING_DATA_VER          (90 + STR_BASE_DATASOURCES)
   #define STR_DS_WRITING_RAM_FILE          (91 + STR_BASE_DATASOURCES)


   //KFXFlashloader
   #define STR_XFL_ERR_INVALID_RESET_FORMAT 701

   //KFXProcess
   #define STR_BASE_PROCESS                 710
//   #define STR_KFX_NO_DONGLE_PASSWORD       ( 1 + STR_BASE_PROCESS)
//   #define STR_KFX_BGROUP_RANGE             ( 2 + STR_BASE_PROCESS)
//   #define STR_KFX_NEW_DEVICE_NAME          ( 3 + STR_BASE_PROCESS)
   #define STR_KFX_DEF_FILTER               ( 4 + STR_BASE_PROCESS)
   #define STR_KFX_SELECT_FILE_NAME         ( 5 + STR_BASE_PROCESS)
   #define STR_KFX_EXISTS_OVERWRITE         ( 6 + STR_BASE_PROCESS)
   #define STR_KFX_OVERWRITE_ALL            ( 7 + STR_BASE_PROCESS)
   #define STR_KFX_NO_DYNAMIC_LANGUAGE      ( 8 + STR_BASE_PROCESS)
   #define STR_KFX_ERR_INVALID_DEF_FILE     ( 9 + STR_BASE_PROCESS)

   //XFLDownload
   #define STR_BASE_XFL_DOWNLOAD            730
   #define STR_FDL_VERSION_ERR              ( 1 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_VERSION_ERR_NO_SUPP      ( 2 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_VERSION_ERR_UNDEF_CODE   ( 3 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_VERSION_ERR_UNDEF        ( 4 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_DIVERT_ERR_NO_SUPP       ( 5 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_VERSION_BBB_ERR_NO_RESP  ( 6 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_VERSION_BBB_ERR_NO_SUPP  ( 7 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_VERSION_BBB_ERR_UNDEF_CODE ( 8 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_VERSION_BBB_ERR_UNDEF    ( 9 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_C_APPL_NOT_POSSIBLE  (10 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_CANOPEN_NOT_POSSIBLE (11 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_IECAPPL_NOT_POSSIBLE (12 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_IECRTS_NOT_POSSIBLE  (13 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_SEC_ERASE_INVALID    (14 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_WRITING_CRCS         (15 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_FLASHING             (16 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_SEC_ERASE_INV2       (17 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_ERASE                (18 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_ERASE_NO_RESP        (19 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_ERASE_NO_ACT         (20 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_ERASE_NO_INVALID     (21 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_ERASE_NO_CODE        (22 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_ERASE_NO_UNDEF_CODE  (23 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_ERASE_NO_UNDEF       (24 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_WRITE_NO_RESP        (25 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_WRITE_NO_ACT         (26 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_WRITE_CHECKSUM       (27 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_WRITE_INV_COMM       (28 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_WRITE_INV_ADDR       (29 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_WRITE_CODE           (30 + STR_BASE_XFL_DOWNLOAD)
//   #define STR_FDL_ERR_WRITE_UNDEF_CODE     (31 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_WRITE_VERIFY         (32 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_SEC_COUNT_ERR            (33 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_NUM_SECTORS              (34 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_WRITING_CRCS             (35 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_CRCS_NO_SUPP             (36 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_TOTAL_TIME               (37 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_FILE_NOT_FOUND           (38 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_SEC_ERASE                (39 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERASING                  (40 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERASING_SECTOR           (41 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERASE_TIME               (42 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_BYTES                    (43 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_BYTE                     (44 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_DOWNLOAD_TIME            (45 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_AVERAGE_RATE             (46 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_UNKNOWN_ERR              (47 + STR_BASE_XFL_DOWNLOAD)

   #define STR_FDL_DFM_RGSELEC_NODE_RESET   (50 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_DFM_RGSELEC_NET_RESET    (51 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_DFM_RGSELEC_STAY_SLEEP   (52 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_DFM_RGSELEC_NODE_RETURN  (53 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_DFM_RGSELEC_NET_START    (54 + STR_BASE_XFL_DOWNLOAD)

   #define STR_FDL_SBC_SUPPORTED            (56 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_BBC_SUPPORTED            (57 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_CS_DEACTIVATED           (58 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_FL_IMPLEMENTATION    (59 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_HEX_ON_PROTECTED_S   (60 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_HINT_USE_AUTO_MODE       (61 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_ONLY_F_AUTO_UD       (62 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_FILETYPE_AUTO_NA     (63 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_INVALID_HEX_FILE     (64 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_WR_PROG_DATA         (65 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_WR_PROG_TIME         (66 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_RD_USER_NAME         (67 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_WR_USER_NAME         (68 + STR_BASE_XFL_DOWNLOAD)
   #define STR_FDL_ERR_WR_APPL_CHECKS       (69 + STR_BASE_XFL_DOWNLOAD)
   //no more space here :-( continue with 830


   //XFLUpload
   #define STR_BASE_XFL_UPLOAD              800
//   #define STR_FUL_ERR_START_ADDR_INVALID   ( 1 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_ERR_ADDR_OVERLAP         ( 2 + STR_BASE_XFL_UPLOAD)
//   #define STR_FUL_ERR_NO_RESP              ( 3 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_ERR_CHECKSUM             ( 4 + STR_BASE_XFL_UPLOAD)
//   #define STR_FUL_ERR_INVALID_ADDR         ( 5 + STR_BASE_XFL_UPLOAD)
//   #define STR_FUL_ERR_UNDEFINED            ( 6 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_START_ADDR_CAPTION       ( 7 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_START_ADDR_PROMPT        ( 8 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_END_ADDR_CAPTION         ( 9 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_END_ADDR_PROMPT          (10 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_SPARE_IRAM               (11 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_READING_FINISHED         (12 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_READING_FINISHED_BOX     (13 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_ERROR                    (14 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_ADDRESS_RANGE            (15 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_WRITING_TO_FILE          (16 + STR_BASE_XFL_UPLOAD)
   #define STR_FUL_CURRENT_ADDRESS          (17 + STR_BASE_XFL_UPLOAD)

   #define STR_FDL_TXT_REC_USER_N_P1        (30 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_REC_USER_N_P2        (31 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_IGNORE               (32 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_RD_HEX_REC_INFO      (33 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_CHG_HEX_REC_LENGTH   (34 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_DO_CHG_HEX_REC_LENGTH (35 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_ENTER_PROG_MODE      (36 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_WR_FLASH_DO          (37 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_WR_FLASH_DO_PT1      (38 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_WR_FLASH_DO_PT2      (39 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_USER_ABORT           (40 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_WARN_COULD_NOT_CALC_APPL_CHK (41 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_WARN_COULD_NOT_WR_APPL_CHK (42 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_CFG_DIC              (43 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_ID_CHECK_PASSED      (44 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_ID_OF_HEX_FILE       (45 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_ID_CHECKING          (46 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_ID_CHECK_NODE_HEX    (47 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_ID_CHECK_ADDR_NA     (48 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_ID_CHECK_RD_NODE     (49 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_ID_CHECK_ID_ADDR_NA  (50 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_ID_CHECK_ID_NA_HEX   (51 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_ID_CHECK_ID_AMBIG_HEX (52 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_ID_CHECK_ID_UNDEF    (53 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_ID_CHECK_IGNOR       (54 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_TXT_ID_NA_CHECK_HEX_FILE (55 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ID_CHECK_MATCH           (56 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_ID_CHECK_MISMATCH_PT1 (57 + STR_BASE_XFL_UPLOAD)
   #define STR_FDL_ERR_ID_CHECK_MISMATCH_PT2 (58 + STR_BASE_XFL_UPLOAD)

   #define STR_FDL_TXT_WR_FLASH_RQ           (59 + STR_BASE_XFL_UPLOAD)



   //Form texts
   //General Texts
   #define STR_DFM_OK                      1001
   #define STR_DFM_CANCEL                  1002
   #define STR_DFM_MENU_FILE               1003
   #define STR_DFM_MENU_EXIT               1004
   #define STR_DFM_MENU_HELP               1005
   #define STR_DFM_MENU_ABOUT              1006
   #define STR_DFM_YES                     1007
   #define STR_DFM_YES_ALL                 1008

   //DlgCANInterface
   #define STR_DFM_CI_CAPTION              1021
   #define STR_DFM_CI_DLL                  1022
   #define STR_DFM_CI_BITRATE              1023

   //DlgCMONFilter
   #define STR_DFM_CF_CAPTION              1031
   #define STR_DFM_CF_CONFIG               1032
   #define STR_DFM_CF_FILTER1              1033
   #define STR_DFM_CF_FILTER2              1034
   #define STR_DFM_CF_FILTER3              1035
   #define STR_DFM_CF_FILTER4              1036
   #define STR_DFM_CF_MIN                  1037
   #define STR_DFM_CF_MAX                  1038
   #define STR_DFM_CF_APPLY                1039
   #define STR_DFM_CF_TYPE                 1040
   #define STR_DFM_CF_BLOCK                1041
   #define STR_DFM_CF_PASS                 1042

   //DlgCMONMain
   #define STR_BASE_CMON_MAIN_DFM          1100
   #define STR_DFM_CM_HINT_START            ( 1 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_HINT_STOP             ( 2 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_HINT_TIME             ( 3 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_HINT_BASE             ( 4 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_HINT_MODE             ( 5 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_HINT_PROT             ( 6 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_HINT_L7               ( 7 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_ACTIVE                ( 8 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_TX_ERRORS             ( 9 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_FPS_HINT              (10 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_FPS                   (11 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_ASCII_FILTER          (12 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_START_PROT       (13 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_STOP_PROT        (14 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_ACTIONS          (15 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_START            (16 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_STOP             (17 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_CONFIG           (18 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_BUS_PARAMS       (19 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_CAN_DLL          (20 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_DLL_INFO         (21 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_SEND_CONFIG      (22 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_RX_FILTERS       (23 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_CAN_STATUS       (24 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_MENU_PROP_PARAMS      (25 + STR_BASE_CMON_MAIN_DFM)
   #define STR_DFM_CM_HINT_SEND_CONFIG      (26 + STR_BASE_CMON_MAIN_DFM)

   //DlgCMONSend
   #define STR_BASE_CMON_SEND_DFM           1140
   #define STR_DFM_CS_CAPTION               ( 1 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_ADD                   ( 2 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DELETE                ( 3 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_LOAD                  ( 4 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_SAVE                  ( 5 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_CLONE                 ( 6 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_EXIT                  ( 7 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_APPLY                 ( 8 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_TRIGGER               ( 9 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_KEY                   (10 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_INTERVAL_HINT         (11 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_INTERVAL_CAP          (12 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_ACT_HINT              (13 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_ACT_CAP               (14 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_RTR_HINT              (15 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_RTR_CAP               (16 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_XTD_HINT              (17 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_XTD_CAP               (18 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB7_HINT              (19 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB7_CAP               (20 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB6_HINT              (21 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB6_CAP               (22 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB5_HINT              (23 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB5_CAP               (24 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB4_HINT              (25 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB4_CAP               (26 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB3_HINT              (27 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB3_CAP               (28 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB2_HINT              (29 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB2_CAP               (30 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB1_HINT              (31 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB1_CAP               (32 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB0_HINT              (33 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DB0_CAP               (34 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DLC_HINT              (35 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_DLC_CAP               (36 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_ID_HINT               (37 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_ID_CAP                (38 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_OFFSET_HINT           (39 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_OFFSET_CAP            (40 + STR_BASE_CMON_SEND_DFM)
   #define STR_DFM_CS_CSL_FILTER            (41 + STR_BASE_CMON_SEND_DFM)

   //DlgCMONStatus
   #define STR_BASE_CMON_STATUS_DFM         1200
   #define STR_DFM_CST_CAPTION              ( 1 + STR_BASE_CMON_STATUS_DFM)
   #define STR_DFM_CST_GB_CAP               ( 2 + STR_BASE_CMON_STATUS_DFM)
   #define STR_DFM_CST_BOFF                 ( 3 + STR_BASE_CMON_STATUS_DFM)
   #define STR_DFM_CST_BWARN                ( 4 + STR_BASE_CMON_STATUS_DFM)
   #define STR_DFM_CST_TXOVER               ( 5 + STR_BASE_CMON_STATUS_DFM)
   #define STR_DFM_CST_RXOVER               ( 6 + STR_BASE_CMON_STATUS_DFM)
   #define STR_DFM_CST_CLOSE                ( 7 + STR_BASE_CMON_STATUS_DFM)
   #define STR_DFM_CST_CYCLIC               ( 8 + STR_BASE_CMON_STATUS_DFM)

   //DlgKFXCANMessageEditor/DlgXFLResetRQ
   #define STR_BASE_KFXCANMSGEDIT_DFM       1220
   #define STR_DFM_CME_CAPTION              ( 1 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_ID                   ( 2 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_DLC                  ( 3 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_DB0                  ( 4 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_DB1                  ( 5 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_DB2                  ( 6 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_DB3                  ( 7 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_DB4                  ( 8 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_DB5                  ( 9 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_DB6                  (10 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CME_DB7                  (11 + STR_BASE_KFXCANMSGEDIT_DFM)
   #define STR_DFM_CRM_CAPTION              (12 + STR_BASE_KFXCANMSGEDIT_DFM)

   //DlgKFXComSetup
   #define STR_BASE_KFXCOMSETUP             1240
   #define STR_DFM_CSE_CAPTION              ( 1 + STR_BASE_KFXCOMSETUP)
   #define STR_DFM_CSE_PROT                 ( 2 + STR_BASE_KFXCOMSETUP)
//   #define STR_DFM_CSE_BASE_ID              ( 3 + STR_BASE_KFXCOMSETUP)
//   #define STR_DFM_CSE_CLIENT               ( 4 + STR_BASE_KFXCOMSETUP)
//   #define STR_DFM_CSE_SERVER               ( 5 + STR_BASE_KFXCOMSETUP)
   #define STR_DFM_CSE_INTERFACE            ( 6 + STR_BASE_KFXCOMSETUP)
   #define STR_DFM_CSE_HARDWARE             ( 7 + STR_BASE_KFXCOMSETUP)
   #define STR_DFM_CSE_CONFIG               ( 8 + STR_BASE_KFXCOMSETUP)

   //DFM Texts
   #define STR_BASE_XFLMAIN_DFM             1260
   #define STR_DFM_XM_WAKEUPLOCALID_ESX     ( 1 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_READ_FROM_ESX_ESX     ( 2 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_FLASHLOCALID_ESX      ( 3 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_WAKEUPLOCALID_BBB     ( 4 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_READ_FROM_ESX_BBB     ( 5 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_FLASHLOCALID_BBB      ( 6 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_C_APPLICATION         ( 7 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_CANOPEN_CONFIG        ( 8 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_IEC_APPLICATION       ( 9 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_IEC_RTS               (10 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_USER_DEFINED          (11 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_BBB_APPLICATION       (12 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_BBB_PARAMETERS        (13 + STR_BASE_XFLMAIN_DFM)

   #define STR_DFM_XM_CAP_HEXFILE           (14 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_CAPTION               (15 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_CANDLL                (16 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_SELECT                (17 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_VIEW                  (18 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_STARTTIME             (19 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_DELAY                 (20 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_CONFIG                (21 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_DELAY_HINT            (22 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_STIME_HINT            (23 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_EXIT                  (24 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_SEARCH                (25 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_TRIM                  (26 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_WAKEUP_SNR            (27 + STR_BASE_XFLMAIN_DFM)
//   #define STR_DFM_XM_SW_BIN                (28 + STR_BASE_XFLMAIN_DFM)
//   #define STR_DFM_XM_SW_HEX                (29 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_FPARAMS               (30 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_SENDID                (31 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_CID                   (32 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_LID                   (33 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_SNR                   (34 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_BR                    (35 + STR_BASE_XFLMAIN_DFM)
//   #define STR_DFM_XM_SW_BIN_ID             (36 + STR_BASE_XFLMAIN_DFM)
//   #define STR_DFM_XM_SW_HEX_ID             (37 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_SEND_ID_HINT          (38 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_CID_HINT              (39 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_LID_HINT              (40 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_FILETYPE              (41 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_SECTORS               (42 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_RESET_RQ              (43 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_RESET_MSG_HINT        (44 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_RESET_MSG_MSG         (45 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_RESET_MSG_SEND        (46 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_RESET_MSG_CFG         (47 + STR_BASE_XFLMAIN_DFM)
//   #define STR_DFM_XM_DEBUG                 (48 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_FILTER_HEXFILES       (49 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_FILTER_CANDLLS        (50 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_FILTER_HEXFILE        (51 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_HEXFILE_SELECT        (52 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_DLL_SELECT            (53 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_SAVE_CONFIG           (54 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_MENU_ACTIONS          (55 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_VERIFY                (56 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_FLASH_SNR             (57 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_TARGET                (58 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_MAINBOARD             (59 + STR_BASE_XFLMAIN_DFM)
   #define STR_DFM_XM_BBB                   (60 + STR_BASE_XFLMAIN_DFM)

   //DFM Texts
   #define STR_BASE_PASSWORDS_DFM           1340
   #define STR_DFM_EP_CAPTION               ( 1 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_PASSWORD              ( 2 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CONFIRM               ( 3 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_GROUP                 ( 4 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP0             ( 5 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP1             ( 6 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP2             ( 7 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP3             ( 8 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP4             ( 9 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP5             (10 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP6             (11 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP7             (12 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP8             (13 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_GROUP9             (14 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_CB_PROGRAMMER         (15 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_EP_APPLY                 (16 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_PWD_CAPTION              (17 + STR_BASE_PASSWORDS_DFM)
   #define STR_DFM_PWD_GB_CAPTION           (18 + STR_BASE_PASSWORDS_DFM)

   //DlgKFXIndexedInputListEditor
   #define STR_BASE_IILE_DFM                1370
   #define STR_DFM_IILE_CAPTION             ( 1 +  STR_BASE_IILE_DFM)
   #define STR_DFM_IILE_NEW                 ( 2 +  STR_BASE_IILE_DFM)
   #define STR_DFM_IILE_DELETE              ( 3 +  STR_BASE_IILE_DFM)
   #define STR_DFM_IILE_HELP1               ( 4 +  STR_BASE_IILE_DFM)
   #define STR_DFM_IILE_HELP2               ( 5 +  STR_BASE_IILE_DFM)
   #define STR_DFM_IILE_FILTER_BMPS         ( 6 +  STR_BASE_IILE_DFM)

   //DlgKFXIndexedListEditor
   #define STR_BASE_ILE_DFM                 1380
   #define STR_DFM_ILE_CAPTION              ( 1 + STR_BASE_ILE_DFM)
   #define STR_DFM_ILE_HELP1                ( 2 + STR_BASE_ILE_DFM)
   #define STR_DFM_ILE_HELP2                ( 3 + STR_BASE_ILE_DFM)
   #define STR_DFM_ILE_HELP3                ( 4 + STR_BASE_ILE_DFM)
   #define STR_DFM_ILE_HELP4                ( 5 + STR_BASE_ILE_DFM)
   #define STR_DFM_ILE_HELP5                ( 6 + STR_BASE_ILE_DFM)

   //DlgKFXIndexedStringEditor
   #define STR_BASE_ISE_DFM                 1390
   #define STR_DFM_ISE_CAPTION              ( 1 + STR_BASE_ISE_DFM)
   #define STR_DFM_ISE_HELP1                ( 2 + STR_BASE_ISE_DFM)
   #define STR_DFM_ISE_HELP2                ( 3 + STR_BASE_ISE_DFM)

   //DlgKFXWakeup
   #define STR_BASE_XFL_WAKEUP_DFM          1400
   #define STR_DFM_XW_CAPTION               ( 1 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_CONFIG                ( 2 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_SLEEP                 ( 3 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_RETURN                ( 4 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_RESET                 ( 5 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_SET                   ( 6 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_FLASH_CRCS            ( 7 + STR_BASE_XFL_WAKEUP_DFM)
//   #define STR_DFM_XW_CMP                   ( 8 + STR_BASE_XFL_WAKEUP_DFM)
//   #define STR_DFM_XW_SET_CRC               ( 9 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_GET_CRCS              (10 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_TO_FACTORS            (11 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_TO_FACTOR_ERASE       (12 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_TO_FACTOR_WRITE       (13 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_TO_ERASE_HINT         (14 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_TO_WRITE_HINT         (15 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_INFORMATION           (16 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_BBBINFO               (17 + STR_BASE_XFL_WAKEUP_DFM)
   #define STR_DFM_XW_LAST_FLASHED          (18 + STR_BASE_XFL_WAKEUP_DFM)

   //DlgXFLSearch
   #define STR_DFM_XS_CAPTION               1421
   #define STR_DFM_XS_NODE_IDS              1422
//   #define STR_DFM_XS_SNRS                  1423
   #define STR_DFM_XS_CHANGEID              1424

   //DlgXFLBBBDivertStream
   #define STR_DFM_XBB_CAPTION              1431
   #define STR_DFM_XBB_SETTINGS             1432
   #define STR_DFM_XBB_SLOT                 1433
   #define STR_DFM_XBB_PASSWORD             1434

   //DlgRAMEditChart
   #define STR_DFM_REC_BASE                 1440
   #define STR_DFM_RVC_CAPTION              ( 1 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_XAXIS_CAPTION        ( 2 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP1                ( 3 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP2                ( 4 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP3                ( 5 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP4                ( 6 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP5                ( 7 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP6                ( 8 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP7                ( 9 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP8                (10 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP9                (11 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP10               (12 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP11               (13 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP12               (14 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP13               (15 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP14               (16 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP15               (17 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP16               (18 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP17               (19 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP18               (20 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP19               (21 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP20               (22 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP21               (23 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP22               (24 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP23               (25 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP24               (26 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP25               (27 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP26               (28 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP27               (29 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP28               (30 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP29               (31 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_HELP30               (32 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_NTV                  (33 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_SELECTED             (34 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_VARIABLE             (35 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_TIMESTAMP            (36 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_VALUE                (37 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_VISIBLE              (38 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_INTERVAL             (39 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CB_DISPLAY           (40 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CB_S1                (41 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CB_S2                (42 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CB_S3                (43 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CB_S4                (44 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CB_S5                (45 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CB_S6                (46 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_ALL                  (47 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_SHOW_MARKERS         (48 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CURSORS              (49 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_BLACK                (50 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_GREY                 (51 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_REFERENCE            (52 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_STATUS               (53 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_START_HINT           (54 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_START_CAPTION        (55 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_START_TS_HINT        (56 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_START_TS_CAPTION     (57 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_STOP_HINT            (58 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_STOP_CAPTION         (59 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CLEAR_HINT           (60 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CLEAR_CAPTION        (61 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_SAVE_HINT            (62 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_SAVE_CAPTION         (63 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_LOAD_HINT            (64 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_LOAD_CAPTION         (65 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_PRINT_HINT           (66 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_PRINT_CAPTION        (67 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CLIP_HINT            (68 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_CLIP_CAPTION         (69 + STR_DFM_REC_BASE)
   #define STR_DFM_RVC_RCH_FILTER           (70 + STR_DFM_REC_BASE)

   //DlgXFLFinished
   #define STR_DFM_XFF_CAPTION               1541
   #define STR_DFM_XFF_RG_CAPTION            1542
   #define STR_DFM_XFF_RG_ITEM1              1543
   #define STR_DFM_XFF_RG_ITEM2              1544
   #define STR_DFM_XFF_RG_ITEM3              1545

   //new strings added after V1.0
   #define STR_KFX_ERR_OPEN_KSP_FILE        1607
   #define STR_KSP_ALREADY_EXISTS           1608

   #define STR_DS_ERR_ADDRESS_RESPONSE      1611
   #define STR_FW_ERR_ID_TYPE_NOCHANGE      1612
   #define STR_FW_ID_TYPE_CHANGED           1613
   #define STR_FW_ERR_NO_XTD                1614
   #define STR_FW_ERR_NO_XTD_CONTINUE       1615
   #define STR_FW_INFO_RESET                1616
   #define STR_DS_ERR_TRANSFER              1617
   #define STR_CM_ERR_INI_WRITE             1620

   #define STR_DS_ERR_INI_WRITE             1624
   #define STR_DS_ERR_PROJECT_INVALID       1625

   #define STR_DFM_CS_COMMENT_CAP           1627

   #define STR_KSP_READ_ACCESS_DENIED       1628
   #define STR_KSP_WRITE_ACCESS_DENIED      1629
#endif

