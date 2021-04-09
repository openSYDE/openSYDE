//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SyvDaDashboardContentBaseWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent            Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardContentBaseWidget::C_SyvDaDashboardContentBaseWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardContentBaseWidget::~C_SyvDaDashboardContentBaseWidget(void)
{
}
