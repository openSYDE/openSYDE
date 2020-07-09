//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list table model (implementation)

   Node data pool list table model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QMimeData>
#include <QIcon>

#include "C_SdNdeDpContentUtil.h"
#include "C_SdNdeDpListTableModel.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_PuiSdHandler.h"
#include "C_GtGetText.h"
#include "C_SdClipBoardHelper.h"
#include "C_Uti.h"
#include "C_SdUtil.h"
#include "C_NagToolTip.h"
#include "C_SdNdeUnoUtil.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_tgl;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListTableModel::C_SdNdeDpListTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mu32_ListIndex(0)
{
   connect(&this->mc_ErrorManager, &C_SdNdeDpListTableErrorManager::SigErrorChange, this,
           &C_SdNdeDpListTableModel::m_OnErrorChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated list

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::SetList(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                      const uint32 & oru32_ListIndex)
{
   beginResetModel();
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->mu32_ListIndex = oru32_ListIndex;
   endResetModel();
   this->mc_ErrorManager.Init(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if equal

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::Equals(const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex,
                                     const uint32 & oru32_ListIndex) const
{
   bool q_Retval;

   if (((this->mu32_NodeIndex == oru32_NodeIndex) && (this->mu32_DataPoolIndex == oru32_DataPoolIndex)) &&
       (this->mu32_ListIndex == oru32_ListIndex))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  osn_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  osn_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpListTableModel::headerData(const sintn osn_Section, const Qt::Orientation oe_Orientation,
                                             const sintn osn_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(osn_Section, oe_Orientation, osn_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      sint32 s32_DataSetIndex;
      const C_SdNdeDpListTableModel::E_Columns e_Col = ColumnToEnum(osn_Section, &s32_DataSetIndex);
      if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eINVALID:
            c_Retval = "";
            break;
         case eINDEX:
            c_Retval = "#";
            break;
         case eICON:
            c_Retval = "";
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eVALUE_TYPE:
            c_Retval = C_GtGetText::h_GetText("Value type");
            break;
         case eARRAY_SIZE:
            c_Retval = C_GtGetText::h_GetText("Array size");
            break;
         case eAUTO_MIN_MAX:
            c_Retval = C_GtGetText::h_GetText("Auto\nMin/Max");
            break;
         case eMIN:
            c_Retval = C_GtGetText::h_GetText("Min");
            break;
         case eMAX:
            c_Retval = C_GtGetText::h_GetText("Max");
            break;
         case eFACTOR:
            c_Retval = C_GtGetText::h_GetText("Factor");
            break;
         case eOFFSET:
            c_Retval = C_GtGetText::h_GetText("Offset");
            break;
         case eUNIT:
            c_Retval = C_GtGetText::h_GetText("Unit");
            break;
         case eDATA_SET:
            if (s32_DataSetIndex >= 0)
            {
               const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
                  this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
               if (pc_List != NULL)
               {
                  if (static_cast<uint32>(s32_DataSetIndex) < pc_List->c_DataSets.size())
                  {
                     c_Retval = QString(C_GtGetText::h_GetText("Dataset -\n")) +
                                QString(pc_List->c_DataSets[s32_DataSetIndex].c_Name.c_str());
                  }
               }
            }
            break;
         case eACCESS:
            c_Retval = C_GtGetText::h_GetText("Access");
            break;
         case eDATA_SIZE:
            c_Retval = C_GtGetText::h_GetText("Data size");
            break;
         case eADDRESS:
            c_Retval = C_GtGetText::h_GetText("Address");
            break;
         case eEVENT_CALL:
            c_Retval = C_GtGetText::h_GetText("Event call");
            break;
         case eUNKNOWN:
         default:
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eINVALID:
            //no tooltip
            break;
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("#");
            break;
         case eICON:
            c_Retval = C_GtGetText::h_GetText("");
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eVALUE_TYPE:
            c_Retval = C_GtGetText::h_GetText("Value type");
            break;
         case eARRAY_SIZE:
            c_Retval = C_GtGetText::h_GetText("Array size");
            break;
         case eAUTO_MIN_MAX:
            c_Retval = C_GtGetText::h_GetText("Auto Min/Max");
            break;
         case eMIN:
            c_Retval = C_GtGetText::h_GetText("Min");
            break;
         case eMAX:
            c_Retval = C_GtGetText::h_GetText("Max");
            break;
         case eFACTOR:
            c_Retval = C_GtGetText::h_GetText("Factor");
            break;
         case eOFFSET:
            c_Retval = C_GtGetText::h_GetText("Offset");
            break;
         case eUNIT:
            c_Retval = C_GtGetText::h_GetText("Unit");
            break;
         case eDATA_SET:
            c_Retval = C_GtGetText::h_GetText("Dataset");
            break;
         case eACCESS:
            c_Retval = C_GtGetText::h_GetText("Access");
            break;
         case eDATA_SIZE:
            c_Retval = C_GtGetText::h_GetText("Data size");
            break;
         case eADDRESS:
            c_Retval = C_GtGetText::h_GetText("Address");
            break;
         case eEVENT_CALL:
            c_Retval = C_GtGetText::h_GetText("Event call");
            break;
         case eUNKNOWN:
         default:
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         const QString c_PhysicalValueInfo = C_GtGetText::h_GetText("The raw value of a data element is the value as it is transmitted in the network."
                                                                    "\nThe physical value of a data element is the value of the physical quantity (e.g. speed, "
                                                                    "\nrpm, temperature, etc.) that represents the data element."
                                                                    "\nThe following conversion formula is used to transform the raw value "
                                                                    "\nto a physical value or in the reverse direction:"
                                                                    "\n\n[Physical value] = ([Raw value] * [Factor]) + [Offset]");
         QString c_InfoText;

         switch (e_Col)
         {
         case eINVALID:
            //no tooltip
            break;
         case eINDEX:
            c_Retval = C_GtGetText::h_GetText("Data element number.");
            break;
         case eICON:
            c_Retval = "Type of data element";
            break;
         case eNAME:
            c_Retval =  C_GtGetText::h_GetText(
               "Symbolic data element name. Unique within a Datapool list."
               "\nFollowing C naming conventions are required:"
               "\n - must not be empty"
               "\n - only alphanumeric characters and \"_\""
               "\n - should not be longer than 31 characters");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment for this data element.");
            break;
         case eVALUE_TYPE:
            c_Retval = C_GtGetText::h_GetText("Data element value type");
            break;
         case eARRAY_SIZE:
            c_Retval = C_GtGetText::h_GetText("Array size range: 1 - 500. 1 = no array.");
            break;
         case eAUTO_MIN_MAX:
            c_InfoText = C_GtGetText::h_GetText(
               "Minimum and Maximum are defined automatically depending on data element value type.\nValues are set as physical values.\n\n");
            c_Retval = c_InfoText.append(c_PhysicalValueInfo);
            break;
         case eMIN:
            c_InfoText = C_GtGetText::h_GetText("Minimum is set as physical value.\n\n");
            c_Retval = c_InfoText.append(c_PhysicalValueInfo);
            break;
         case eMAX:
            c_InfoText = C_GtGetText::h_GetText("Maximum is set as physical value.\n\n");
            c_Retval = c_InfoText.append(c_PhysicalValueInfo);
            break;
         case eFACTOR:
            c_Retval = c_PhysicalValueInfo;
            break;
         case eOFFSET:
            c_Retval = c_PhysicalValueInfo;
            break;
         case eUNIT:
            c_InfoText = C_GtGetText::h_GetText("Unit of the signals physical quantity\n\n");
            c_Retval = c_InfoText.append(c_PhysicalValueInfo);
            break;
         case eDATA_SET:
            c_Retval = C_GtGetText::h_GetText("Dataset value");
            break;
         case eACCESS:
            c_Retval = C_GtGetText::h_GetText("Data element access permission via diagnostic protocol. \n"
                                              "RO = Read Only\n"
                                              "RW = Read / Write"
                                              "\nUse case: Data element visualization in Dashboards");
            break;
         case eDATA_SIZE:
            c_Retval = C_GtGetText::h_GetText("Resulting NVM usage for this data element in bytes."
                                              "\nDepends on value type and array size.");
            break;
         case eADDRESS:
            c_Retval = C_GtGetText::h_GetText("Start address where this data element \n"
                                              "is stored in non volatile memory (NVM).\n"
                                              "<relative>(<absolute>)");
            break;
         case eEVENT_CALL:
            c_Retval = C_GtGetText::h_GetText(
               "Notify the application with a callback function when data elements are changed.");
            break;
         case eUNKNOWN:
         default:
            break;
         }
      }
      //Deactivate help icon (Kept for future reactivation)
      //else if (osn_Role == static_cast<sintn>(Qt::DecorationRole))
      //{
      //   switch (e_Col)
      //   {
      //   case eNAME:
      //      c_Retval = QPixmap(":/images/IconHelpLight.png");
      //      break;
      //   case eINVALID:
      //   case eINDEX:
      //   case eICON:
      //   case eCOMMENT:
      //   case eVALUE_TYPE:
      //   case eARRAY_SIZE:
      //   case eAUTO_MIN_MAX:
      //   case eMIN:
      //   case eMAX:
      //   case eFACTOR:
      //   case eOFFSET:
      //   case eUNIT:
      //   case eDATA_SET:
      //   case eACCESS:
      //   case eDATA_SIZE:
      //   case eADDRESS:
      //   case eEVENT_CALL:
      //   case eUNKNOWN:
      //   default:
      //      break;
      //   }
      //}
      else
      {
         //No data in default case
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeDpListTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval = 0;

   if (!orc_Parent.isValid())
   {
      const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      if (pc_List != NULL)
      {
         //For table parent should always be invalid
         sn_Retval = pc_List->c_Elements.size();
      }
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeDpListTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   stw_types::sintn sn_Retval = 0;
   if (!orc_Parent.isValid())
   {
      const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      if (pc_List != NULL)
      {
         //For table parent should always be invalid
         sn_Retval = 17 + static_cast<sintn>(pc_List->c_DataSets.size());
      }
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpListTableModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      sint32 s32_DataSetIndex;
      const C_SdNdeDpListTableModel::E_Columns e_Col = ColumnToEnum(orc_Index.column(), &s32_DataSetIndex);
      if ((osn_Role == static_cast<sintn>(Qt::DisplayRole)) || (osn_Role == static_cast<sintn>(Qt::EditRole)))
      {
         if (orc_Index.row() >= 0)
         {
            const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
            uint32 u32_SizeByte;
            const C_PuiSdNodeDataPoolListElement * const pc_UIElement =
               C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                         this->mu32_ListIndex, u32_Index);
            const C_OSCNodeDataPoolListElement * const pc_OSCElement =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                          this->mu32_DataPoolIndex,
                                                                          this->mu32_ListIndex, u32_Index);
            if ((pc_OSCElement != NULL) && (pc_UIElement != NULL))
            {
               const C_OSCNodeDataPool * pc_Datapool;
               switch (e_Col)
               {
               case eINVALID:
                  //No data
                  break;
               case eINDEX:
                  c_Retval = orc_Index.row() + 1;
                  break;
               case eNAME:
                  C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OSCElement, *pc_UIElement,
                                                           C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_NAME,
                                                           c_Retval, 0, 0);
                  break;
               case eCOMMENT:
                  C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OSCElement, *pc_UIElement,
                                                           C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_COMMENT,
                                                           c_Retval, 0, 0);
                  break;
               case eVALUE_TYPE:
                  if (osn_Role == static_cast<sintn>(Qt::ItemDataRole::EditRole))
                  {
                     C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OSCElement, *pc_UIElement,
                                                              C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_VALUE_TYPE,
                                                              c_Retval, 0, 0);
                  }
                  else
                  {
                     if (pc_UIElement->q_InterpretAsString == true)
                     {
                        c_Retval = C_GtGetText::h_GetText("string");
                     }
                     else
                     {
                        c_Retval = C_SdNdeDpUtil::h_ConvertContentTypeToString(pc_OSCElement->c_Value.GetType());
                     }
                  }

                  break;
               case eARRAY_SIZE:
                  if (osn_Role == static_cast<sintn>(Qt::EditRole))
                  {
                     C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OSCElement, *pc_UIElement,
                                                              C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_ARRAY,
                                                              c_Retval, 0, 0);
                  }
                  else
                  {
                     if (pc_OSCElement->GetArray() == false)
                     {
                        c_Retval = "-";
                     }
                     else
                     {
                        c_Retval = static_cast<stw_types::sintn>(pc_OSCElement->GetArraySize());
                     }
                  }
                  break;
               case eMIN:
                  if (pc_UIElement->q_InterpretAsString == true)
                  {
                     c_Retval = C_SdNdeDpUtil::h_ConvertToString(pc_OSCElement->c_MinValue);
                  }
                  else
                  {
                     if ((pc_OSCElement->GetArray() == false) || (pc_UIElement->q_AutoMinMaxActive == true))
                     {
                        c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                           pc_OSCElement->c_MinValue,
                           pc_OSCElement->f64_Factor,
                           pc_OSCElement->f64_Offset,
                           0);
                     }
                     else
                     {
                        //Special link handling
                        if (osn_Role == static_cast<sintn>(Qt::ItemDataRole::EditRole))
                        {
                           c_Retval = QPoint();
                        }
                        else
                        {
                           QString c_Final;
                           C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(pc_OSCElement->c_MinValue,
                                                                                   pc_OSCElement->f64_Factor,
                                                                                   pc_OSCElement->f64_Offset,
                                                                                   c_Final);
                           c_Retval = c_Final;
                        }
                     }
                  }
                  break;
               case eMAX:
                  if (pc_UIElement->q_InterpretAsString == true)
                  {
                     c_Retval = C_SdNdeDpUtil::h_ConvertToString(pc_OSCElement->c_MaxValue);
                  }
                  else
                  {
                     if ((pc_OSCElement->GetArray() == false) || (pc_UIElement->q_AutoMinMaxActive == true))
                     {
                        c_Retval = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                           pc_OSCElement->c_MaxValue,
                           pc_OSCElement->f64_Factor,
                           pc_OSCElement->f64_Offset,
                           0);
                     }
                     else
                     {
                        //Special link handling
                        if (osn_Role == static_cast<sintn>(Qt::ItemDataRole::EditRole))
                        {
                           c_Retval = QPoint();
                        }
                        else
                        {
                           QString c_Final;
                           C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(pc_OSCElement->c_MaxValue,
                                                                                   pc_OSCElement->f64_Factor,
                                                                                   pc_OSCElement->f64_Offset,
                                                                                   c_Final);
                           c_Retval = c_Final;
                        }
                     }
                  }
                  break;
               case eFACTOR:
                  C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OSCElement, *pc_UIElement,
                                                           C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_FACTOR,
                                                           c_Retval, 0, 0);
                  break;
               case eOFFSET:
                  C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OSCElement, *pc_UIElement,
                                                           C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_OFFSET,
                                                           c_Retval, 0, 0);
                  break;
               case eUNIT:
                  C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OSCElement, *pc_UIElement,
                                                           C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_UNIT,
                                                           c_Retval, 0, 0);
                  if (c_Retval == "")
                  {
                     c_Retval = "-";
                  }
                  break;
               case eDATA_SET:
                  if (pc_UIElement->q_InterpretAsString == true)
                  {
                     if (s32_DataSetIndex >= 0)
                     {
                        const uint32 u32_DataSet = static_cast<uint32>(s32_DataSetIndex);
                        if (u32_DataSet < pc_OSCElement->c_DataSetValues.size())
                        {
                           QString c_Result =
                              C_SdNdeDpUtil::h_ConvertToString(pc_OSCElement->c_DataSetValues[u32_DataSet]);
                           if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
                           {
                              c_Result += "\\0";
                           }
                           c_Retval = c_Result;
                        }
                     }
                  }
                  else
                  {
                     if (pc_OSCElement->GetArray() == false)
                     {
                        if (s32_DataSetIndex >= 0)
                        {
                           const uint32 u32_DataSet = static_cast<uint32>(s32_DataSetIndex);
                           if (u32_DataSet < pc_OSCElement->c_DataSetValues.size())
                           {
                              c_Retval =
                                 C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_OSCElement->c_DataSetValues[
                                                                                          u32_DataSet],
                                                                                       pc_OSCElement->f64_Factor,
                                                                                       pc_OSCElement->f64_Offset,
                                                                                       0);
                           }
                        }
                     }
                     else
                     {
                        //Special link handling
                        if (osn_Role == static_cast<sintn>(Qt::ItemDataRole::EditRole))
                        {
                           c_Retval = QPoint();
                        }
                        else
                        {
                           if (s32_DataSetIndex >= 0)
                           {
                              const uint32 u32_DataSet = static_cast<uint32>(s32_DataSetIndex);
                              if (u32_DataSet < pc_OSCElement->c_DataSetValues.size())
                              {
                                 QString c_Final;
                                 C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(
                                    pc_OSCElement->c_DataSetValues[u32_DataSet],
                                    pc_OSCElement->f64_Factor,
                                    pc_OSCElement->f64_Offset,
                                    c_Final);
                                 c_Retval = c_Final;
                              }
                           }
                        }
                     }
                  }
                  break;
               case eACCESS:
                  if (osn_Role == static_cast<sintn>(Qt::EditRole))
                  {
                     C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OSCElement, *pc_UIElement,
                                                              C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_ACCESS,
                                                              c_Retval, 0, 0);
                  }
                  else
                  {
                     c_Retval = C_SdNdeDpUtil::h_ConvertElementAccessToString(pc_OSCElement->e_Access);
                  }
                  break;
               case eDATA_SIZE:
                  u32_SizeByte = pc_OSCElement->GetSizeByte();
                  if (u32_SizeByte == 1)
                  {
                     c_Retval = QString("%1 Byte").arg(u32_SizeByte);
                  }
                  else
                  {
                     c_Retval = QString("%1 Bytes").arg(u32_SizeByte);
                  }
                  break;
               case eADDRESS:
                  pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(this->mu32_NodeIndex,
                                                                                this->mu32_DataPoolIndex);
                  c_Retval = QString("%1 (%2)").arg(
                     pc_OSCElement->u32_NvMStartAddress - pc_Datapool->u32_NvMStartAddress).arg(
                     pc_OSCElement->u32_NvMStartAddress);
                  break;
               case eEVENT_CALL:
               case eAUTO_MIN_MAX:
               case eICON:
               case eUNKNOWN:
               default:
                  break;
               }
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_HEADING)
      {
         if (orc_Index.row() >= 0)
         {
            C_OSCNodeDataPool::E_Type e_DataPoolType;
            const C_OSCNodeDataPoolList * pc_OSCElement;
            switch (e_Col)
            {
            case eINVALID:
               pc_OSCElement =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(this->mu32_NodeIndex,
                                                                      this->mu32_DataPoolIndex,
                                                                      this->mu32_ListIndex);
               if (pc_OSCElement != NULL)
               {
                  bool q_NameConflict = false;
                  bool q_NameInvalid = false;
                  bool q_MinOverMax = false;
                  bool q_DataSetInvalid = false;
                  pc_OSCElement->CheckErrorElement(static_cast<uint32>(orc_Index.row()), &q_NameConflict,
                                                   &q_NameInvalid, &q_MinOverMax, &q_DataSetInvalid, NULL);
                  if ((((q_NameConflict == true) || (q_NameInvalid == true)) || (q_MinOverMax == true)) ||
                      (q_DataSetInvalid == true))
                  {
                     if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(this->mu32_NodeIndex,
                                                                          this->mu32_DataPoolIndex,
                                                                          e_DataPoolType) == C_NO_ERR)
                     {
                        QString c_Name;
                        if (e_DataPoolType == C_OSCNodeDataPool::E_Type::eDIAG)
                        {
                           c_Name = C_GtGetText::h_GetText("Variable");
                        }
                        else
                        {
                           c_Name = C_GtGetText::h_GetText("Parameter");
                        }
                        c_Retval = QString(C_GtGetText::h_GetText("%1 has invalid content")).arg(c_Name);
                     }
                  }
               }
               break;
            default:
               //No tool tip
               break;
            }
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_TYPE)
      {
         switch (e_Col)
         {
         case eINVALID:
            c_Retval = static_cast<sintn>(C_NagToolTip::eERROR);
            break;
         default:
            c_Retval = static_cast<sintn>(C_NagToolTip::eDEFAULT);
            //No tool tip
            break;
         }
      }
      else if (osn_Role == msn_USER_ROLE_TOOL_TIP_CONTENT)
      {
         if (orc_Index.row() >= 0)
         {
            const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
            const C_PuiSdNodeDataPoolListElement * const pc_UIElement =
               C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                         this->mu32_ListIndex, u32_Index);
            const C_OSCNodeDataPoolListElement * const pc_OSCElement =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                          this->mu32_DataPoolIndex,
                                                                          this->mu32_ListIndex, u32_Index);
            if ((pc_OSCElement != NULL) && (pc_UIElement != NULL))
            {
               const C_OSCNodeDataPoolList * pc_OSCList;
               switch (e_Col)
               {
               case eINVALID:
                  pc_OSCList =
                     C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(this->mu32_NodeIndex,
                                                                         this->mu32_DataPoolIndex,
                                                                         this->mu32_ListIndex);
                  if (pc_OSCList != NULL)
                  {
                     C_OSCNodeDataPool::E_Type e_DataPoolType;
                     bool q_NameConflict = false;
                     bool q_NameInvalid = false;
                     bool q_MinOverMax = false;
                     bool q_DataSetInvalid = false;
                     std::vector<uint32> c_InvalidDataSetIndices;
                     pc_OSCList->CheckErrorElement(
                        static_cast<uint32>(orc_Index.row()), &q_NameConflict,
                        &q_NameInvalid, &q_MinOverMax, &q_DataSetInvalid,
                        &c_InvalidDataSetIndices);
                     if ((((q_NameConflict == true) || (q_NameInvalid == true)) || (q_MinOverMax == true)) ||
                         (q_DataSetInvalid == true))
                     {
                        if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(this->mu32_NodeIndex,
                                                                             this->mu32_DataPoolIndex,
                                                                             e_DataPoolType) == C_NO_ERR)
                        {
                           QString c_Output;
                           QString c_Name;
                           if (e_DataPoolType == C_OSCNodeDataPool::E_Type::eDIAG)
                           {
                              c_Name = C_GtGetText::h_GetText("Variable");
                           }
                           else
                           {
                              c_Name = C_GtGetText::h_GetText("Parameter");
                           }
                           if (((q_NameConflict == true) || (q_NameInvalid == true)) || (q_MinOverMax == true))
                           {
                              c_Output += C_GtGetText::h_GetText("Invalid properties:\n");
                              if (q_NameConflict == true)
                              {
                                 c_Output +=
                                    QString(C_GtGetText::h_GetText("Duplicate %1 name detected.\n")).arg(c_Name);
                              }
                              if (q_NameInvalid == true)
                              {
                                 c_Output +=
                                    QString(C_GtGetText::h_GetText("%1 name is empty or contains invalid characters.\n"))
                                    .arg(c_Name);
                              }
                              if (q_MinOverMax == true)
                              {
                                 c_Output += C_GtGetText::h_GetText("Minimum value over maximum value\n");
                              }
                              c_Output += "\n";
                           }
                           if (q_DataSetInvalid == true)
                           {
                              c_Output += C_GtGetText::h_GetText("Data sets:\n");
                              for (uint32 u32_ItAppl = 0;
                                   (u32_ItAppl < c_InvalidDataSetIndices.size()) &&
                                   (u32_ItAppl < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                                   ++u32_ItAppl)
                              {
                                 const C_OSCNodeDataPoolDataSet * const pc_DataSet =
                                    C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListDataSet(this->mu32_NodeIndex,
                                                                                               this->mu32_DataPoolIndex,
                                                                                               this->mu32_ListIndex,
                                                                                               c_InvalidDataSetIndices[
                                                                                                  u32_ItAppl]);
                                 if (pc_DataSet != NULL)
                                 {
                                    c_Output += QString("%1\n").arg(pc_DataSet->c_Name.c_str());
                                 }
                              }
                              if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidDataSetIndices.size())
                              {
                                 c_Output += QString("+%1\n").arg(
                                    c_InvalidDataSetIndices.size() - mu32_TOOL_TIP_MAXIMUM_ITEMS);
                              }
                              c_Output += "\n";
                           }
                           c_Retval = c_Output;
                        }
                     }
                  }

                  break;
               case eCOMMENT:
                  C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OSCElement, *pc_UIElement,
                                                           C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_COMMENT,
                                                           c_Retval, 0, 0);
                  break;
               case eMIN:
                  if ((pc_OSCElement->GetArray() == false) || (pc_UIElement->q_AutoMinMaxActive == true) ||
                      (pc_UIElement->q_InterpretAsString == true))
                  {
                     //No tooltip
                  }
                  else
                  {
                     //Special link handling
                     QString c_Final;
                     C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(pc_OSCElement->c_MinValue,
                                                                             pc_OSCElement->f64_Factor,
                                                                             pc_OSCElement->f64_Offset,
                                                                             c_Final);
                     c_Retval = c_Final;
                  }
                  break;
               case eMAX:
                  if ((pc_OSCElement->GetArray() == false) || (pc_UIElement->q_AutoMinMaxActive == true) ||
                      (pc_UIElement->q_InterpretAsString == true))
                  {
                     //No tooltip
                  }
                  else
                  {
                     //Special link handling
                     QString c_Final;
                     C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(pc_OSCElement->c_MaxValue,
                                                                             pc_OSCElement->f64_Factor,
                                                                             pc_OSCElement->f64_Offset,
                                                                             c_Final);
                     c_Retval = c_Final;
                  }

                  break;
               case eDATA_SET:
                  if (pc_UIElement->q_InterpretAsString == true)
                  {
                     if (s32_DataSetIndex >= 0)
                     {
                        const uint32 u32_DataSet = static_cast<uint32>(s32_DataSetIndex);
                        if (u32_DataSet < pc_OSCElement->c_DataSetValues.size())
                        {
                           QString c_Result =
                              C_SdNdeDpUtil::h_ConvertToString(pc_OSCElement->c_DataSetValues[u32_DataSet]);
                           c_Result += "\\0";
                           c_Retval = c_Result;
                        }
                     }
                  }
                  else
                  {
                     if (pc_OSCElement->GetArray() == false)
                     {
                        //No tooltip
                     }
                     else
                     {
                        if (s32_DataSetIndex >= 0)
                        {
                           const uint32 u32_DataSet = static_cast<uint32>(s32_DataSetIndex);
                           if (u32_DataSet < pc_OSCElement->c_DataSetValues.size())
                           {
                              QString c_Final;
                              C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(
                                 pc_OSCElement->c_DataSetValues[u32_DataSet],
                                 pc_OSCElement->f64_Factor,
                                 pc_OSCElement->f64_Offset,
                                 c_Final);
                              c_Retval = c_Final;
                           }
                        }
                     }
                  }
                  break;
               default:
                  //No tool tip
                  break;
               }
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
      {
         if (orc_Index.row() >= 0)
         {
            const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
            const C_PuiSdNodeDataPoolListElement * pc_UIElement;
            const C_OSCNodeDataPoolListElement * pc_OSCElement;
            switch (e_Col)
            {
            case eAUTO_MIN_MAX:
               pc_UIElement =
                  C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex,
                                                                            this->mu32_DataPoolIndex,
                                                                            this->mu32_ListIndex,
                                                                            u32_Index);
               if (pc_UIElement != NULL)
               {
                  if (pc_UIElement->q_AutoMinMaxActive == true)
                  {
                     c_Retval = static_cast<sintn>(Qt::Checked);
                  }
                  else
                  {
                     c_Retval = static_cast<sintn>(Qt::Unchecked);
                  }
               }
               break;
            case eEVENT_CALL:
               pc_OSCElement =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                             this->mu32_DataPoolIndex,
                                                                             this->mu32_ListIndex, u32_Index);
               if (pc_OSCElement != NULL)
               {
                  if (pc_OSCElement->q_DiagEventCall == true)
                  {
                     c_Retval = static_cast<sintn>(Qt::Checked);
                  }
                  else
                  {
                     c_Retval = static_cast<sintn>(Qt::Unchecked);
                  }
               }
               break;
            default:
               break;
            }
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::FontRole))
      {
         QFont c_Font;
         //Stylesheets do not allow access of specific columns so we need to set fonts manually
         switch (e_Col)
         {
         case eCOMMENT:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
            break;
         case eDATA_SET:
            //Default
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            if (m_CheckLink(orc_Index) == true)
            {
               //Special link handling
               c_Font.setUnderline(true);
            }
            break;
         case eMIN:
         case eMAX:
            //Default
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            if (orc_Index.row() >= 0)
            {
               const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
               const C_PuiSdNodeDataPoolListElement * const pc_UIElement =
                  C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex,
                                                                            this->mu32_DataPoolIndex,
                                                                            this->mu32_ListIndex, u32_Index);
               const C_OSCNodeDataPoolListElement * const pc_OSCElement =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                             this->mu32_DataPoolIndex,
                                                                             this->mu32_ListIndex, u32_Index);
               if ((pc_OSCElement != NULL) && (pc_UIElement != NULL))
               {
                  if ((pc_UIElement->q_AutoMinMaxActive == false) &&
                      ((pc_UIElement->q_InterpretAsString == false) && (pc_OSCElement->GetArray() == true)))
                  {
                     //Special link handling
                     c_Font.setUnderline(true);
                  }
               }
            }
            break;
         case eNAME:
         case eINVALID:
         case eINDEX:
         case eICON:
         case eVALUE_TYPE:
         case eARRAY_SIZE:
         case eAUTO_MIN_MAX:
         case eFACTOR:
         case eOFFSET:
         case eUNIT:
         case eACCESS:
         case eDATA_SIZE:
         case eADDRESS:
         case eEVENT_CALL:
         case eUNKNOWN:
         default:
            c_Font = mc_STYLE_GUIDE_FONT_REGULAR_14;
            break;
         }
         //Convert point to pixel
         c_Font.setPixelSize(c_Font.pointSize());
         c_Retval = c_Font;
      }
      else if (osn_Role == static_cast<sintn>(Qt::ForegroundRole))
      {
         bool q_MinOverMax = false;
         const C_OSCNodeDataPoolList * pc_OSCElement;
         //Stylesheets do not allow access of specific columns so we need to set t manually
         switch (e_Col)
         {
         case eCOMMENT:
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            break;
         case eMIN:
         case eMAX:
            //Default
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            //Link color
            if (m_CheckLink(orc_Index) == true)
            {
               //Special link handling
               c_Retval = mc_STYLE_GUIDE_COLOR_LINK;
            }
            //Check inactive
            if (orc_Index.row() >= 0)
            {
               const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
               const C_PuiSdNodeDataPoolListElement * const pc_UIElement =
                  C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex,
                                                                            this->mu32_DataPoolIndex,
                                                                            this->mu32_ListIndex, u32_Index);
               if (pc_UIElement != NULL)
               {
                  if (pc_UIElement->q_AutoMinMaxActive == true)
                  {
                     c_Retval = mc_STYLE_GUIDE_COLOR_10;
                  }
                  else
                  {
                     //Check error
                     if (orc_Index.row() >= 0)
                     {
                        pc_OSCElement =
                           C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(this->mu32_NodeIndex,
                                                                               this->mu32_DataPoolIndex,
                                                                               this->mu32_ListIndex);
                        if (pc_OSCElement != NULL)
                        {
                           pc_OSCElement->CheckErrorElement(static_cast<uint32>(orc_Index.row()), NULL,
                                                            NULL, &q_MinOverMax, NULL, NULL);
                        }
                     }
                     if (q_MinOverMax == true)
                     {
                        c_Retval = mc_STYLE_GUIDE_COLOR_18;
                     }
                  }
               }
            }
            break;
         case eDATA_SIZE:
         case eADDRESS:
            c_Retval = mc_STYLE_GUIDE_COLOR_10;
            break;
         //Error
         case eNAME:
            //Default
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            //Check error
            if (orc_Index.row() >= 0)
            {
               bool q_NameInvalid = false;
               pc_OSCElement =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(this->mu32_NodeIndex,
                                                                      this->mu32_DataPoolIndex,
                                                                      this->mu32_ListIndex);
               if (pc_OSCElement != NULL)
               {
                  bool q_NameConflict = false;
                  pc_OSCElement->CheckErrorElement(static_cast<uint32>(orc_Index.row()),
                                                   &q_NameConflict, &q_NameInvalid, NULL, NULL, NULL);
                  if (q_NameConflict == true)
                  {
                     q_NameInvalid = true;
                  }
               }
               if (q_NameInvalid == true)
               {
                  c_Retval = mc_STYLE_GUIDE_COLOR_18;
               }
            }
            break;
         case eDATA_SET:
            //Default
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            //Link color
            if (m_CheckLink(orc_Index) == true)
            {
               //Special link handling
               c_Retval = mc_STYLE_GUIDE_COLOR_LINK;
            }
            //Check error
            if ((orc_Index.row() >= 0) && (s32_DataSetIndex >= 0))
            {
               const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
               bool q_ValueBelowMin = false;
               pc_OSCElement =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(this->mu32_NodeIndex,
                                                                      this->mu32_DataPoolIndex,
                                                                      this->mu32_ListIndex);
               if (pc_OSCElement != NULL)
               {
                  bool q_ValueOverMax = false;
                  pc_OSCElement->CheckErrorDataSetValue(u32_Index, static_cast<uint32>(s32_DataSetIndex),
                                                        &q_ValueBelowMin, &q_ValueOverMax, NULL);
                  if (q_ValueOverMax == true)
                  {
                     q_ValueBelowMin = true;
                  }
               }
               if (q_ValueBelowMin == true)
               {
                  c_Retval = mc_STYLE_GUIDE_COLOR_18;
               }
            }
            break;
         case eINDEX:
            c_Retval = mc_STYLE_GUIDE_COLOR_8;
            break;
         case eINVALID:
         case eICON:
         case eVALUE_TYPE:
         case eARRAY_SIZE:
         case eAUTO_MIN_MAX:
         case eFACTOR:
         case eOFFSET:
         case eUNIT:
         case eACCESS:
         case eEVENT_CALL:
         case eUNKNOWN:
         default:
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            break;
         }
      }
      else if (osn_Role == static_cast<sintn>(Qt::TextAlignmentRole))
      {
         c_Retval = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (osn_Role == msn_USER_ROLE_ICON)
      {
         if (orc_Index.row() >= 0)
         {
            QStringList c_Tmp;
            C_OSCNodeDataPool::E_Type e_DataPoolType;
            const C_OSCNodeDataPoolList * pc_OSCElement;
            const C_PuiSdNodeDataPoolListElement * pc_UIElement;
            switch (e_Col)
            {
            case eINVALID:
               pc_OSCElement =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(this->mu32_NodeIndex,
                                                                      this->mu32_DataPoolIndex,
                                                                      this->mu32_ListIndex);
               pc_UIElement =
                  C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex,
                                                                            this->mu32_DataPoolIndex,
                                                                            this->mu32_ListIndex,
                                                                            static_cast<uint32>(orc_Index.row()));
               if ((pc_OSCElement != NULL) && (pc_UIElement != NULL))
               {
                  bool q_NameConflict = false;
                  bool q_NameInvalid = false;
                  bool q_MinOverMax = false;
                  bool q_DataSetInvalid = false;
                  pc_OSCElement->CheckErrorElement(static_cast<uint32>(orc_Index.row()), &q_NameConflict,
                                                   &q_NameInvalid, &q_MinOverMax, &q_DataSetInvalid, NULL);
                  if ((((q_NameConflict == true) || (q_NameInvalid == true)) || (q_MinOverMax == true)) ||
                      (q_DataSetInvalid == true))
                  {
                     c_Tmp.push_back(QString::number(24));
                     //Show error
                     c_Tmp.push_back("://images/Error_iconV2.svg");
                  }
               }
               break;
            case eICON:
               if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                    e_DataPoolType) == C_NO_ERR)
               {
                  c_Tmp.push_back(QString::number(20));
                  if (e_DataPoolType == C_OSCNodeDataPool::E_Type::eDIAG)
                  {
                     c_Tmp.push_back(":/images/system_definition/IconVariable.svg");
                  }
                  else
                  {
                     c_Tmp.push_back(":/images/system_definition/IconParameter.svg");
                  }
               }
               break;
            case eNAME:
            case eINDEX:
            case eCOMMENT:
            case eVALUE_TYPE:
            case eARRAY_SIZE:
            case eAUTO_MIN_MAX:
            case eMIN:
            case eMAX:
            case eFACTOR:
            case eOFFSET:
            case eUNIT:
            case eDATA_SET:
            case eACCESS:
            case eDATA_SIZE:
            case eADDRESS:
            case eEVENT_CALL:
            case eUNKNOWN:
            default:
               //No decoration
               break;
            }
            c_Retval = c_Tmp;
         }
      }
      else
      {
         //No special handling
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set data at index

   \param[in]  orc_Index   Index
   \param[in]  orc_Value   New data
   \param[in]  osn_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const sintn osn_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, osn_Role) != orc_Value)
   {
      if (orc_Index.row() >= 0)
      {
         if (osn_Role == static_cast<sintn>(Qt::EditRole))
         {
            const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
            sint32 s32_DataSetIndex;
            const C_SdNdeDpListTableModel::E_Columns e_Col =
               ColumnToEnum(orc_Index.column(), &s32_DataSetIndex);
            switch (e_Col)
            {
            case eINVALID:
               //No edit
               break;
            case eINDEX:
               //No edit
               break;
            case eNAME:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_NAME,
                                          0, 0);
               q_Retval = true;
               break;
            case eCOMMENT:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_COMMENT,
                                          0, 0);
               q_Retval = true;
               break;
            case eVALUE_TYPE:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_VALUE_TYPE,
                                          0, 0);
               q_Retval = true;
               break;
            case eARRAY_SIZE:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_ARRAY,
                                          0, 0);
               q_Retval = true;
               break;
            case eAUTO_MIN_MAX:
               //No edit
               break;
            case eMIN:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_MIN,
                                          0, 0);
               q_Retval = true;
               break;
            case eMAX:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_MAX,
                                          0, 0);
               q_Retval = true;
               break;
            case eFACTOR:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_FACTOR,
                                          0, 0);
               q_Retval = true;
               break;
            case eOFFSET:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_OFFSET,
                                          0, 0);
               q_Retval = true;
               break;
            case eUNIT:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_UNIT,
                                          0, 0);
               q_Retval = true;
               break;
            case eDATA_SET:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_DATA_SET,
                                          0, s32_DataSetIndex);
               q_Retval = true;
               break;
            case eACCESS:
               Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                          this->mu32_ListIndex,
                                          u32_Index, orc_Value,
                                          C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_ACCESS,
                                          0, 0);
               q_Retval = true;
               break;
            case eDATA_SIZE:
               //No edit
               break;
            case eADDRESS:
               //No edit
               break;
            case eEVENT_CALL:
               //No edit
               break;
            default:
               break;
            }
         }
         else if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
         {
            const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
            sint32 s32_DataSetIndex;
            const C_SdNdeDpListTableModel::E_Columns e_Col =
               ColumnToEnum(orc_Index.column(), &s32_DataSetIndex);
            if (orc_Value.type() == QVariant::Int)
            {
               Qt::CheckState e_CheckedState = static_cast<Qt::CheckState>(orc_Value.toInt());
               bool q_Val;
               if (e_CheckedState == Qt::Checked)
               {
                  q_Val = true;
               }
               else
               {
                  q_Val = false;
               }
               switch (e_Col)
               {
               case eAUTO_MIN_MAX:
                  Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                             this->mu32_ListIndex,
                                             u32_Index, QVariant(q_Val),
                                             C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_AUTO_MIN_MAX,
                                             0, 0);
                  q_Retval = true;
                  break;
               case eEVENT_CALL:
                  Q_EMIT this->SigDataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                             this->mu32_ListIndex,
                                             u32_Index, QVariant(q_Val),
                                             C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_EVENT_CALL,
                                             0, 0);
                  q_Retval = true;
                  break;
               default:
                  break;
               }
            }
         }
         else
         {
            //Unknown data change
         }
         //lint -e{1793} Qt example
         Q_EMIT this->dataChanged(orc_Index, orc_Index, QVector<stw_types::sintn>() << osn_Role);
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flags for item

   \param[in]  orc_Index   Item

   \return
   Flags for item
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdNdeDpListTableModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval;
   if (orc_Index.isValid() == true)
   {
      const C_PuiSdNodeDataPoolListElement * pc_UIElement;
      const C_OSCNodeDataPool * pc_OSCDataPool;
      bool q_Edit = true;
      const C_SdNdeDpListTableModel::E_Columns e_Col = ColumnToEnum(orc_Index.column());
      //Each item
      c_Retval =  Qt::ItemIsSelectable;
      //Add edit
      switch (e_Col)
      {
      case eINVALID:
         break;
      case eNAME:
      case eCOMMENT:
      case eVALUE_TYPE:
      case eARRAY_SIZE:
      case eEVENT_CALL:
      case eDATA_SET:
         c_Retval = c_Retval | Qt::ItemIsEditable | Qt::ItemIsEnabled;
         //Check box
         if (e_Col == eEVENT_CALL)
         {
            c_Retval = c_Retval | Qt::ItemIsUserCheckable;
         }
         break;
      case eACCESS:
         pc_OSCDataPool =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(this->mu32_NodeIndex, this->mu32_DataPoolIndex);
         if ((pc_OSCDataPool != NULL) && (pc_OSCDataPool->q_IsSafety == false))
         {
            c_Retval = c_Retval | Qt::ItemIsEditable | Qt::ItemIsEnabled;
         }
         else
         {
            c_Retval = c_Retval | Qt::ItemIsEnabled;
         }
         break;
      case eAUTO_MIN_MAX:
      case eFACTOR:
      case eOFFSET:
      case eUNIT:
         pc_UIElement =
            C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                      this->mu32_ListIndex,
                                                                      static_cast<uint32>(orc_Index.row()));
         if (pc_UIElement != NULL)
         {
            if (pc_UIElement->q_InterpretAsString == true)
            {
               q_Edit = false;
            }
         }

         if (q_Edit == true)
         {
            c_Retval = c_Retval | Qt::ItemIsEditable | Qt::ItemIsEnabled;
            //Check box
            if (e_Col == eAUTO_MIN_MAX)
            {
               c_Retval = c_Retval | Qt::ItemIsUserCheckable;
            }
         }
         break;
      case eMIN:
      case eMAX:
         pc_UIElement =
            C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                      this->mu32_ListIndex,
                                                                      static_cast<uint32>(orc_Index.row()));
         if (pc_UIElement != NULL)
         {
            if ((pc_UIElement->q_AutoMinMaxActive == true) || (pc_UIElement->q_InterpretAsString == true))
            {
               q_Edit = false;
            }
         }

         if (q_Edit == true)
         {
            c_Retval = c_Retval | Qt::ItemIsEditable | Qt::ItemIsEnabled;
         }
         break;
      case eADDRESS:
      case eDATA_SIZE:
         //Not enabled
         break;
      case eUNKNOWN:
      case eICON:
      case eINDEX:
      default:
         c_Retval = c_Retval | Qt::ItemIsEnabled;
         break;
      }
   }
   else
   {
      c_Retval = Qt::NoItemFlags;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get supported drop actions

   \return
   Supported drop actions
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::DropActions C_SdNdeDpListTableModel::supportedDropActions(void) const
{
   return Qt::MoveAction;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert rows with content

   \param[in]  orc_OSCInsertedElements    Inserted elements OSC content
   \param[in]  orc_UIInsertedElements     Inserted elements UI content
   \param[in]  orc_Rows                   Rows (Expected: Sorted ascending!)

   \return
   Continuous sections of new elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::vector<uint32> > C_SdNdeDpListTableModel::DoInsertRows(
   const std::vector<C_OSCNodeDataPoolListElement> & orc_OSCInsertedElements,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIInsertedElements, const std::vector<uint32> & orc_Rows)
{
   std::vector<std::vector<uint32> > c_Retval;

   if ((orc_OSCInsertedElements.size() == orc_UIInsertedElements.size()) &&
       (orc_OSCInsertedElements.size() == orc_Rows.size()))
   {
      uint32 u32_Counter = 0UL;
      c_Retval = C_Uti::h_GetContiguousSectionsAscending(orc_Rows);
      C_PuiSdHandler::h_GetInstance()->ReserveDataPoolListElements(this->mu32_NodeIndex,
                                                                   this->mu32_DataPoolIndex,
                                                                   this->mu32_ListIndex,
                                                                   orc_OSCInsertedElements.size());
      for (uint32 u32_ItSection = 0UL; u32_ItSection < c_Retval.size(); ++u32_ItSection)
      {
         const std::vector<uint32> & rc_Section = c_Retval[u32_ItSection];
         if (rc_Section.size() > 0UL)
         {
            this->beginInsertRows(QModelIndex(), rc_Section[0UL],
                                  rc_Section[static_cast<std::vector<uint32>::size_type>(rc_Section.size() - 1UL)]);
            for (uint32 u32_ItItem = 0UL; u32_ItItem < rc_Section.size(); ++u32_ItItem)
            {
               C_PuiSdHandler::h_GetInstance()->InsertDataPoolListElement(this->mu32_NodeIndex,
                                                                          this->mu32_DataPoolIndex,
                                                                          this->mu32_ListIndex, rc_Section[u32_ItItem],
                                                                          orc_OSCInsertedElements[u32_Counter],
                                                                          orc_UIInsertedElements[u32_Counter]);
               ++u32_Counter;
            }
            this->endInsertRows();
         }
      }
      this->mc_ErrorManager.OnErrorChange();
      Q_EMIT this->SigSizeChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert items into model

   \param[in]  osn_Col     Starting column
   \param[in]  osn_Count   Number of inserted items
   \param[in]  orc_Parent  Parent

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::insertColumns(const sintn osn_Col, const sintn osn_Count, const QModelIndex & orc_Parent)
{
   bool q_Retval = false;

   if ((osn_Count > 0) && (osn_Col >= 0))
   {
      beginInsertColumns(orc_Parent, osn_Col, (osn_Col + osn_Count) - 1);
      //No action as change was already done before (function call just for signals)
      endInsertColumns();
      q_Retval =  true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle delete items action

   \param[in]  orc_Rows    Row indices (Expected: unique, ascending)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::DoRemoveRows(const std::vector<uint32> & orc_Rows)
{
   const std::vector<std::vector<uint32> > c_ContiguousSections = C_Uti::h_GetContiguousSectionsAscending(orc_Rows);

   //Start deleting from back (easier to keep indices valid)
   for (uint32 u32_ItSection = c_ContiguousSections.size(); u32_ItSection > 0UL; --u32_ItSection)
   {
      const std::vector<uint32> & rc_Section =
         c_ContiguousSections[static_cast<std::vector<uint32>::size_type>(u32_ItSection - 1UL)];
      if (rc_Section.size() > 0UL)
      {
         this->beginRemoveRows(QModelIndex(), rc_Section[0UL],
                               rc_Section[static_cast<std::vector<uint32>::size_type>(rc_Section.size() - 1UL)]);
         for (uint32 u32_ItItem = rc_Section.size(); u32_ItItem > 0UL; --u32_ItItem)
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->RemoveDataPoolListElement(this->mu32_NodeIndex,
                                                                                  this->mu32_DataPoolIndex,
                                                                                  this->mu32_ListIndex,
                                                                                  rc_Section[static_cast<std::vector<uint32>
                                                                                                         ::size_type>(
                                                                                                u32_ItItem - 1UL)]) ==
                       C_NO_ERR);
         }
         this->endRemoveRows();
      }
   }
   this->mc_ErrorManager.OnErrorChange();
   Q_EMIT this->SigSizeChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove columns from model

   \param[in]  osn_Col     Starting column
   \param[in]  osn_Count   Number of removed items
   \param[in]  orc_Parent  Parent

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::removeColumns(const sintn osn_Col, const sintn osn_Count, const QModelIndex & orc_Parent)
{
   bool q_Retval = false;

   if ((osn_Count > 0) && (osn_Col >= 0))
   {
      beginRemoveColumns(orc_Parent, osn_Col, (osn_Col + osn_Count) - 1);
      //No action as change was already done before (function call just for signals)
      endRemoveColumns();
      q_Retval = true;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle move selected items action

   Warning 1: only works if target indices have same contiguous property as selected indices
   i.e. if you make the selected indices contiguous by resorting the target indices should also become contiguous
   Warning 2: does no boundary check

   \param[in]  orc_SelectedIndices  Selected row indices (Expected: unique)
   \param[in]  orc_TargetIndices    Target row indices (Expected: unique)
                                    Example value:
                                    "move up" -> orc_SelectedIndices - 1
                                    "move down" -> orc_TargetIndices + 1
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::DoMoveRows(const std::vector<uint32> & orc_SelectedIndices,
                                         const std::vector<uint32> & orc_TargetIndices)
{
   if (orc_SelectedIndices.size() == orc_TargetIndices.size())
   {
      std::vector<uint32> c_SelectedIndicesCopy = orc_SelectedIndices;
      std::vector<uint32> c_TargetIndicesCopy = orc_TargetIndices;
      std::vector<std::vector<uint32> > c_ContiguousSections;
      uint32 u32_TargetAccessIndex = 0UL;

      //Step 1 sort (so the next step can assume the contiguous selection has the same order!
      C_Uti::h_SortIndicesAscendingAndSync(c_SelectedIndicesCopy, c_TargetIndicesCopy);

      //Step 2: get contiguous selection
      c_ContiguousSections = C_Uti::h_GetContiguousSectionsAscending(orc_SelectedIndices);

      //Step 3: move
      for (uint32 u32_ItSection = 0UL; u32_ItSection < c_ContiguousSections.size(); ++u32_ItSection)
      {
         const std::vector<uint32> & rc_Section = c_ContiguousSections[u32_ItSection];
         if (rc_Section.size() > 0UL)
         {
            uint32 u32_TargetIndex = c_TargetIndicesCopy[u32_TargetAccessIndex];
            uint32 u32_TargetIndexParam = c_TargetIndicesCopy[u32_TargetAccessIndex];
            //+1UL seems to fit the documentation of beginMoveRows of what we want to achieve here
            if (u32_TargetIndex > rc_Section[0UL])
            {
               //Qt interface seems to insert the items before removing anything so the "new position" has to have an
               // bigger offset
               u32_TargetIndex += rc_Section.size();
               //We insert after removing so this has to be considered
               u32_TargetIndexParam = u32_TargetIndex - 1UL;
            }
            this->beginMoveRows(QModelIndex(), rc_Section[0UL],
                                rc_Section[static_cast<std::vector<uint32>::size_type>(rc_Section.size() - 1UL)],
                                QModelIndex(), u32_TargetIndex);
            this->m_MoveItems(rc_Section, u32_TargetIndexParam);
            this->endMoveRows();
            u32_TargetAccessIndex += rc_Section.size();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]   ors32_Column        Column
   \param[out]  opc_DataSetIndex    Optional: If column is a data set, this contains the data set index
                                    Else -1

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListTableModel::E_Columns C_SdNdeDpListTableModel::ColumnToEnum(const sint32 & ors32_Column,
                                                                         sint32 * const opc_DataSetIndex)
const
{
   C_SdNdeDpListTableModel::E_Columns e_Retval;
   if (opc_DataSetIndex != NULL)
   {
      *opc_DataSetIndex = -1;
   }
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eINVALID;
      break;
   case 1:
      e_Retval = eINDEX;
      break;
   case 2:
      e_Retval = eICON;
      break;
   case 3:
      e_Retval = eNAME;
      break;
   case 4:
      e_Retval = eCOMMENT;
      break;
   case 5:
      e_Retval = eVALUE_TYPE;
      break;
   case 6:
      e_Retval = eARRAY_SIZE;
      break;
   case 7:
      e_Retval = eAUTO_MIN_MAX;
      break;
   case 8:
      e_Retval = eMIN;
      break;
   case 9:
      e_Retval = eMAX;
      break;
   case 10:
      e_Retval = eFACTOR;
      break;
   case 11:
      e_Retval = eOFFSET;
      break;
   case 12:
      e_Retval = eUNIT;
      break;
   default:
      if (ors32_Column == static_cast<sint32>(this->columnCount() - 1))
      {
         e_Retval = eEVENT_CALL;
      }
      else if (ors32_Column == static_cast<sint32>(this->columnCount() - 2))
      {
         e_Retval = eADDRESS;
      }
      else if (ors32_Column == static_cast<sint32>(this->columnCount() - 3))
      {
         e_Retval = eDATA_SIZE;
      }
      else if (ors32_Column == static_cast<sint32>(this->columnCount() - 4))
      {
         e_Retval = eACCESS;
      }
      else
      {
         e_Retval = eDATA_SET;
         if (opc_DataSetIndex != NULL)
         {
            *opc_DataSetIndex = ors32_Column - 13;
         }
      }
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enum to column conversion

   \param[in]  ore_Value   Enum value

   \return
   Column
   -1 Error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeDpListTableModel::EnumToColumn(const C_SdNdeDpListTableModel::E_Columns & ore_Value) const
{
   sint32 s32_Retval;

   switch (ore_Value)
   {
   case eINVALID:
      s32_Retval = 0;
      break;
   case eINDEX:
      s32_Retval = 1;
      break;
   case eICON:
      s32_Retval = 2;
      break;
   case eNAME:
      s32_Retval = 3;
      break;
   case eCOMMENT:
      s32_Retval = 4;
      break;
   case eVALUE_TYPE:
      s32_Retval = 5;
      break;
   case eARRAY_SIZE:
      s32_Retval = 6;
      break;
   case eAUTO_MIN_MAX:
      s32_Retval = 7;
      break;
   case eMIN:
      s32_Retval = 8;
      break;
   case eMAX:
      s32_Retval = 9;
      break;
   case eFACTOR:
      s32_Retval = 10;
      break;
   case eOFFSET:
      s32_Retval = 11;
      break;
   case eUNIT:
      s32_Retval = 12;
      break;
   case eDATA_SET:
      s32_Retval = 13;
      break;
   case eACCESS:
      s32_Retval = static_cast<sint32>(this->columnCount() - 4);
      break;
   case eDATA_SIZE:
      s32_Retval = static_cast<sint32>(this->columnCount() - 3);
      break;
   case eADDRESS:
      s32_Retval = static_cast<sint32>(this->columnCount() - 2);
      break;
   case eEVENT_CALL:
      s32_Retval = static_cast<sint32>(this->columnCount() - 1);
      break;
   case eUNKNOWN:
      s32_Retval = -1;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Mime types

   \return
   Table mime types
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_SdNdeDpListTableModel::mimeTypes(void) const
{
   //Append original types (Default events only accept this type)
   QStringList c_Retval = QAbstractTableModel::mimeTypes();

   //Append own types last (Default events only accept Qt types at start)
   c_Retval.append("application/stw/opensyde/system/node/data-pool/list/elements");
   c_Retval.append("application/stw/opensyde/system/node/data-pool/list/indices");

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get mime data of specified indices

   \param[in]  orc_Indices    Indices

   \return
   Mime data of specified indices
*/
//----------------------------------------------------------------------------------------------------------------------
QMimeData * C_SdNdeDpListTableModel::mimeData(const QModelIndexList & orc_Indices) const
{
   QMimeData * pc_Retval = NULL;

   if (orc_Indices.size() > 0)
   {
      if (this->mimeTypes().size() > 2)
      {
         const C_PuiSdNodeDataPoolList * const pc_UIList = C_PuiSdHandler::h_GetInstance()->GetUIDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         const C_OSCNodeDataPoolList * const pc_OSCList = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         if ((pc_UIList != NULL) && (pc_OSCList != NULL))
         {
            std::vector<uint32> c_Rows = C_SdNdeDpUtil::h_ConvertVector(orc_Indices, true);
            QString c_String;
            std::vector<C_OSCNodeDataPoolListElement> c_OSCElements;
            std::vector<C_PuiSdNodeDataPoolListElement> c_UIElements;
            //Convert indices to unique rows
            C_Uti::h_Uniqueify(c_Rows);
            //Reserve
            c_OSCElements.reserve(c_Rows.size());
            c_UIElements.reserve(c_Rows.size());
            //Copy relevant data
            for (uint32 u32_ItIndex = 0; u32_ItIndex < c_Rows.size(); ++u32_ItIndex)
            {
               c_OSCElements.push_back(pc_OSCList->c_Elements.at(c_Rows[u32_ItIndex]));
               c_UIElements.push_back(pc_UIList->c_DataPoolListElements.at(c_Rows[u32_ItIndex]));
            }
            //Store in mime
            pc_Retval = new QMimeData();
            //Use default mime type so qt accepts these
            pc_Retval->setData(this->mimeTypes().at(0), "");
            C_SdClipBoardHelper::h_StoreDataPoolListElementsToString(c_OSCElements, c_UIElements, c_String);
            pc_Retval->setData(this->mimeTypes().at(1), c_String.toStdString().c_str());
            C_SdClipBoardHelper::h_StoreIndicesToString(c_Rows, c_String);
            pc_Retval->setData(this->mimeTypes().at(2), c_String.toStdString().c_str());
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get osc node data pool list element

   \param[in]  orc_Index   Table index

   \return
   OSC node data pool list element
   Else NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNodeDataPoolListElement * C_SdNdeDpListTableModel::GetOSCElement(const QModelIndex & orc_Index)
const
{
   return C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                     this->mu32_ListIndex,
                                                                     static_cast<uint32>(orc_Index.row()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get ui node data pool list element

   \param[in]  orc_Index   Table index

   \return
   UI node data pool list element
   Else NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSdNodeDataPoolListElement * C_SdNdeDpListTableModel::GetUIElement(const QModelIndex & orc_Index) const
{
   return C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                    this->mu32_ListIndex,
                                                                    static_cast<uint32>(orc_Index.row()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal model update

   \param[in]  oru32_Row            Changed row
   \param[in]  ore_ChangeType       Change type
   \param[in]  ors32_DataSetIndex   Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::HandleDataChange(const uint32 & oru32_Row,
                                               const C_SdNdeDpUtil::E_ElementDataChangeType & ore_ChangeType,
                                               const sint32 & ors32_DataSetIndex)
{
   bool q_ErrorChange = false;
   sint32 s32_Column = -1;

   switch (ore_ChangeType)
   {
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_ACCESS:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eACCESS);
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_NAME:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eNAME);
      q_ErrorChange = true;
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_COMMENT:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eCOMMENT);
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_AUTO_MIN_MAX:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eAUTO_MIN_MAX);
      q_ErrorChange = true;
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_MAX:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eMAX);
      q_ErrorChange = true;
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_MIN:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eMIN);
      q_ErrorChange = true;
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_ARRAY:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eARRAY_SIZE);
      q_ErrorChange = true;
      Q_EMIT this->SigSizeChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_DATA_SET:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eDATA_SET);
      if (ors32_DataSetIndex >= 0)
      {
         s32_Column += ors32_DataSetIndex;
      }
      q_ErrorChange = true;
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_EVENT_CALL:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eEVENT_CALL);
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_FACTOR:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eFACTOR);
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_OFFSET:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eOFFSET);
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_UNIT:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eUNIT);
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_VALUE_TYPE:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eVALUE_TYPE);
      q_ErrorChange = true;
      Q_EMIT this->SigSizeChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      break;
   default:
      //No handling
      break;
   }
   if (q_ErrorChange == true)
   {
      const QModelIndex c_Index = this->index(oru32_Row, this->EnumToColumn(eINVALID));
      this->mc_ErrorManager.OnErrorChange();
      Q_EMIT this->dataChanged(c_Index, c_Index);
   }
   if (s32_Column >= 0)
   {
      Q_EMIT this->dataChanged(this->index(oru32_Row, s32_Column), this->index(oru32_Row, s32_Column));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update column count after data set insertion

   \param[in]  oru32_DataSetIndex   Inserted data set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::DoInsertDataSet(const uint32 & oru32_DataSetIndex)
{
   sint32 s32_Index = this->EnumToColumn(eDATA_SET);

   if (s32_Index >= 0)
   {
      const sintn sn_Column = static_cast<sintn>(s32_Index) + static_cast<sintn>(oru32_DataSetIndex);
      this->insertColumn(sn_Column);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update column count after data set deletion

   \param[in]  oru32_DataSetIndex   Deleted data set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::DoDeleteDataSet(const uint32 & oru32_DataSetIndex)
{
   sint32 s32_Index = this->EnumToColumn(eDATA_SET);

   if (s32_Index >= 0)
   {
      this->removeColumn(static_cast<sintn>(s32_Index) + static_cast<sintn>(oru32_DataSetIndex));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register model data reset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::Reset(void)
{
   this->beginResetModel();
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check name

   \param[in]  oru32_Index    Element index
   \param[in]  orc_Name       Name

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::CheckName(const uint32 & oru32_Index, const QString & orc_Name) const
{
   return C_PuiSdHandler::h_GetInstance()->CheckNodeDataPoolListElementNameAvailable(this->mu32_NodeIndex,
                                                                                     this->mu32_DataPoolIndex,
                                                                                     this->mu32_ListIndex,
                                                                                     orc_Name.toStdString().c_str(),
                                                                                     &oru32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get array size of specified element

   \param[in]  oru32_Index    Element index

   \return
   0    Invalid
   Else Current array size
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNdeDpListTableModel::GetArraySize(const uint32 & oru32_Index) const
{
   uint32 u32_Retval = 0;
   const C_OSCNodeDataPoolListElement * const pc_Element = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex, oru32_Index);

   if (pc_Element != NULL)
   {
      u32_Retval = pc_Element->GetArraySize();
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if row is of type string

   \param[in]  orc_Index   Model index

   \return
   True  Row is of type string
   False Row is of other type
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::IsString(const QModelIndex & orc_Index) const
{
   bool q_Retval;
   const QModelIndex c_Index =
      this->index(orc_Index.row(), this->EnumToColumn(C_SdNdeDpListTableModel::eVALUE_TYPE));

   q_Retval = (this->data(c_Index, static_cast<sintn>(Qt::EditRole)) == 10);
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if item needs link

   \param[in]  orc_Index   Index to check for link

   \return
   True  Link
   False Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::m_CheckLink(const QModelIndex & orc_Index) const
{
   bool q_Retval = false;

   if (orc_Index.row() >= 0)
   {
      const uint32 u32_Index = static_cast<uint32>(orc_Index.row());
      const C_PuiSdNodeDataPoolListElement * const pc_UIElement =
         C_PuiSdHandler::h_GetInstance()->GetUIDataPoolListElement(this->mu32_NodeIndex,
                                                                   this->mu32_DataPoolIndex,
                                                                   this->mu32_ListIndex, u32_Index);
      const C_OSCNodeDataPoolListElement * const pc_OSCElement =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mu32_NodeIndex,
                                                                    this->mu32_DataPoolIndex,
                                                                    this->mu32_ListIndex, u32_Index);
      if ((pc_OSCElement != NULL) && (pc_UIElement != NULL))
      {
         if ((pc_UIElement->q_InterpretAsString == false) && (pc_OSCElement->GetArray() == true))
         {
            //Special link handling
            q_Retval = true;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal error state change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::m_OnErrorChange(void)
{
   Q_EMIT this->SigErrorChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move items to target index

   \param[in]  orc_ContiguousIndices   Contiguous section of ascending indices
   \param[in]  ou32_TargetIndex        Target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::m_MoveItems(const std::vector<uint32> & orc_ContiguousIndices,
                                          const uint32 ou32_TargetIndex) const
{
   if (orc_ContiguousIndices.size() > 0UL)
   {
      std::vector<uint32> c_ContiguousIndicesCopy = orc_ContiguousIndices;
      bool q_Forward;
      if (c_ContiguousIndicesCopy[0UL] < ou32_TargetIndex)
      {
         q_Forward = true;
      }
      else
      {
         q_Forward = false;
      }
      for (uint32 u32_ItItem = 0UL; u32_ItItem < c_ContiguousIndicesCopy.size(); ++u32_ItItem)
      {
         if (q_Forward == true)
         {
            //There should always be a "new item" at the same position
            this->m_MoveItem(orc_ContiguousIndices[0UL], ou32_TargetIndex);
         }
         else
         {
            //The item position should not change but the target will
            this->m_MoveItem(orc_ContiguousIndices[u32_ItItem], ou32_TargetIndex + u32_ItItem);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move specific item

   For implementation: First delete ou32_SourceIndex then insert ou32_TargetIndex
   Warning: not expected to fail

   \param[in]  ou32_SourceIndex  Source index
   \param[in]  ou32_TargetIndex  Target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::m_MoveItem(const uint32 ou32_SourceIndex, const uint32 ou32_TargetIndex) const
{
   C_PuiSdHandler::h_GetInstance()->MoveDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                            this->mu32_ListIndex, ou32_SourceIndex, ou32_TargetIndex);
}
