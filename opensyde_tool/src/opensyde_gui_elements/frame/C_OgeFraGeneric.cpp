//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Frame with generic border and background color

   Frame with generic border and background color.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeFraGeneric.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeFraGeneric::C_OgeFraGeneric(QWidget * const opc_Parent) :
   QFrame(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set border color (stylesheet color index)

   \param[in]  osn_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeFraGeneric::SetBorderColor(const stw_types::sintn osn_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Border", osn_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set background color (stylesheet color index)

   \param[in]  osn_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeFraGeneric::SetBackgroundColor(const stw_types::sintn osn_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Background", osn_Value);
}
