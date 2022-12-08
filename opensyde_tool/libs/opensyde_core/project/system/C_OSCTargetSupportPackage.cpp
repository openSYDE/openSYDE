//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE target support package data handling class (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscTargetSupportPackage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize all class elements with default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscTspApplication::C_OscTspApplication(void) :
   q_IsProgrammable(true),
   u8_ProcessId(0),
   c_Name(""),
   c_Comment(""),
   c_IdeCall(""),
   q_IsStandardSydeCoderCe(true),
   c_CodeGeneratorPath(""),
   c_ProjectFolder(""),
   c_GeneratePath(""),
   u16_GenCodeVersion(0),
   q_GeneratesPsiFiles(false)
{
   c_ResultPaths.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize all class elements with default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscTargetSupportPackage::C_OscTargetSupportPackage(void) :
   c_DeviceName(""),
   c_Comment(""),
   u8_ApplicationIndex(0U),
   u8_MaxParallelTransmissions(64U),
   u16_MaxMessageBufferTx(585U),
   u16_MaxRoutingMessageBufferRx(585U),
   c_TemplatePath(""),
   c_HalcDefPath(""),
   c_HalcComment("")
{
   c_Applications.clear();
   c_CodeExportSettings.Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear elements

   Sets all strings to "" and other values to default.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscTargetSupportPackage::Clear(void)
{
   c_DeviceName = "";
   c_Comment = "";
   c_TemplatePath = "";

   u8_ApplicationIndex = 0U;
   u8_MaxParallelTransmissions = 64U;
   u16_MaxMessageBufferTx = 585U;
   u16_MaxRoutingMessageBufferRx = 585U;

   c_Applications.clear();
   c_CodeExportSettings.Initialize();
}
