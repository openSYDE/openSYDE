//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Frame with generic border and background color

   Frame with generic border and background color.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeFraGeneric.hpp"
#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

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

   \param[in]  os32_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeFraGeneric::SetBorderColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Border", os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set background color (stylesheet color index)

   \param[in]  os32_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeFraGeneric::SetBackgroundColor(const int32_t os32_Value)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Background", os32_Value);
}
