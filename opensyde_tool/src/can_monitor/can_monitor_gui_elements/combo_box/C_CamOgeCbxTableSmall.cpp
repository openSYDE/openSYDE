//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box styled for smaller widget usage (implementation)

   Combo box styled for smaller widget usage.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeCbxIconDelegate.hpp"
#include "C_CamOgeCbxTableSmall.hpp"

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
C_CamOgeCbxTableSmall::C_CamOgeCbxTableSmall(QWidget * const opc_Parent) :
   C_OgeCbxTableBase(opc_Parent)
{
   //this code allows to handle the QAbstractItemView::item in stylesheets
   C_OgeCbxIconDelegate * const pc_ItemDelegate = new C_OgeCbxIconDelegate();

   this->setItemDelegate(pc_ItemDelegate);
} //lint !e429  no memory leak because of the parent all elements and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgeCbxTableSmall::~C_CamOgeCbxTableSmall(void)
{
}
