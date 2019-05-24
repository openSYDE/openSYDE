//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box for font (implementation)

   Custom combo box for font.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"
#include <QAbstractItemView>
#include <QScrollBar>
#include "C_OgeCbxFontProperties.h"

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
C_OgeCbxFontProperties::C_OgeCbxFontProperties(QWidget * const opc_Parent) :
   QFontComboBox(opc_Parent)
{
   // Deactivate custom context menu of scroll bar
   this->view()->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->view()->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}
