//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic line edit for table edit style for can monitor (implementation)

   Generic line edit for table edit style for can monitor

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_CamOgeLeTableEdit.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
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
C_CamOgeLeTableEdit::C_CamOgeLeTableEdit(QWidget * const opc_Parent) :
   C_TblEditLineEditBase(opc_Parent)
{
}
