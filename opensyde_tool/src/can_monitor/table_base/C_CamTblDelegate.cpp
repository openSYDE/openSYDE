//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate (interaction) component base for CAN monitor table (implementation)

   Delegate (interaction) component base for any table

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_CamTblDelegate.h"
#include "C_CamOgeLeTableEdit.h"
#include "C_CamOgeCbxTableSmall.h"
#include "C_CamOgeWiSpinBoxGroupTable.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamTblDelegate::C_CamTblDelegate(QObject * const opc_Parent) :
   C_TblDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create CAN monitor combo box interaction element

   \param[in,out]  opc_Parent    Optional pointer to parent

   \return
   CAN monitor combo box interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxTableBase * C_CamTblDelegate::m_CreateComboBox(QWidget * const opc_Parent) const
{
   return new C_CamOgeCbxTableSmall(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create multi select combo box

   \param[in,out]  opc_Parent    Parent

   \return
   CAN monitor multi select combo box interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxMultiSelect * C_CamTblDelegate::m_CreateMultiSelectComboBox(QWidget * const opc_Parent) const
{
   return new C_OgeCbxMultiSelect(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create CAN monitor line edit interaction element

   \param[in,out]  opc_Parent    Optional pointer to parent

   \return
   CAN monitor line edit interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblEditLineEditBase * C_CamTblDelegate::m_CreateLineEdit(QWidget * const opc_Parent) const
{
   return new C_CamOgeLeTableEdit(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create CAN monitor spin box interaction element

   \param[in,out]  opc_Parent    Optional pointer to parent

   \return
   CAN monitor spin box interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiSpinBoxGroup * C_CamTblDelegate::m_CreateSpinBox(QWidget * const opc_Parent) const
{
   return new C_CamOgeWiSpinBoxGroupTable(opc_Parent);
}
