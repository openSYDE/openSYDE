//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for text brower with context menu styled as a simple label

   Class for text brower with context menu styled as a simple label.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeTebLabel.hpp"

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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTebLabel::C_OgeTebLabel(QWidget * const opc_Parent) :
   C_OgeTebContextMenuBase(opc_Parent)
{
   Qt::TextInteractionFlags c_NewFlags;
   c_NewFlags.setFlag(Qt::LinksAccessibleByKeyboard, true);
   c_NewFlags.setFlag(Qt::LinksAccessibleByMouse, true);
   this->setTextInteractionFlags(c_NewFlags);
}
