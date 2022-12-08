//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Alternative tab widget (implementation)

   Alternative tab widget.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QTabBar>

#include "constants.hpp"
#include "C_Uti.hpp"
#include "C_OgeTawAlternative.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
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
C_OgeTawAlternative::C_OgeTawAlternative(QWidget * const opc_Parent) :
   QTabWidget(opc_Parent)
{
   this->setUsesScrollButtons(false);
   this->tabBar()->setExpanding(false);

   // need to set font here, because if setting in style sheet, tab width calculation does not work correctly
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   this->tabBar()->setFont(C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_SEMIBOLD_16));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTawAlternative::~C_OgeTawAlternative(void)
{
}
