//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit variant to be integrated into a combo box (implementation)

   Line edit variant to be integrated into a combo box

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QComboBox>
#include "C_OgeLeComboBox.hpp"
#include "C_OgeCbxText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

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
C_OgeLeComboBox::C_OgeLeComboBox(QWidget * const opc_Parent) :
   C_OgeLeContextMenuBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: Trigger combo box

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeComboBox::mousePressEvent(QMouseEvent * const opc_Event)
{
   C_OgeCbxText * const pc_Parent = dynamic_cast<C_OgeCbxText *>(this->parent());

   C_OgeLeContextMenuBase::mousePressEvent(opc_Event);

   //Trigger combo box pop up
   if (pc_Parent != NULL)
   {
      pc_Parent->SuppressHide(); // suppress call to hidePopup (called because of "outside of combo box")
      pc_Parent->showPopup();
   }
}
