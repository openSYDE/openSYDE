//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for openSYDE source code generation command line tools.

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSYCODEEXPORTBASEH
#define C_OSYCODEEXPORTBASEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCSystemDefinition.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OsyCodeExportBase
{
public:
   enum E_ResultCode
   {
      eRESULT_OK = 0,
      eRESULT_HELPING = 10,                         //command line switch "--help" detected (no operation performed)
      eRESULT_ERASE_FILE_LIST_ERROR = 11,           //could not remove pre-existing file list file
      eRESULT_WRITE_FILE_LIST_ERROR = 12,           //could not write file list file
      eRESULT_ERASE_TARGET_FOLDER_ERROR = 13,       //could not erase pre-existing target folder
      eRESULT_CREATE_TARGET_FOLDER_ERROR = 14,      //could not create target folder
      eRESULT_INVALID_CLI_PARAMETERS = 20,          //invalid command line parameters
      eRESULT_SYSTEM_DEFINITION_OPEN_ERROR = 30,    //specified system definition could not be loaded
      eRESULT_CODE_GENERATION_ERROR = 40,           //could not generate code for at least one application
      eRESULT_DEVICE_NOT_FOUND = 41,                //device specified on command line does not exist
      eRESULT_DEVICE_NOT_COMPATIBLE = 42,           //device specified on command line does not support openSYDE
      eRESULT_APPLICATION_NOT_FOUND = 43,           //application specified on command line does not exist
      eRESULT_APPLICATION_NOT_PROGRAMMABLE = 44,    //application specified on command line is not defined as
                                                    // "programmable"
      eRESULT_APPLICATION_UNKNOWN_CODE_VERSION = 45 //application has unknown code format version
                                                    // 9009 is reserved for batch files (executable wrapped in batch
                                                    // script not found)
   };

   static stw_scl::C_SCLString h_GetApplicationVersion(const stw_scl::C_SCLString & orc_FileName);

   virtual ~C_OsyCodeExportBase(void);

   E_ResultCode Init(void);
   E_ResultCode ParseCommandLine(const stw_types::sintn osn_Argc, stw_types::charn * const oapcn_Argv[]);
   E_ResultCode LoadSystemDefinition(void);
   E_ResultCode GenerateSourceCode(void);
   E_ResultCode Exit(const E_ResultCode oe_ResultCode);

private:
   void m_PrintCommandLineParameters(void) const;
   E_ResultCode m_CreateNodeCode(const stw_opensyde_core::C_OSCNode & orc_Node,
                                 const stw_scl::C_SCLString & orc_OutputPath);
   void m_PrintCodeCreationInformation(const stw_scl::C_SCLString & orc_NodeName,
                                       const stw_opensyde_core::C_OSCNodeApplication & orc_Application,
                                       const bool oq_GenerationSuccessful,
                                       std::vector<stw_scl::C_SCLString> & orc_CreatedFiles);
   void m_PrintCodeFormatUnknownInfo(const stw_scl::C_SCLString & orc_NodeName,
                                     const stw_opensyde_core::C_OSCNodeApplication & orc_Application);

protected:
   //parameters from command line:
   stw_scl::C_SCLString mc_SystemDefinitionFilePath;
   stw_scl::C_SCLString mc_OutputPath;
   stw_scl::C_SCLString mc_DeviceName;      //name of device for which to create code ("" -> all)
   stw_scl::C_SCLString mc_ApplicationName; //name of application for which to create code ("" -> all)
   bool mq_EraseTargetFolder;

   //parsed system definition
   stw_opensyde_core::C_OSCSystemDefinition mc_SystemDefinition;

   //application executable information (set by ::Init)
   stw_scl::C_SCLString mc_ExeName;
   stw_scl::C_SCLString mc_ExeVersion; //Format: V<M>.<mm>r<r>
   stw_scl::C_SCLString mc_LogFileName;
   stw_scl::C_SCLString mc_ListOfFilesFileName;

   //list of created source code files; to be filled by GenerateSourceCode
   std::vector<stw_scl::C_SCLString> mc_CreatedFiles;

   //-------------------------------------------------------------------------------------------------------------------
   /*!
      \brief   Generate source code for one application of one node

      The caller guarantees ou16_ApplicationIndex to be within valid range and the application to be a
       "programmable application".

      \param[in]     orc_Node               node to generate code for
      \param[in]     ou16_ApplicationIndex  application to generate code for
      \param[in]     orc_OutputPath         code generation output path
      \param[out]    orc_CreatedFiles       list of created files (with paths)

      \return
      eRESULT_OK                        code created for all programmable application
      eRESULT_CODE_GENERATION_ERROR     problems creating code
      Do not return
   */
   //-------------------------------------------------------------------------------------------------------------------
   virtual E_ResultCode m_CreateApplicationCode(const stw_opensyde_core::C_OSCNode & orc_Node,
                                                const stw_types::uint16 ou16_ApplicationIndex,
                                                const stw_scl::C_SCLString & orc_OutputPath,
                                                std::vector<stw_scl::C_SCLString> & orc_CreatedFiles) = 0;

   //-------------------------------------------------------------------------------------------------------------------
   /*!
      \brief   Print banner and application name to console
   */
   //-------------------------------------------------------------------------------------------------------------------
   virtual void m_PrintBanner(void) = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
