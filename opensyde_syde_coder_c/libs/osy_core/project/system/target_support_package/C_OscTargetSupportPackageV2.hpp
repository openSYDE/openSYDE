//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE target support package V2 data handling class (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCTARGETSUPPORTPACKAGEV2_HPP
#define C_OSCTARGETSUPPORTPACKAGEV2_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_OscNodeCodeExportSettings.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OscTspApplication
{
public:
   C_OscTspApplication(void);
   bool q_IsProgrammable;                            ///< true: generate source code files for this app
   uint8_t u8_ProcessId;                             ///< process ID of application
   stw::scl::C_SclString c_Name;                     ///< name of application
   stw::scl::C_SclString c_Comment;                  ///< comment about application
   stw::scl::C_SclString c_IdeCall;                  ///< invocation of IDE (binary and parameters)
   bool q_IsStandardSydeCoderCe;                     ///< true: use standard SYDE Coder C; else: use c_CodeGeneratorPath
   stw::scl::C_SclString c_CodeGeneratorPath;        ///< alternative file generator
   stw::scl::C_SclString c_ProjectFolder;            ///< project subfolder (relative to root of template archive)
   stw::scl::C_SclString c_GeneratePath;             ///< application file generation path (relative to projectfolder)
   uint16_t u16_GenCodeVersion;                      ///< Version of structure of generated files
   std::vector<stw::scl::C_SclString> c_ResultPaths; ///< list of output-files (relative to projectfolder)
   bool q_GeneratesPsiFiles;                         ///< true: generate HALC PSI file for this app
};

class C_OscTargetSupportPackageV2
{
public:
   C_OscTargetSupportPackageV2(void);
   stw::scl::C_SclString c_DeviceName;              ///< name of device this package it intended for
   stw::scl::C_SclString c_Comment;                 ///< comment about this package
   std::vector<C_OscTspApplication> c_Applications; ///< applications contained in template package archive

   uint8_t u8_ApplicationIndex; ///< Which application is Diagnostic Protocol Driver assigned to?
   ///< zero based
   uint8_t u8_MaxParallelTransmissions; ///< Maximum number of parallel cyclic/event driven transmissions
   uint16_t u16_MaxMessageBufferTx;     ///< CAN Tx routing FIFO size
   ///< (number of messages that can be buffered by in Tx direction)
   uint16_t u16_MaxRoutingMessageBufferRx; ///< CAN Rx routing FIFO size
   ///< (number of messages that can be buffered by in Rx direction)

   C_OscNodeCodeExportSettings c_CodeExportSettings; ///< General export settings

   stw::scl::C_SclString c_TemplatePath; ///< path to template package archive

   stw::scl::C_SclString c_HalcDefPath; ///< path to halc definition file within template package archive
   stw::scl::C_SclString c_HalcComment; ///< comment on halc definition file included in this package

   void Clear(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
