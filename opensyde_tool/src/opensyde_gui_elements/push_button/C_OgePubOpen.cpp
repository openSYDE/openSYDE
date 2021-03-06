//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard open button (implementation)

   Standard open button for file browse action with "..." icon.
   Left border is styled for seamless transition to file path line edit.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgePubOpen.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubOpen::C_OgePubOpen(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent)
{
   this->setIcon(QIcon("://images/IconBrowse.svg"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set background color (stylesheet color index)

   \param[in] osn_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubOpen::SetBackgroundColor(const sintn osn_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Background", osn_Value);
}
