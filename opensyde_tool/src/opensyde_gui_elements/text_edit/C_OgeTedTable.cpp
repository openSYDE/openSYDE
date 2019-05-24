//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom text edit for table (implementation)

   Custom text edit for table

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>

#include "C_OgeTedTable.h"

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
C_OgeTedTable::C_OgeTedTable(QWidget * const opc_Parent) :
   C_OgeTedContextMenuBase(opc_Parent)
{
   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten focus in event slot

   Here: Select all text

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTedTable::focusInEvent(QFocusEvent * const opc_Event)
{
   QTextEdit::focusInEvent(opc_Event);
   this->selectAll();
}
