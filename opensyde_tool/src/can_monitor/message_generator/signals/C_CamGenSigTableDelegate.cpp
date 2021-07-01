//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for message generator signals table (implementation)

   Delegate for message generator signals table

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_CamOgeCbxTable.h"
#include "C_CamGenSigTableDelegate.h"

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
/*! \brief  Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamGenSigTableDelegate::C_CamGenSigTableDelegate(QObject * const opc_Parent) :
   C_CamTblDelegate(opc_Parent)
{
   this->mq_InitialSelection = false;
   //Explicit queued connection to invalidate selection
   connect(this, &C_CamGenSigTableDelegate::SigAfterSetEditorData, this,
           &C_CamGenSigTableDelegate::m_AfterSetEditorData,
           Qt::QueuedConnection);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten set editor data event slot

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   C_TblEditLineEditBase * pc_LineEdit;

   //Call base for actual functionality
   C_CamTblDelegate::setEditorData(opc_Editor, orc_Index);

   pc_LineEdit = dynamic_cast<C_TblEditLineEditBase *>(opc_Editor);
   if (pc_LineEdit != NULL)
   {
      //Delayed selection handling
      Q_EMIT (this->SigAfterSetEditorData(pc_LineEdit));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create generic combo box interaction element

   \param[in,out] opc_Parent Optional pointer to parent

   \return
   Generic combo box interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxTableBase * C_CamGenSigTableDelegate::m_CreateComboBox(QWidget * const opc_Parent) const
{
   C_CamOgeCbxTable * const pc_Combobox = new C_CamOgeCbxTable(opc_Parent);

   return pc_Combobox;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle delayed widget selection clear

   \param[in,out] opc_Widget Widget to clear the selection for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamGenSigTableDelegate::m_AfterSetEditorData(QLineEdit * const opc_Widget) const
{
   //Set selection to line edit end
   opc_Widget->deselect();
}
