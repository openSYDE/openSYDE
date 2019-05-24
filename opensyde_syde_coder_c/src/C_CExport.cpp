//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE C source code generation application

   Console application to generate C source code from an openSYDE System Definition

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <iostream>

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCProjectFiler.h"
#include "C_OSCProject.h"
#include "CSCLString.h"
#include "C_OSCSystemDefinition.h"
#include "C_OSCSystemDefinitionFiler.h"
#include "C_OSCExportNode.h"
#include "C_OSCLoggingHandler.h"
#include "TGLFile.h"
#include "C_OsyCodeExportBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CExport :
   public C_OsyCodeExportBase
{
protected:
   virtual E_ResultCode m_CreateApplicationCode(const C_OSCNode & orc_Node, const uint16 ou16_ApplicationIndex,
                                                const C_SCLString & orc_OutputPath,
                                                std::vector<C_SCLString> & orc_CreatedFiles);
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
C_OsyCodeExportBase::E_ResultCode C_CExport::m_CreateApplicationCode(const C_OSCNode & orc_Node,
                                                                     const uint16 ou16_ApplicationIndex,
                                                                     const C_SCLString & orc_OutputPath,
                                                                     std::vector<C_SCLString> & orc_CreatedFiles)
{
   const sint32 s32_Return = C_OSCExportNode::h_CreateSourceCode(orc_Node, ou16_ApplicationIndex, orc_OutputPath,
                                                                 orc_CreatedFiles);

   return (s32_Return == C_NO_ERR) ? eRESULT_OK : eRESULT_CODE_GENERATION_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Print banner of application to console

   Print banner and name of application to console.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CExport::m_PrintBanner(void)
{
   /* from : http://patorjk.com/software/taag/#p=display&f=Slant&t=SYDE Coder C */
   std::cout << "   _______  ______  ______   ______          __             ______\n";
   std::cout << "  / ___/\\ \\/ / __ \\/ ____/  / ____/___  ____/ /__  _____   / ____/\n";
   std::cout << "  \\__ \\  \\  / / / / __/    / /   / __ \\/ __  / _ \\/ ___/  / /\n";
   std::cout << " ___/ /  / / /_/ / /___   / /___/ /_/ / /_/ /  __/ /     / /___\n";
   std::cout << "/____/  /_/_____/_____/   \\____/\\____/\\__,_/\\___/_/      \\____/\n";
   std::cout << "\n";
   std::cout << "openSYDE connector for Code Generation in \"C\"\n";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   main

   \param[in]   osn_Argc     Number of command line arguments
   \param[in]   oapcn_Argv   Command line arguments

   \retval
   0     no problems
   else  trouble (see definition of C_CExport::E_ResultCode constants)
*/
//----------------------------------------------------------------------------------------------------------------------
sintn main(const sintn osn_Argc, charn * const oapcn_Argv[])
{
   C_CExport c_TheCExport;

   C_CExport::E_ResultCode e_Result;

   e_Result = c_TheCExport.Init();

   if (e_Result == C_OsyCodeExportBase::eRESULT_OK)
   {
      e_Result = c_TheCExport.ParseCommandLine(osn_Argc, oapcn_Argv);
   }
   if (e_Result == C_OsyCodeExportBase::eRESULT_OK)
   {
      e_Result = c_TheCExport.LoadSystemDefinition();
   }
   if (e_Result == C_OsyCodeExportBase::eRESULT_OK)
   {
      e_Result = c_TheCExport.GenerateSourceCode();
   }

   e_Result = c_TheCExport.Exit(e_Result);

   return static_cast<uint8>(e_Result);
}
