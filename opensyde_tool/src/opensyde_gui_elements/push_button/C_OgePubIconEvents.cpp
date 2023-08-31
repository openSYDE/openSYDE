//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design without borders, with icon and handle mouseClickEvents

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_OgePubIconEvents.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parameterized constructor
 *
    \param[in,out] opc_Parent      Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubIconEvents::C_OgePubIconEvents(QWidget * const opc_Parent) :
   C_OgePubIconText(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Over written mousePressEvent

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubIconEvents::mousePressEvent(QMouseEvent * const opc_Event)
{
   C_OgePubIconText::mousePressEvent(opc_Event);
   if (opc_Event->button() == Qt::LeftButton)
   {
      if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
      {
         Q_EMIT (this->SigPubIconClicked(true));
      }
      else
      {
         Q_EMIT (this->SigPubIconClicked(false));
      }
   }
}
