//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCTargetSupportPackage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize all class elements with default values
*/
//-----------------------------------------------------------------------------
C_OSCTSPApplication::C_OSCTSPApplication(void) :
   q_IsProgrammable(true),
   u8_ProcessId(0),
   c_Name(""),
   c_Comment(""),
   c_IdeCall(""),
   q_IsStandardSydeCoderC(true),
   c_CodeGeneratorPath(""),
   c_ProjectFolder(""),
   c_GeneratePath(""),
   c_ResultPath("")
{
}

//-----------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize all class elements with default values
*/
//-----------------------------------------------------------------------------
C_OSCTargetSupportPackage::C_OSCTargetSupportPackage(void) :
   c_DeviceName(""),
   c_Comment(""),
   u8_ApplicationIndex(0U),
   u8_MaxParallelTransmissions(64U),
   u16_MaxMessageBufferTx(585U),
   u16_MaxRoutingMessageBufferRx(585U),
   c_TemplatePath("")
{
   c_Applications.clear();
}

//-----------------------------------------------------------------------------
/*! \brief   Clear elements

   Sets all strings to "" and other values to default.
*/
//-----------------------------------------------------------------------------
void C_OSCTargetSupportPackage::Clear(void)
{
   c_DeviceName = "";
   c_Comment = "";
   c_TemplatePath = "";

   u8_ApplicationIndex = 0U;
   u8_MaxParallelTransmissions = 64U;
   u16_MaxMessageBufferTx = 585U;
   u16_MaxRoutingMessageBufferRx = 585U;

   c_Applications.clear();
}
