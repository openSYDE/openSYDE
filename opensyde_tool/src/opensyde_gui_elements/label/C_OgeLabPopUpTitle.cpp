//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom label for pop up title (implementation)

   Custom label for pop up title

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeLabPopUpTitle.hpp"

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
C_OgeLabPopUpTitle::C_OgeLabPopUpTitle(QWidget * const opc_Parent) :
   QLabel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: Emit public signal

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabPopUpTitle::mousePressEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigMousePress(opc_Event);

   QLabel::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   Here: Emit public signal

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabPopUpTitle::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigMouseRelease(opc_Event);

   QLabel::mouseReleaseEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: Emit public signal

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabPopUpTitle::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   Q_EMIT this->SigMouseMove(opc_Event);

   QLabel::mouseMoveEvent(opc_Event);
}
