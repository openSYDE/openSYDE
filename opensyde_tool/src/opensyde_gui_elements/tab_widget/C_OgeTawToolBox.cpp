//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget of toolbox (implementation)

   Tab widget of toolbox.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeTabBar.h"
#include "C_OgeTawToolBox.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

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
C_OgeTawToolBox::C_OgeTawToolBox(QWidget * const opc_Parent) :
   QTabWidget(opc_Parent)
{
   this->setUsesScrollButtons(false);
   this->setTabBar(new C_OgeTabBar(this));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTawToolBox::~C_OgeTawToolBox(void)
{
}
