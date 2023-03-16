//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget for selecting modes (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QTabBar>
#include <QVariant>

#include "C_OgeTawSelector.hpp"

#include "C_OgeWiUtil.hpp"

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
C_OgeTawSelector::C_OgeTawSelector(QWidget * const opc_Parent) :
   C_OgeTawToolTipBase(opc_Parent)
{
   //This property holds when expanding is true QTabBar will expand the tabs to use the empty space.
   this->tabBar()->setExpanding(false);
   stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->tabBar(), "C_OgeTawSelector", true);
}
