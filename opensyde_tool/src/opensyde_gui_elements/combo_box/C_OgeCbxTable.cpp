//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box for table (implementation)

   Custom combo box for table.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeWiUtil.hpp"
#include "C_OgeCbxTable.hpp"
#include "C_OgeCbxIconDelegate.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_OgeCbxTable::C_OgeCbxTable(QWidget * const opc_Parent) :
   C_OgeCbxResizingView(opc_Parent)
{
   //this code allows to handle the QAbstractItemView::item in stylesheets
   C_OgeCbxIconDelegate * const pc_ItemDelegate = new C_OgeCbxIconDelegate();

   this->setItemDelegate(pc_ItemDelegate);
} //lint !e429  //no memory leak because of the parent all elements and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxTable::~C_OgeCbxTable(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Explicitly ignore events which are globally handled (Won't be forwarded otherwise!)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxTable::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (C_OgeWiUtil::h_CheckGlobalKey(opc_Event) == true)
   {
      opc_Event->ignore();
   }
   else
   {
      C_OgeCbxResizingView::keyPressEvent(opc_Event);
   }
}
