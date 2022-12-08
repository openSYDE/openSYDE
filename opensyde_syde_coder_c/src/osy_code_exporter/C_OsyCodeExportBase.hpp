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

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscSystemDefinition.hpp"

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

   static stw::scl::C_SclString h_GetApplicationVersion(const stw::scl::C_SclString & orc_FileName);

   virtual ~C_OsyCodeExportBase(void);

   E_ResultCode Init(void);
   E_ResultCode ParseCommandLine(const int32_t os32_Argc, char_t * const opacn_Argv[]);
   E_ResultCode LoadSystemDefinition(void);
   E_ResultCode GenerateSourceCode(void);
   E_ResultCode Exit(const E_ResultCode oe_ResultCode);

private:
   void m_PrintCommandLineParameters(void) const;
   E_ResultCode m_CreateNodeCode(const stw::opensyde_core::C_OscNode & orc_Node,
                                 const stw::scl::C_SclString & orc_OutputPath);
   void m_PrintCodeCreationInformation(const stw::scl::C_SclString & orc_NodeName,
                                       const stw::opensyde_core::C_OscNodeApplication & orc_Application,
                                       const bool oq_GenerationSuccessful,
                                       std::vector<stw::scl::C_SclString> & orc_CreatedFiles);
   void m_PrintCodeFormatUnknownInfo(const stw::scl::C_SclString & orc_NodeName,
                                     const stw::opensyde_core::C_OscNodeApplication & orc_Application);

protected:
   //parameters from command line:
   stw::scl::C_SclString mc_SystemDefinitionFilePath;
   stw::scl::C_SclString mc_OutputPath;
   stw::scl::C_SclString mc_DeviceName;      //name of device for which to create code ("" -> all)
   stw::scl::C_SclString mc_ApplicationName; //name of application for which to create code ("" -> all)
   bool mq_EraseTargetFolder;

   //parsed system definition
   stw::opensyde_core::C_OscSystemDefinition mc_SystemDefinition;

   //application executable information (set by ::Init)
   stw::scl::C_SclString mc_ExeName;
   stw::scl::C_SclString mc_ExeVersion; //Format: V<M>.<mm>r<r>
   stw::scl::C_SclString mc_LogFileName;
   stw::scl::C_SclString mc_ListOfFilesFileName;
   stw::scl::C_SclString mc_BinaryHash;

   //list of created source code files; to be filled by GenerateSourceCode
   std::vector<stw::scl::C_SclString> mc_CreatedFiles;

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
   virtual E_ResultCode m_CreateApplicationCode(const stw::opensyde_core::C_OscNode & orc_Node,
                                                const uint16_t ou16_ApplicationIndex,
                                                const stw::scl::C_SclString & orc_OutputPath,
                                                std::vector<stw::scl::C_SclString> & orc_CreatedFiles) = 0;

   //-------------------------------------------------------------------------------------------------------------------
   /*!
      \brief   Print banner and application name to console
   */
   //-------------------------------------------------------------------------------------------------------------------
   virtual void m_PrintBanner(void) = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */

#endif
