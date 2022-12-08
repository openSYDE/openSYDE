//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab Button in material design

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFile>
#include "C_OgePubToolBar.hpp"

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

   \param[in]     ou32_Index  Unique identifier index for the button
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubToolBar::C_OgePubToolBar(QWidget * const opc_Parent, const uint32_t ou32_Index) :
   C_OgePubStandard(opc_Parent),
   mu32_Index(ou32_Index)
{
   connect(this, &C_OgePubToolBar::clicked, this, &C_OgePubToolBar::m_ButtonClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send the index with click signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubToolBar::m_ButtonClicked(void)
{
   // send the index with click signal
   Q_EMIT this->SigClickedWithIndex(this->mu32_Index);
}
