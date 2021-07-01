//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE target support package data handling class (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCTARGETSUPPORTPACKAGE_H
#define C_OSCTARGETSUPPORTPACKAGE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCNodeCodeExportSettings.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OSCTSPApplication
{
public:
   C_OSCTSPApplication(void);
   bool q_IsProgrammable;                           ///< true: generate source code files for this app
   stw_types::uint8 u8_ProcessId;                   ///< process ID of application
   stw_scl::C_SCLString c_Name;                     ///< name of application
   stw_scl::C_SCLString c_Comment;                  ///< comment about application
   stw_scl::C_SCLString c_IdeCall;                  ///< invocation of IDE (binary and parameters)
   bool q_IsStandardSydeCoderC;                     ///< true: use standard SYDE Coder C; else: use c_CodeGeneratorPath
   stw_scl::C_SCLString c_CodeGeneratorPath;        ///< alternative file generator
   stw_scl::C_SCLString c_ProjectFolder;            ///< project subfolder (relative to root of template archive)
   stw_scl::C_SCLString c_GeneratePath;             ///< application file generation path (relative to projectfolder)
   stw_types::uint16 u16_GenCodeVersion;            ///< Version of structure of generated files
   std::vector<stw_scl::C_SCLString> c_ResultPaths; ///< list of output-files (relative to projectfolder)
   bool q_GeneratesPsiFiles;                        ///< true: generate HALC PSI file for this app
};

class C_OSCTargetSupportPackage
{
public:
   C_OSCTargetSupportPackage(void);
   stw_scl::C_SCLString c_DeviceName;               ///< name of device this package it intended for
   stw_scl::C_SCLString c_Comment;                  ///< comment about this package
   std::vector<C_OSCTSPApplication> c_Applications; ///< applications contained in template package archive

   stw_types::uint8 u8_ApplicationIndex; ///< Which application is Diagnostic Protocol Driver assigned to?
   ///< zero based
   stw_types::uint8 u8_MaxParallelTransmissions; ///< Maximum number of parallel cyclic/event driven transmissions
   stw_types::uint16 u16_MaxMessageBufferTx;     ///< CAN Tx routing FIFO size
   ///< (number of messages that can be buffered by in Tx direction)
   stw_types::uint16 u16_MaxRoutingMessageBufferRx; ///< CAN Rx routing FIFO size
   ///< (number of messages that can be buffered by in Rx direction)

   C_OSCNodeCodeExportSettings c_CodeExportSettings; ///< General export settings

   stw_scl::C_SCLString c_TemplatePath; ///< path to template package archive

   stw_scl::C_SCLString c_HalcDefPath; ///< path to halc definition file within template package archive
   stw_scl::C_SCLString c_HalcComment; ///< comment on halc definition file included in this package

   void Clear(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
