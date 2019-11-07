//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing already shared datapools (implementation)

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeDpSelectorAddListWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorAddListWidget::C_SdNdeDpSelectorAddListWidget(QWidget * const opc_Parent) :
   QListWidget(opc_Parent)
{
   this->setSelectionMode(QAbstractItemView::NoSelection);
   //set no focus policy to hide focus rectangle of an selected item
   this->setFocusPolicy(Qt::NoFocus);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorAddListWidget::~C_SdNdeDpSelectorAddListWidget(void)
{
}
