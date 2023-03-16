//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE C source code generation application

   Console application to generate C source code from an openSYDE System Definition

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscProjectFiler.hpp"
#include "C_OscProject.hpp"
#include "C_SclString.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscSystemDefinitionFiler.hpp"
#include "C_OscExportNode.hpp"
#include "C_OscLoggingHandler.hpp"
#include "TGLFile.hpp"
#include "C_OsyCodeExportBase.hpp"

#include "C_OscBinaryHash.hpp"
/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CodeExport :
   public C_OsyCodeExportBase
{
protected:
   virtual E_ResultCode m_CreateApplicationCode(const C_OscNode & orc_Node, const uint16_t ou16_ApplicationIndex,
                                                const C_SclString & orc_OutputPath,
                                                std::vector<C_SclString> & orc_CreatedFiles);
   virtual void m_PrintBanner(void);

public:
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Generate source code for one application of one node

   Here: create C and H files
   The caller guarantees ou16_ApplicationIndex to be within valid range and the application to be a
    "programmable application".

   \param[in]     orc_Node               node to generate code for
   \param[in]     ou16_ApplicationIndex  application to generate code for
   \param[in]     orc_OutputPath         code generation output path
   \param[out]    orc_CreatedFiles       list of created files (with paths)

   \return
   eRESULT_OK                        code created for all programmable application
   eRESULT_CODE_GENERATION_ERROR     problems creating code
*/
//----------------------------------------------------------------------------------------------------------------------
C_OsyCodeExportBase::E_ResultCode C_CodeExport::m_CreateApplicationCode(const C_OscNode & orc_Node,
                                                                        const uint16_t ou16_ApplicationIndex,
                                                                        const C_SclString & orc_OutputPath,
                                                                        std::vector<C_SclString> & orc_CreatedFiles)
{
   const C_SclString c_SYDE_CODER_C_VERSION = mc_ExeVersion + ", MD5-Checksum: " +
                                              stw::opensyde_core::C_OscBinaryHash::h_CreateBinaryHash();
   const int32_t s32_Return =
      C_OscExportNode::h_CreateSourceCode(orc_Node, ou16_ApplicationIndex, orc_OutputPath, orc_CreatedFiles,
                                          stw::tgl::TglExtractFileName(mc_ExeName), c_SYDE_CODER_C_VERSION);

   return (s32_Return == C_NO_ERR) ? eRESULT_OK : eRESULT_CODE_GENERATION_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Print banner of application to console

   Print banner and name of application to console.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CodeExport::m_PrintBanner(void)
{
   /* from : http://patorjk.com/software/taag/#p=display&f=Slant&t=SYDE Coder C */
   std::cout << "   _______  ______  ______   ______          __             ______\n";
   std::cout << "  / ___/\\ \\/ / __ \\/ ____/  / ____/___  ____/ /__  _____   / ____/\n";
   std::cout << "  \\__ \\  \\  / / / / __/    / /   / __ \\/ __  / _ \\/ ___/  / /\n";
   std::cout << " ___/ /  / / /_/ / /___   / /___/ /_/ / /_/ /  __/ /     / /___\n";
   std::cout << "/____/  /_/_____/_____/   \\____/\\____/\\__,_/\\___/_/      \\____/\n";
   std::cout << "\n";
   std::cout << "openSYDE connector for Code Generation in \"C\"" << &std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   main

   \param[in]   osn_Argc     Number of command line arguments
   \param[in]   opacn_Argv   Command line arguments

   \retval
   0     no problems
   else  trouble (see definition of C_CodeExport::E_ResultCode constants)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t main(const int32_t os32_Argc, char_t * const opacn_Argv[])
{
   C_CodeExport c_TheCodeExport;

   C_CodeExport::E_ResultCode e_Result;

   e_Result = c_TheCodeExport.Init();

   if (e_Result == C_OsyCodeExportBase::eRESULT_OK)
   {
      e_Result = c_TheCodeExport.ParseCommandLine(os32_Argc, opacn_Argv);
   }
   if (e_Result == C_OsyCodeExportBase::eRESULT_OK)
   {
      e_Result = c_TheCodeExport.LoadSystemDefinition();
   }
   if (e_Result == C_OsyCodeExportBase::eRESULT_OK)
   {
      e_Result = c_TheCodeExport.GenerateSourceCode();
   }

   e_Result = c_TheCodeExport.Exit(e_Result);

   return static_cast<uint8_t>(e_Result);
}
