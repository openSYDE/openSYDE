//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       short description (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.10.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

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
/*!
   \brief   Constructor

   Initialize all class elements with default values

   \created     02.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_OSCTSPApplication::C_OSCTSPApplication(void) :
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
/*!
   \brief   Constructor

   Initialize all class elements with default values

   \created     01.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_OSCTargetSupportPackage::C_OSCTargetSupportPackage(void) :
   c_DeviceName(""),
   c_Comment(""),
   u8_ApplicationIndex(0U),
   u8_MaxParallelTransmissions(16U),
   u16_MaxMessageBufferTx(512U),
   u16_MaxRoutingMessageBufferRx(512U),
   c_TemplatePath("")
{
   c_Applications.clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clear elements

   Sets all strings to "" and other values to default.

   \created     01.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_OSCTargetSupportPackage::Clear(void)
{
   c_DeviceName = "";
   c_Comment = "";
   c_TemplatePath = "";

   u8_ApplicationIndex = 0U;
   u8_MaxParallelTransmissions = 16U;
   u16_MaxMessageBufferTx = 512U;
   u16_MaxRoutingMessageBufferRx = 512U;

   c_Applications.clear();
}
