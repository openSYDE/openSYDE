//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE X-config generation application

   openSYDE X-config generation application

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <iostream>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_OscProjectFiler.hpp"
#include "C_OscProject.hpp"
#include "C_SclString.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscSystemDefinitionFiler.hpp"
#include "C_OscLoggingHandler.hpp"
#include "TglFile.hpp"
#include "C_XconfigGenExportBase.hpp"
#include "C_OscXcoCreate.hpp"

#include "C_OscBinaryHash.hpp"
/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_XconfigGenExport :
   public C_XconfigGenExportBase
{
protected:
   virtual E_ResultCode m_CreateApplicationCode(const C_OscNode & orc_Node,
                                                const C_OscSystemDefinition & orc_SystemDefinition,
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
/*! \brief   Generate X-Config for one application of one node

   \param[in]   orc_Node               node to generate code for
   \param[in]   orc_SystemDefinition   System definition
   \param[in]   orc_OutputPath         X-Config output path
   \param[out]  orc_CreatedFiles       list of created files (with paths)

   \return
   eRESULT_OK                        X-Config created for all programmable application
   eRESULT_CODE_GENERATION_ERROR     problems creating code
*/
//----------------------------------------------------------------------------------------------------------------------
C_XconfigGenExportBase::E_ResultCode C_XconfigGenExport::m_CreateApplicationCode(const C_OscNode & orc_Node,
                                                                                 const C_OscSystemDefinition & orc_SystemDefinition, const C_SclString & orc_OutputPath,
                                                                                 std::vector<C_SclString> & orc_CreatedFiles)
{
   int32_t s32_Return;

   s32_Return = C_OscUtils::h_CreateFolderRecursively(orc_OutputPath);
   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Creating X-Config Package", "Could not create target directory \"" + orc_OutputPath + "\".");
      s32_Return = C_RD_WR;
   }
   else
   {
      C_OscXcoManifest c_Manifest;
      C_SclStringList c_Warnings;
      C_SclString c_Error;
      const C_SclString c_Path = TglFileIncludeTrailingDelimiter(orc_OutputPath) + "x_app.syde_xcfg";

      c_Manifest.c_NodeName = orc_Node.c_Properties.c_Name;
      s32_Return =
         C_OscXcoCreate::h_CreatePackage(c_Path, orc_SystemDefinition, c_Manifest, c_Warnings, c_Error);
      orc_CreatedFiles.push_back("x_app.syde_xcfg");
   }
   return (s32_Return == C_NO_ERR) ? eRESULT_OK : eRESULT_X_CONFIG_GENERATION_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Print banner of application to console

   Print banner and name of application to console.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XconfigGenExport::m_PrintBanner(void)
{
   /* from : http://patorjk.com/software/taag/#p=display&f=Slant&t=SYDE X Gen */
   std::cout << "   _______  ______  ______   _  __    ______\n";
   std::cout << "  / ___/\\ \\/ / __ \\/ ____/  | |/ /   / ____/__  ____\n";
   std::cout << "  \\__ \\  \\  / / / / __/     |   /   / / __/ _ \\/ __ \\\n";
   std::cout << " ___/ /  / / /_/ / /___    /   |   / /_/ /  __/ / / /\n";
   std::cout << "/____/  /_/_____/_____/   /_/|_|   \\____/\\___/_/ /_/\n";
   std::cout << "\n";

   std::cout << "openSYDE connector for X Config Generation" << &std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   main

   \param[in]  os32_Argc   Number of command line arguments
   \param[in]  opacn_Argv  Command line arguments

   \retval
   0     no problems
   else  trouble (see definition of C_XconfigGenExport::E_ResultCode constants)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t main(const int32_t os32_Argc, char_t * const opacn_Argv[])
{
   C_XconfigGenExport c_TheCodeExport;

   C_XconfigGenExport::E_ResultCode e_Result;

   e_Result = c_TheCodeExport.Init(os32_Argc, opacn_Argv);

   if (e_Result == C_XconfigGenExportBase::eRESULT_OK)
   {
      e_Result = c_TheCodeExport.ParseCommandLine(os32_Argc, opacn_Argv);
   }
   if (e_Result == C_XconfigGenExportBase::eRESULT_OK)
   {
      e_Result = c_TheCodeExport.LoadSystemDefinition();
   }
   if (e_Result == C_XconfigGenExportBase::eRESULT_OK)
   {
      e_Result = c_TheCodeExport.GenerateSourceCode();
   }

   e_Result = c_TheCodeExport.Exit(e_Result);

   return static_cast<uint8_t>(e_Result);
}
