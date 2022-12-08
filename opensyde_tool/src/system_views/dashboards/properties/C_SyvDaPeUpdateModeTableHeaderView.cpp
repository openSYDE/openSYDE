//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header view for dashboard properties table widget (implementation)

   Header view for dashboard table widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvDaPeUpdateModeTableHeaderView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ore_Orientation Orientation
   \param[in,out] opc_Parent      Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeUpdateModeTableHeaderView::C_SyvDaPeUpdateModeTableHeaderView(const Qt::Orientation & ore_Orientation,
                                                                       QWidget * const opc_Parent) :
   C_SdNdeDpListTableHeaderView(ore_Orientation, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header size

   Here: set custom height

   \return
   Header size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SyvDaPeUpdateModeTableHeaderView::sizeHint(void) const
{
   QSize c_Retval = QHeaderView::sizeHint();

   c_Retval.setHeight(40);

   return c_Retval;
}
