//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter table model (implementation)

   Parameter table model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QSvgRenderer>
#include <QEvent>

#include "TGLUtils.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_OgeCbxParam.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDpUtil.h"
#include "C_TblTreDelegateUtil.h"
#include "C_SyvDaItPaTreeModel.h"
#include "C_SyvDaItPaTreeDelegate.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaTreeDelegate::C_SyvDaItPaTreeDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mpc_Editor(NULL)
{
   connect(this, &C_SyvDaItPaTreeDelegate::SigNewEditor, this, &C_SyvDaItPaTreeDelegate::m_SetNewEditor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current editor

   \return
   NULL Editor not found
   Else Valid editor
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_SyvDaItPaTreeDelegate::GetEditor(void)
{
   return this->mpc_Editor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten create editor event slot

   Here: Create appropriate editor widget

   \param[in,out] opc_Parent Parent widget
   \param[in]     orc_Option Style options
   \param[in]     orc_Index  Correlating index

   \return
   Editor widget
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_SyvDaItPaTreeDelegate::createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                                const QModelIndex & orc_Index) const
{
   QWidget * pc_Retval = NULL;

   Q_UNUSED(orc_Option)
   if (orc_Index.isValid() == true)
   {
      C_OSCNodeDataPoolListElementId c_Id;
      uint32 u32_ValidLayers;

      C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
      const C_SyvDaItPaTreeModel::E_Columns e_Col = C_SyvDaItPaTreeModel::h_ColumnToEnum(orc_Index.column());
      if (u32_ValidLayers > 2UL)
      {
         QComboBox * pc_ComboBox;
         switch (e_Col)
         {
         case C_SyvDaItPaTreeModel::eSET:
            if (u32_ValidLayers == 4UL)
            {
               pc_Retval = m_CreateEditor(opc_Parent, orc_Index);
            }
            else if (u32_ValidLayers == 3UL)
            {
               const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
                  c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex, c_Id.u32_ListIndex);
               if (pc_List != NULL)
               {
                  pc_ComboBox = new C_OgeCbxParam(opc_Parent);
                  pc_ComboBox->addItem(C_GtGetText::h_GetText("Custom values"));
                  for (uint32 u32_ItDataSet = 0UL; u32_ItDataSet < pc_List->c_DataSets.size(); ++u32_ItDataSet)
                  {
                     const C_OSCNodeDataPoolDataSet & rc_DataSet = pc_List->c_DataSets[u32_ItDataSet];
                     pc_ComboBox->addItem(QString(C_GtGetText::h_GetText("Dataset \"%1\"")).arg(
                                             rc_DataSet.c_Name.c_str()));
                  }
                  pc_Retval = pc_ComboBox;
               }
            }
            else
            {
               //Undefined
            }
            break;
         case C_SyvDaItPaTreeModel::eDEVICE_VALUE:
            //Should only be possible if link
            Q_EMIT this->SigLinkClicked(orc_Index);
            break;
         case C_SyvDaItPaTreeModel::eACTION_READ:
            Q_EMIT this->SigActionRead(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_WRITE:
            Q_EMIT this->SigActionWrite(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_APPLY:
            Q_EMIT this->SigActionApply(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_LOAD:
            Q_EMIT this->SigActionLoad(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_SAVE:
            Q_EMIT this->SigActionSave(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_RECORD:
            Q_EMIT this->SigActionRecord(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_REMOVE:
            Q_EMIT this->SigActionRemove(c_Id, u32_ValidLayers);
            break;
         default:
            break;
         }
      }
      else
      {
         switch (e_Col)
         {
         case C_SyvDaItPaTreeModel::eACTION_READ:
            Q_EMIT this->SigActionRead(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_WRITE:
            Q_EMIT this->SigActionWrite(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_APPLY:
            Q_EMIT this->SigActionApply(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_LOAD:
            Q_EMIT this->SigActionLoad(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_SAVE:
            Q_EMIT this->SigActionSave(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_RECORD:
            Q_EMIT this->SigActionRecord(c_Id, u32_ValidLayers);
            break;
         case C_SyvDaItPaTreeModel::eACTION_REMOVE:
            Q_EMIT this->SigActionRemove(c_Id, u32_ValidLayers);
            break;
         default:
            break;
         }
      }
   }
   Q_EMIT this->SigNewEditor(pc_Retval);
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten set editor data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   if ((opc_Editor != NULL) && (orc_Index.isValid() == true))
   {
      C_OSCNodeDataPoolListElementId c_Id;
      uint32 u32_ValidLayers;

      C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
      if (u32_ValidLayers > 2UL)
      {
         const C_SyvDaItPaTreeModel::E_Columns e_Col = C_SyvDaItPaTreeModel::h_ColumnToEnum(orc_Index.column());
         switch (e_Col)
         {
         case C_SyvDaItPaTreeModel::eSET:
            if (u32_ValidLayers == 4UL)
            {
               C_SdNdeDpUtil::h_SetGenericEditorDataVariable(opc_Editor, orc_Index);
            }
            else if (u32_ValidLayers == 3UL)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               QComboBox * const pc_ComboBox = dynamic_cast<QComboBox * const>(opc_Editor);
               if (pc_ComboBox != NULL)
               {
                  bool q_Ok;
                  pc_ComboBox->setCurrentIndex(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toInt(&q_Ok));
                  tgl_assert(q_Ok == true);
               }
            }
            else
            {
               //Undefined
            }
            break;
         default:
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten set model data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in,out] opc_Model  Model object
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeDelegate::setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                           const QModelIndex & orc_Index) const
{
   if (((opc_Editor != NULL) && (opc_Model != NULL)) && (orc_Index.isValid() == true))
   {
      C_OSCNodeDataPoolListElementId c_Id;
      uint32 u32_ValidLayers;

      C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
      if (u32_ValidLayers > 2UL)
      {
         const C_SyvDaItPaTreeModel::E_Columns e_Col = C_SyvDaItPaTreeModel::h_ColumnToEnum(orc_Index.column());
         switch (e_Col)
         {
         case C_SyvDaItPaTreeModel::eSET:
            if (u32_ValidLayers == 4UL)
            {
               C_SdNdeDpUtil::h_SetModelGenericDataVariable(opc_Editor, opc_Model, orc_Index);
            }
            else if (u32_ValidLayers == 3UL)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               QComboBox * const pc_ComboBox = dynamic_cast<QComboBox * const>(opc_Editor);
               if (pc_ComboBox != NULL)
               {
                  opc_Model->setData(orc_Index, pc_ComboBox->currentIndex());
               }
            }
            else
            {
               //Undefined
            }
            break;
         default:
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Size hint of each index

   \param[in] orc_Option Style options
   \param[in] orc_Index  Index

   \return
   Item size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SyvDaItPaTreeDelegate::sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const
{
   QSize c_Retval = QStyledItemDelegate::sizeHint(orc_Option, orc_Index);

   if (orc_Index.isValid() == true)
   {
      c_Retval.setHeight(20);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paint item

   Here: handle icons

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                    const QModelIndex & orc_Index) const
{
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   C_TblTreDelegateUtil::h_PaintIcon(opc_Painter, orc_Option, orc_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten destroy editor event slot

   Here: Register destruction

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeDelegate::destroyEditor(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   QStyledItemDelegate::destroyEditor(opc_Editor, orc_Index);
   Q_EMIT this->SigNewEditor(NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten event filter slot

   Here: Handle showing popup of combobox. It has a problem with the combination of a delegate in a table on a
   proxy widget in graphics scene
   https://stackoverflow.com/questions/14248036/qcombobox-doesnt-show-its-item-list

   \param[in,out] opc_Object Event sender object
   \param[in,out] opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItPaTreeDelegate::eventFilter(QObject * const opc_Object, QEvent * const opc_Event)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_OgeCbxParam * pc_ComboBox = dynamic_cast<C_OgeCbxParam *>(opc_Object);
   bool q_Return = false;

   if (pc_ComboBox != NULL)
   {
      if (opc_Event->type() == QEvent::MouseButtonRelease)
      {
         pc_ComboBox->showPopup();
         q_Return = true;
      }
   }
   else
   {
      q_Return = QStyledItemDelegate::eventFilter(opc_Object, opc_Event);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set editor

   \param[in] opc_Editor New editor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaTreeDelegate::m_SetNewEditor(QWidget * const opc_Editor)
{
   this->mpc_Editor = opc_Editor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create widget to use for editing this value

   \param[in,out] opc_Parent Parent widget
   \param[in]     orc_Index  Correlating index

   \return
   Editor widget
*/
//----------------------------------------------------------------------------------------------------------------------
QWidget * C_SyvDaItPaTreeDelegate::m_CreateEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index) const
{
   QWidget * pc_Retval = NULL;

   if (orc_Index.isValid() == true)
   {
      C_OSCNodeDataPoolListElementId c_Id;
      uint32 u32_ValidLayers;

      C_SyvDaItPaTreeModel::h_DecodeIndex(orc_Index, c_Id, u32_ValidLayers);
      if (u32_ValidLayers == 4UL)
      {
         //Core data
         const C_OSCNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                       c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
         if (pc_Element != NULL)
         {
            pc_Retval = C_SdNdeDpUtil::h_CreateGenericEditor(opc_Parent, orc_Index, pc_Element->c_MinValue,
                                                                   pc_Element->c_MaxValue, pc_Element->f64_Factor,
                                                                   pc_Element->f64_Offset, 0, true);
            if (pc_Retval == NULL)
            {
               //Send link click
               Q_EMIT this->SigLinkClicked(orc_Index);
            }
         }
      }
   }
   return pc_Retval;
}
