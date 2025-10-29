//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View (display) component for filter items table view (implementation)

   View (display) component for filter items table view.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QHeaderView>

#include "C_CamMosFilterTableView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

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
C_CamMosFilterTableView::C_CamMosFilterTableView(QWidget * const opc_Parent) :
   C_TblViewToolTipBase(opc_Parent)
{
   this->setItemDelegate(&this->mc_Delegate);

   // model is set externally (see C_CamMosFilterPopup)

   //Configure
   this->setShowGrid(false);
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setMinimumSectionSize(21);
   this->verticalHeader()->setMaximumSectionSize(21);
   this->verticalHeader()->setDefaultSectionSize(21); // after min & max size!

   //Deactivate
   this->verticalHeader()->setVisible(false);

   this->setSelectionBehavior(QAbstractItemView::SelectRows);
   this->setSelectionMode(QAbstractItemView::SingleSelection);

   this->setMinimumHeight(100);
   this->setMaximumHeight(100);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: handle delete key press

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterTableView::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (opc_Event->key() == static_cast<int32_t>(Qt::Key_Delete))
   {
      Q_EMIT (this->SigDeleteKeyPressed());
   }

   C_TblViewToolTipBase::keyPressEvent(opc_Event);
}
