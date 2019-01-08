//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data class, containing the update package configuration (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SyvUpUpdatePackageConfig.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     20.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageConfigNodeApp::C_SyvUpUpdatePackageConfigNodeApp() :
   e_Type(stw_opensyde_core::C_OSCNodeApplication::eBINARY),
   c_Name(""),
   c_Path("")
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     20.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageConfigNode::C_SyvUpUpdatePackageConfigNode() :
   c_Name(""),
   c_DeviceType("")
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     20.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpUpdatePackageConfig::C_SyvUpUpdatePackageConfig()
{
}
