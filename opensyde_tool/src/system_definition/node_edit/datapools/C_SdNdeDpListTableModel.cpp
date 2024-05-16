//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list table model (implementation)

   Node data pool list table model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QMimeData>
#include <QIcon>

#include "C_SdNdeDpContentUtil.hpp"
#include "C_SdNdeDpListTableModel.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_SdClipBoardHelper.hpp"
#include "C_Uti.hpp"
#include "C_SdUtil.hpp"
#include "C_NagToolTip.hpp"
#include "C_SdNdeUnoUtil.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::errors;

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
   mu32_ListIndex(0),
   mq_DpIsSafety(false)
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
void C_SdNdeDpListTableModel::SetList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                      const uint32_t & oru32_ListIndex)
{
   beginResetModel();
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->mu32_ListIndex = oru32_ListIndex;
   this->m_FillDpListInfo();
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
bool C_SdNdeDpListTableModel::Equals(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                     const uint32_t & oru32_ListIndex) const
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

   \param[in]  os32_Section       Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role          Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpListTableModel::headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                                             const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      int32_t s32_DataSetIndex;
      const C_SdNdeDpListTableModel::E_Columns e_Col = ColumnToEnum(os32_Section, &s32_DataSetIndex);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
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
               const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
                  this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
               if (pc_List != NULL)
               {
                  if (static_cast<uint32_t>(s32_DataSetIndex) < pc_List->c_DataSets.size())
                  {
                     c_Retval = static_cast<QString>(C_GtGetText::h_GetText("Dataset -\n")) +
                                static_cast<QString>(pc_List->c_DataSets[s32_DataSetIndex].c_Name.c_str());
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
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
         // header font etc. are styled via stylesheets of class C_SdNdeSingleHeaderView
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
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
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
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
int32_t C_SdNdeDpListTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      s32_Retval = this->mc_DpListInfoAll.size();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get table column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeDpListTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
      if (pc_List != NULL)
      {
         //For table parent should always be invalid
         s32_Retval = 17 + static_cast<int32_t>(pc_List->c_DataSets.size());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeDpListTableModel::data(const QModelIndex & orc_Index, const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      int32_t s32_DataSetIndex;
      const C_SdNdeDpListTableModel::E_Columns e_Col = ColumnToEnum(orc_Index.column(), &s32_DataSetIndex);
      const uint32_t u32_DataSetIndex = static_cast<uint32_t>(s32_DataSetIndex);
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
         if (u32_Index < this->mc_DpListInfoAll.size())
         {
            const C_DpListTableData & rc_Data = this->mc_DpListInfoAll[u32_Index];
            switch (e_Col)
            {
            case eINVALID:
               //No data
               break;
            case eINDEX:
               c_Retval = orc_Index.row() + 1;
               break;
            case eNAME:
               c_Retval = rc_Data.c_Name;
               break;
            case eCOMMENT:
               c_Retval = rc_Data.c_Comment;
               break;
            case eVALUE_TYPE:
               if (os32_Role == static_cast<int32_t>(Qt::ItemDataRole::EditRole))
               {
                  c_Retval = rc_Data.c_ValueTypeEdit;
               }
               else
               {
                  c_Retval = rc_Data.c_ValueType;
               }
               break;
            case eARRAY_SIZE:
               if (os32_Role == static_cast<int32_t>(Qt::EditRole))
               {
                  c_Retval = rc_Data.c_ArraySizeEdit;
               }
               else
               {
                  c_Retval = rc_Data.c_ArraySize;
               }
               break;
            case eMIN:
               if (os32_Role == static_cast<int32_t>(Qt::EditRole))
               {
                  c_Retval = rc_Data.c_MinEdit;
               }
               else
               {
                  c_Retval = rc_Data.c_Min;
               }

               break;
            case eMAX:
               if (os32_Role == static_cast<int32_t>(Qt::EditRole))
               {
                  c_Retval = rc_Data.c_MaxEdit;
               }
               else
               {
                  c_Retval = rc_Data.c_Max;
               }

               break;
            case eFACTOR:
               c_Retval = rc_Data.c_Factor;
               break;
            case eOFFSET:
               c_Retval = rc_Data.c_Offset;
               break;
            case eUNIT:
               c_Retval = rc_Data.c_Unit;
               break;
            case eDATA_SET:
               if ((s32_DataSetIndex >= 0) &&
                   (u32_DataSetIndex < rc_Data.c_DataSetData.size()))
               {
                  if (os32_Role == static_cast<int32_t>(Qt::EditRole))
                  {
                     c_Retval = rc_Data.c_DataSetData[u32_DataSetIndex].c_DataSetEdit;
                  }
                  else
                  {
                     c_Retval = rc_Data.c_DataSetData[u32_DataSetIndex].c_DataSet;
                  }
               }
               break;
            case eACCESS:
               if (os32_Role == static_cast<int32_t>(Qt::EditRole))
               {
                  c_Retval = rc_Data.c_AccessEdit;
               }
               else
               {
                  c_Retval = rc_Data.c_Access;
               }
               break;
            case eDATA_SIZE:
               c_Retval = rc_Data.c_DataSize;
               break;
            case eADDRESS:
               c_Retval = rc_Data.c_Address;
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
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         if (e_Col == eINVALID)
         {
            const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
            if (u32_Index < this->mc_DpListInfoAll.size())
            {
               const C_DpListTableData & rc_Data = this->mc_DpListInfoAll[u32_Index];

               c_Retval = rc_Data.c_InvalidToolTipHeading;
            }
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_TYPE)
      {
         if (e_Col == eINVALID)
         {
            c_Retval = static_cast<int32_t>(C_NagToolTip::eERROR);
         }
         else
         {
            c_Retval = static_cast<int32_t>(C_NagToolTip::eDEFAULT);
            //No tool tip
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
         if (u32_Index < this->mc_DpListInfoAll.size())
         {
            const C_DpListTableData & rc_Data = this->mc_DpListInfoAll[u32_Index];

            switch (e_Col) //lint !e788 //not all columns get a tooltip
            {
            case eINVALID:
               c_Retval = rc_Data.c_InvalidToolTipContent;
               break;
            case eCOMMENT:
               c_Retval = rc_Data.c_Comment;
               break;
            case eMIN:
               c_Retval = rc_Data.c_MinToolTipContent;
               break;
            case eMAX:
               c_Retval = rc_Data.c_MaxToolTipContent;
               break;
            case eDATA_SET:
               if ((s32_DataSetIndex >= 0) &&
                   (u32_DataSetIndex < rc_Data.c_DataSetData.size()))
               {
                  c_Retval = rc_Data.c_DataSetData[u32_DataSetIndex].c_DataSetToolTipContent;
               }
               break;
            default:
               //No tool tip
               break;
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
      {
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
         if (u32_Index < this->mc_DpListInfoAll.size())
         {
            const C_DpListTableData & rc_Data = this->mc_DpListInfoAll[u32_Index];
            switch (e_Col) //lint !e788 //not all columns handled on purpose
            {
            case eAUTO_MIN_MAX:
               c_Retval = rc_Data.s32_AutoMinMaxCheckState;
               break;
            case eEVENT_CALL:
               c_Retval = rc_Data.s32_EventCallCheckState;
               break;
            default:
               break;
            }
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::FontRole))
      {
         QFont c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
         //Stylesheets do not allow access of specific columns so we need to set fonts manually
         switch (e_Col)
         {
         case eDATA_SET:
            {
               const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
               if (u32_Index < this->mc_DpListInfoAll.size())
               {
                  c_Font = this->mc_DpListInfoAll[u32_Index].c_DataSetFont;
               }
            }
            break;
         case eMIN:
         case eMAX:
            {
               const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
               if (u32_Index < this->mc_DpListInfoAll.size())
               {
                  c_Font = this->mc_DpListInfoAll[u32_Index].c_MinMaxFont;
               }
            }
            break;
         case eNAME:
         case eCOMMENT:
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
            // no special font adaption in these cases
            break;
         }
         //Convert point to pixel
         c_Font.setPixelSize(c_Font.pointSize());
         c_Retval = c_Font;
      }
      else if (os32_Role == static_cast<int32_t>(Qt::ForegroundRole))
      {
         //Stylesheets do not allow access of specific columns so we need to set t manually
         switch (e_Col)
         {
         case eCOMMENT:
            c_Retval = mc_STYLE_GUIDE_COLOR_6;
            break;
         case eMIN:
         case eMAX:
            {
               const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
               if (u32_Index < this->mc_DpListInfoAll.size())
               {
                  c_Retval = this->mc_DpListInfoAll[u32_Index].c_MinMaxForeground;
               }
            }
            break;
         case eDATA_SIZE:
         case eADDRESS:
            c_Retval = mc_STYLE_GUIDE_COLOR_10;
            break;
         //Error
         case eNAME:
            {
               const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
               if (u32_Index < this->mc_DpListInfoAll.size())
               {
                  c_Retval = this->mc_DpListInfoAll[u32_Index].c_NameForeground;
               }
            }
            break;
         case eDATA_SET:
            if (s32_DataSetIndex >= 0)
            {
               const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
               if (u32_Index < this->mc_DpListInfoAll.size())
               {
                  const C_DpListTableData & rc_Data = this->mc_DpListInfoAll[u32_Index];

                  if (u32_DataSetIndex < rc_Data.c_DataSetData.size())
                  {
                     c_Retval = rc_Data.c_DataSetData[u32_DataSetIndex].c_DataSetForeground;
                  }
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
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (os32_Role == ms32_USER_ROLE_ICON)
      {
         const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
         QStringList c_Tmp;
         switch (e_Col)
         {
         case eINVALID:
            if (u32_Index < this->mc_DpListInfoAll.size())
            {
               const C_DpListTableData & rc_Data = this->mc_DpListInfoAll[u32_Index];
               c_Tmp = rc_Data.c_InvalidIconRole;
            }
            break;
         case eICON:
            if (u32_Index < this->mc_DpListInfoAll.size())
            {
               const C_DpListTableData & rc_Data = this->mc_DpListInfoAll[u32_Index];
               c_Tmp = rc_Data.c_IconIconRole;
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
   \param[in]  os32_Role    Data role

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                      const int32_t os32_Role)
{
   bool q_Retval = false;

   if (data(orc_Index, os32_Role) != orc_Value)
   {
      if (orc_Index.row() >= 0)
      {
         if (os32_Role == static_cast<int32_t>(Qt::EditRole))
         {
            const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
            int32_t s32_DataSetIndex;
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
            case eICON:
               //No edit
               break;
            case eNAME:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                  this->mu32_ListIndex,
                                  u32_Index, orc_Value,
                                  C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_NAME,
                                  0, 0);
               q_Retval = true;
               break;
            case eCOMMENT:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                  this->mu32_ListIndex,
                                  u32_Index, orc_Value,
                                  C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_COMMENT,
                                  0, 0);
               q_Retval = true;
               break;
            case eVALUE_TYPE:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                  this->mu32_ListIndex,
                                  u32_Index, orc_Value,
                                  C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_VALUE_TYPE,
                                  0, 0);
               q_Retval = true;
               break;
            case eARRAY_SIZE:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
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
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                  this->mu32_ListIndex,
                                  u32_Index, orc_Value,
                                  C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_MIN,
                                  0, 0);
               q_Retval = true;
               break;
            case eMAX:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                  this->mu32_ListIndex,
                                  u32_Index, orc_Value,
                                  C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_MAX,
                                  0, 0);
               q_Retval = true;
               break;
            case eFACTOR:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                  this->mu32_ListIndex,
                                  u32_Index, orc_Value,
                                  C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_FACTOR,
                                  0, 0);
               q_Retval = true;
               break;
            case eOFFSET:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                  this->mu32_ListIndex,
                                  u32_Index, orc_Value,
                                  C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_OFFSET,
                                  0, 0);
               q_Retval = true;
               break;
            case eUNIT:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                  this->mu32_ListIndex,
                                  u32_Index, orc_Value,
                                  C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_UNIT,
                                  0, 0);
               q_Retval = true;
               break;
            case eDATA_SET:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                  this->mu32_ListIndex,
                                  u32_Index, orc_Value,
                                  C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_DATA_SET,
                                  0, s32_DataSetIndex);
               q_Retval = true;
               break;
            case eACCESS:
               this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
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
            case eUNKNOWN:
               //No edit
               break;
            default:
               break;
            }
         }
         else if (os32_Role == static_cast<int32_t>(Qt::CheckStateRole))
         {
            const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
            const C_SdNdeDpListTableModel::E_Columns e_Col = this->ColumnToEnum(orc_Index.column());
            if (orc_Value.type() == QVariant::Int)
            {
               const Qt::CheckState e_CheckedState = static_cast<Qt::CheckState>(orc_Value.toInt());
               bool q_Val;
               if (e_CheckedState == Qt::Checked)
               {
                  q_Val = true;
               }
               else
               {
                  q_Val = false;
               }
               switch (e_Col) //lint !e788 //not all columns handled on purpose
               {
               case eAUTO_MIN_MAX:
                  this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                     this->mu32_ListIndex,
                                     u32_Index, static_cast<QVariant>(q_Val),
                                     C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_AUTO_MIN_MAX,
                                     0, 0);
                  q_Retval = true;
                  break;
               case eEVENT_CALL:
                  this->m_DataChange(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                     this->mu32_ListIndex,
                                     u32_Index, static_cast<QVariant>(q_Val),
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
         Q_EMIT this->dataChanged(orc_Index, orc_Index, QVector<int32_t>() << os32_Role);
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
         if (this->mq_DpIsSafety == false)
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
         {
            const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
            if (u32_Index < this->mc_DpListInfoAll.size())
            {
               const C_DpListTableData & rc_Data = this->mc_DpListInfoAll[u32_Index];

               if (rc_Data.q_InterpretAsString == true)
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
         }
         break;
      case eMIN:
      case eMAX:
         {
            const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
            if (u32_Index < this->mc_DpListInfoAll.size())
            {
               const C_DpListTableData & rc_Data = this->mc_DpListInfoAll[u32_Index];

               if ((rc_Data.q_AutoMinMaxActive == true) ||
                   (rc_Data.q_InterpretAsString == true))
               {
                  q_Edit = false;
               }
            }

            if (q_Edit == true)
            {
               c_Retval = c_Retval | Qt::ItemIsEditable | Qt::ItemIsEnabled;
            }
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

   \param[in]  orc_OscInsertedElements    Inserted elements OSC content
   \param[in]  orc_UiInsertedElements     Inserted elements UI content
   \param[in]  orc_Rows                   Rows (Expected: Sorted ascending!)

   \return
   Continuous sections of new elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::vector<uint32_t> > C_SdNdeDpListTableModel::DoInsertRows(
   const std::vector<C_OscNodeDataPoolListElement> & orc_OscInsertedElements,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiInsertedElements, const std::vector<uint32_t> & orc_Rows)
{
   std::vector<std::vector<uint32_t> > c_Retval;

   if ((orc_OscInsertedElements.size() == orc_UiInsertedElements.size()) &&
       (orc_OscInsertedElements.size() == orc_Rows.size()))
   {
      uint32_t u32_Counter = 0UL;
      c_Retval = C_Uti::h_GetContiguousSectionsAscending(orc_Rows);
      C_PuiSdHandler::h_GetInstance()->ReserveDataPoolListElements(this->mu32_NodeIndex,
                                                                   this->mu32_DataPoolIndex,
                                                                   this->mu32_ListIndex,
                                                                   orc_OscInsertedElements.size());
      for (uint32_t u32_ItSection = 0UL; u32_ItSection < c_Retval.size(); ++u32_ItSection)
      {
         const std::vector<uint32_t> & rc_Section = c_Retval[u32_ItSection];
         if (rc_Section.size() > 0UL)
         {
            this->beginInsertRows(QModelIndex(), rc_Section[0UL],
                                  rc_Section[static_cast<std::vector<uint32_t>::size_type>(rc_Section.size() - 1UL)]);
            for (uint32_t u32_ItItem = 0UL; u32_ItItem < rc_Section.size(); ++u32_ItItem)
            {
               C_PuiSdHandler::h_GetInstance()->InsertDataPoolListElement(this->mu32_NodeIndex,
                                                                          this->mu32_DataPoolIndex,
                                                                          this->mu32_ListIndex, rc_Section[u32_ItItem],
                                                                          orc_OscInsertedElements[u32_Counter],
                                                                          orc_UiInsertedElements[u32_Counter]);
               ++u32_Counter;
            }
            this->endInsertRows();
         }
      }
      this->mc_ErrorManager.OnErrorChange();
      this->m_FillDpListInfo();
      Q_EMIT this->SigSizeChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert items into model

   \param[in]  os32_Col     Starting column
   \param[in]  os32_Count   Number of inserted items
   \param[in]  orc_Parent  Parent

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::insertColumns(const int32_t os32_Col, const int32_t os32_Count,
                                            const QModelIndex & orc_Parent)
{
   bool q_Retval = false;

   if ((os32_Count > 0) && (os32_Col >= 0))
   {
      this->m_FillDpListInfo();
      beginInsertColumns(orc_Parent, os32_Col, (os32_Col + os32_Count) - 1);
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
void C_SdNdeDpListTableModel::DoRemoveRows(const std::vector<uint32_t> & orc_Rows)
{
   const std::vector<std::vector<uint32_t> > c_ContiguousSections = C_Uti::h_GetContiguousSectionsAscending(orc_Rows);

   //Start deleting from back (easier to keep indices valid)
   for (uint32_t u32_ItSection = c_ContiguousSections.size(); u32_ItSection > 0UL; --u32_ItSection)
   {
      const std::vector<uint32_t> & rc_Section =
         c_ContiguousSections[static_cast<std::vector<uint32_t>::size_type>(u32_ItSection - 1UL)];
      if (rc_Section.size() > 0UL)
      {
         this->beginRemoveRows(QModelIndex(), rc_Section[0UL],
                               rc_Section[static_cast<std::vector<uint32_t>::size_type>(rc_Section.size() - 1UL)]);
         for (uint32_t u32_ItItem = rc_Section.size(); u32_ItItem > 0UL; --u32_ItItem)
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->RemoveDataPoolListElement(this->mu32_NodeIndex,
                                                                                  this->mu32_DataPoolIndex,
                                                                                  this->mu32_ListIndex,
                                                                                  rc_Section[static_cast<std::vector<uint32_t>
                                                                                                         ::size_type>(
                                                                                                u32_ItItem - 1UL)]) ==
                       C_NO_ERR);
         }
         this->endRemoveRows();
      }
   }
   this->mc_ErrorManager.OnErrorChange();
   this->m_FillDpListInfo();
   Q_EMIT this->SigSizeChangePossible(this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove columns from model

   \param[in]  os32_Col     Starting column
   \param[in]  os32_Count   Number of removed items
   \param[in]  orc_Parent  Parent

   \return
   true  success
   false failure
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::removeColumns(const int32_t os32_Col, const int32_t os32_Count,
                                            const QModelIndex & orc_Parent)
{
   bool q_Retval = false;

   if ((os32_Count > 0) && (os32_Col >= 0))
   {
      this->m_FillDpListInfo();
      beginRemoveColumns(orc_Parent, os32_Col, (os32_Col + os32_Count) - 1);
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
void C_SdNdeDpListTableModel::DoMoveRows(const std::vector<uint32_t> & orc_SelectedIndices,
                                         const std::vector<uint32_t> & orc_TargetIndices)
{
   if (orc_SelectedIndices.size() == orc_TargetIndices.size())
   {
      std::vector<uint32_t> c_SelectedIndicesCopy = orc_SelectedIndices;
      std::vector<uint32_t> c_TargetIndicesCopy = orc_TargetIndices;
      std::vector<std::vector<uint32_t> > c_ContiguousSections;
      uint32_t u32_TargetAccessIndex = 0UL;

      //Step 1 sort (so the next step can assume the contiguous selection has the same order!
      C_Uti::h_SortIndicesAscendingAndSync(c_SelectedIndicesCopy, c_TargetIndicesCopy);

      //Step 2: get contiguous selection
      c_ContiguousSections = C_Uti::h_GetContiguousSectionsAscending(orc_SelectedIndices);

      //Step 3: move
      for (uint32_t u32_ItSection = 0UL; u32_ItSection < c_ContiguousSections.size(); ++u32_ItSection)
      {
         const std::vector<uint32_t> & rc_Section = c_ContiguousSections[u32_ItSection];
         if (rc_Section.size() > 0UL)
         {
            uint32_t u32_TargetIndex = c_TargetIndicesCopy[u32_TargetAccessIndex];
            uint32_t u32_TargetIndexParam = c_TargetIndicesCopy[u32_TargetAccessIndex];
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
                                rc_Section[static_cast<std::vector<uint32_t>::size_type>(rc_Section.size() - 1UL)],
                                QModelIndex(), u32_TargetIndex);
            this->m_MoveItems(rc_Section, u32_TargetIndexParam);
            this->endMoveRows();
            u32_TargetAccessIndex += rc_Section.size();
         }
      }
      this->m_FillDpListInfo();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]   ors32_Column          Column
   \param[out]  ops32_DataSetIndex    Optional: If column is a data set, this contains the data set index
                                      Else -1

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListTableModel::E_Columns C_SdNdeDpListTableModel::ColumnToEnum(const int32_t & ors32_Column,
                                                                         int32_t * const ops32_DataSetIndex)
const
{
   C_SdNdeDpListTableModel::E_Columns e_Retval;
   if (ops32_DataSetIndex != NULL)
   {
      *ops32_DataSetIndex = -1;
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
      if (ors32_Column == static_cast<int32_t>(this->columnCount() - 1))
      {
         e_Retval = eEVENT_CALL;
      }
      else if (ors32_Column == static_cast<int32_t>(this->columnCount() - 2))
      {
         e_Retval = eADDRESS;
      }
      else if (ors32_Column == static_cast<int32_t>(this->columnCount() - 3))
      {
         e_Retval = eDATA_SIZE;
      }
      else if (ors32_Column == static_cast<int32_t>(this->columnCount() - 4))
      {
         e_Retval = eACCESS;
      }
      else
      {
         e_Retval = eDATA_SET;
         if (ops32_DataSetIndex != NULL)
         {
            *ops32_DataSetIndex = ors32_Column - 13;
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
int32_t C_SdNdeDpListTableModel::EnumToColumn(const C_SdNdeDpListTableModel::E_Columns & ore_Value) const
{
   int32_t s32_Retval;

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
      s32_Retval = static_cast<int32_t>(this->columnCount() - 4);
      break;
   case eDATA_SIZE:
      s32_Retval = static_cast<int32_t>(this->columnCount() - 3);
      break;
   case eADDRESS:
      s32_Retval = static_cast<int32_t>(this->columnCount() - 2);
      break;
   case eEVENT_CALL:
      s32_Retval = static_cast<int32_t>(this->columnCount() - 1);
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
         const C_PuiSdNodeDataPoolList * const pc_UiList = C_PuiSdHandler::h_GetInstance()->GetUiDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         const C_OscNodeDataPoolList * const pc_OscList = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);
         const C_OscNodeDataPool * const pc_OscDatapool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
            this->mu32_NodeIndex, this->mu32_DataPoolIndex);
         if (((pc_UiList != NULL) && (pc_OscList != NULL)) && (pc_OscDatapool != NULL))
         {
            std::vector<uint32_t> c_Rows = C_SdNdeDpUtil::h_ConvertVector(orc_Indices, true);
            QString c_String;
            std::vector<C_OscNodeDataPoolListElement> c_OscElements;
            std::vector<C_PuiSdNodeDataPoolListElement> c_UiElements;
            //Convert indices to unique rows
            C_Uti::h_Uniqueify(c_Rows);
            //Reserve
            c_OscElements.reserve(c_Rows.size());
            c_UiElements.reserve(c_Rows.size());
            //Copy relevant data
            for (uint32_t u32_ItIndex = 0; u32_ItIndex < c_Rows.size(); ++u32_ItIndex)
            {
               c_OscElements.push_back(pc_OscList->c_Elements.at(c_Rows[u32_ItIndex]));
               c_UiElements.push_back(pc_UiList->c_DataPoolListElements.at(c_Rows[u32_ItIndex]));
            }
            //Store in mime
            pc_Retval = new QMimeData();
            //Use default mime type so qt accepts these
            pc_Retval->setData(this->mimeTypes().at(0), "");
            C_SdClipBoardHelper::h_StoreDataPoolListElementsToString(c_OscElements, c_UiElements,
                                                                     pc_OscDatapool->e_Type, c_String);
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
const C_OscNodeDataPoolListElement * C_SdNdeDpListTableModel::GetOscElement(const QModelIndex & orc_Index)
const
{
   return C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                     this->mu32_ListIndex,
                                                                     static_cast<uint32_t>(orc_Index.row()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node data pool list element interpreted as string flag from model

   \param[in]  orc_Index   Table index

   \return
   true     Is interpreted as string
   false    Is not interpreted as string
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::IsElementInterpretedAsString(const QModelIndex & orc_Index) const
{
   bool q_Return = false;

   if (orc_Index.row() >= 0)
   {
      const uint32_t u32_Index = static_cast<uint32_t>(orc_Index.row());
      if (u32_Index < this->mc_DpListInfoAll.size())
      {
         q_Return = this->mc_DpListInfoAll[u32_Index].q_InterpretAsString;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal model update

   \param[in]  ou32_Row             Changed row
   \param[in]  ore_ChangeType       Change type
   \param[in]  os32_DataSetIndex    Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::HandleDataChange(const uint32_t ou32_Row,
                                               const C_SdNdeDpUtil::E_ElementDataChangeType & ore_ChangeType,
                                               const int32_t os32_DataSetIndex)
{
   bool q_ErrorChange = false;
   bool q_ReloadAll = false;
   int32_t s32_Column = -1;

   switch (ore_ChangeType)
   {
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_ACCESS:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eACCESS);
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_NAME:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eNAME);
      q_ErrorChange = true;
      q_ReloadAll = true;
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
      //Size!
      q_ReloadAll = true;
      break;
   case C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_DATA_SET:
      s32_Column = this->EnumToColumn(C_SdNdeDpListTableModel::eDATA_SET);
      if (os32_DataSetIndex >= 0)
      {
         s32_Column += os32_DataSetIndex;
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
      //Size!
      q_ReloadAll = true;
      break;
   default:
      //No handling
      break;
   }

   if (q_ReloadAll == false)
   {
      this->m_FillDpListElementInfo(ou32_Row);
   }
   else
   {
      this->m_FillDpListInfo();
   }

   if (q_ErrorChange == true)
   {
      const QModelIndex c_Index = this->index(ou32_Row, this->EnumToColumn(eINVALID));
      this->mc_ErrorManager.OnErrorChange();
      Q_EMIT this->dataChanged(c_Index, c_Index);
   }
   if (s32_Column >= 0)
   {
      Q_EMIT this->dataChanged(this->index(ou32_Row, s32_Column), this->index(ou32_Row, s32_Column));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update column count after data set insertion

   \param[in]  ou32_DataSetIndex   Inserted data set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::DoInsertDataSet(const uint32_t ou32_DataSetIndex)
{
   const int32_t s32_Index = this->EnumToColumn(eDATA_SET);
   const int32_t s32_Column = static_cast<int32_t>(s32_Index) + static_cast<int32_t>(ou32_DataSetIndex);

   this->insertColumn(s32_Column);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update column count after data set deletion

   \param[in]  ou32_DataSetIndex   Deleted data set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::DoDeleteDataSet(const uint32_t ou32_DataSetIndex)
{
   const int32_t s32_Index = this->EnumToColumn(eDATA_SET);

   this->removeColumn(static_cast<int32_t>(s32_Index) + static_cast<int32_t>(ou32_DataSetIndex));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register model data reset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::Reset(void)
{
   this->beginResetModel();
   this->m_FillDpListInfo();
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check name

   \param[in]  ou32_Index     Element index
   \param[in]  orc_Name       Name

   \return
   true  Available
   false Already in use
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::CheckName(const uint32_t ou32_Index, const QString & orc_Name) const
{
   return C_PuiSdHandler::h_GetInstance()->CheckNodeDataPoolListElementNameAvailable(this->mu32_NodeIndex,
                                                                                     this->mu32_DataPoolIndex,
                                                                                     this->mu32_ListIndex,
                                                                                     orc_Name.toStdString().c_str(),
                                                                                     &ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get array size of specified element

   \param[in]  ou32_Index    Element index

   \return
   0    Invalid
   Else Current array size
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDpListTableModel::GetArraySize(const uint32_t ou32_Index) const
{
   uint32_t u32_Retval = 0;

   if (ou32_Index < this->mc_DpListInfoAll.size())
   {
      u32_Retval = this->mc_DpListInfoAll[ou32_Index].u32_ArraySize;
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

   q_Retval = (this->data(c_Index, static_cast<int32_t>(Qt::EditRole)) == 10);
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Collects all Datapool list information for table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::m_FillDpListInfo(void)
{
   this->mc_DpListInfoAll.clear();

   const C_OscNodeDataPool * const pc_Datapool =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPool(this->mu32_NodeIndex, this->mu32_DataPoolIndex);

   if ((pc_Datapool != NULL) &&
       (this->mu32_ListIndex < pc_Datapool->c_Lists.size()))
   {
      uint32_t u32_ElementCounter;

      this->mq_DpIsSafety = pc_Datapool->q_IsSafety;

      this->mc_DpListInfoAll.resize(pc_Datapool->c_Lists[this->mu32_ListIndex].c_Elements.size());

      for (u32_ElementCounter = 0U; u32_ElementCounter < pc_Datapool->c_Lists[this->mu32_ListIndex].c_Elements.size();
           ++u32_ElementCounter)
      {
         this->m_FillDpListElementInfo(u32_ElementCounter);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Collects all Datapool list element information for one row of table and updates the data

   \param[in]       ou32_ElementIndex     Datapool element index which equals the row
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::m_FillDpListElementInfo(const uint32_t ou32_ElementIndex)
{
   const C_PuiSdNodeDataPoolListElement * const pc_UiElement =
      C_PuiSdHandler::h_GetInstance()->GetUiDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                                this->mu32_ListIndex, ou32_ElementIndex);
   const C_OscNodeDataPoolListElement * const pc_OscElement =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mu32_NodeIndex,
                                                                 this->mu32_DataPoolIndex,
                                                                 this->mu32_ListIndex, ou32_ElementIndex);

   const C_OscNodeDataPool * const pc_Datapool =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPool(this->mu32_NodeIndex, this->mu32_DataPoolIndex);

   if ((pc_Datapool != NULL) && (pc_OscElement != NULL) && (pc_UiElement != NULL) &&
       (this->mu32_ListIndex < pc_Datapool->c_Lists.size()))
   {
      C_DpListTableData & rc_Data = this->mc_DpListInfoAll[ou32_ElementIndex];
      const bool q_HasLink = this->m_CheckLink(pc_UiElement, pc_OscElement);
      uint32_t u32_SizeByte;
      uint32_t u32_DataSetCounter;
      bool q_NameConflict = false;
      bool q_NameInvalid = false;
      bool q_MinOverMax = false;
      bool q_DataSetInvalid = false;
      C_OscNodeDataPool::E_Type e_DataPoolType;

      std::vector<uint32_t> c_InvalidDataSetIndices;
      const C_OscNodeDataPoolList & rc_List = pc_Datapool->c_Lists[this->mu32_ListIndex];

      // Reset error icon
      rc_Data.c_InvalidIconRole.clear();

      rc_List.CheckErrorElement(ou32_ElementIndex, &q_NameConflict, &q_NameInvalid, &q_MinOverMax, &q_DataSetInvalid,
                                &c_InvalidDataSetIndices);

      // Name
      C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OscElement, *pc_UiElement,
                                               C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_NAME,
                                               rc_Data.c_Name, 0, 0);

      if ((q_NameConflict == true) || (q_NameInvalid == true))
      {
         // error
         rc_Data.c_NameForeground = mc_STYLE_GUIDE_COLOR_18;
      }
      else
      {
         //Default
         rc_Data.c_NameForeground = mc_STYLE_GUIDE_COLOR_6;
      }

      // Comment
      C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OscElement, *pc_UiElement,
                                               C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_COMMENT,
                                               rc_Data.c_Comment, 0, 0);

      // Value Type
      if (pc_UiElement->q_InterpretAsString == true)
      {
         rc_Data.c_ValueType = C_GtGetText::h_GetText("string");
      }
      else
      {
         rc_Data.c_ValueType = C_SdNdeDpUtil::h_ConvertContentTypeToString(pc_OscElement->c_Value.GetType());
      }
      C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OscElement, *pc_UiElement,
                                               C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_VALUE_TYPE,
                                               rc_Data.c_ValueTypeEdit, 0, 0);

      // Array size
      rc_Data.u32_ArraySize = static_cast<int32_t>(pc_OscElement->GetArraySize());
      if (pc_OscElement->GetArray() == false)
      {
         rc_Data.c_ArraySize = "-";
      }
      else
      {
         rc_Data.c_ArraySize = rc_Data.u32_ArraySize;
      }
      C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OscElement, *pc_UiElement,
                                               C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_ARRAY,
                                               rc_Data.c_ArraySizeEdit, 0, 0);

      // Min and max
      if (pc_UiElement->q_InterpretAsString == true)
      {
         // No Edit role in this case
         rc_Data.c_Min = C_SdNdeDpContentUtil::h_ConvertToString(pc_OscElement->c_MinValue);
         rc_Data.c_Max = C_SdNdeDpContentUtil::h_ConvertToString(pc_OscElement->c_MaxValue);
         rc_Data.c_MinEdit = rc_Data.c_Min;
         rc_Data.c_MaxEdit = rc_Data.c_Max;
         // No tool tip
         rc_Data.c_MinToolTipContent.clear();
         rc_Data.c_MaxToolTipContent.clear();
      }
      else
      {
         if ((pc_OscElement->GetArray() == false) || (pc_UiElement->q_AutoMinMaxActive == true))
         {
            // Min
            rc_Data.c_Min = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
               pc_OscElement->c_MinValue,
               pc_OscElement->f64_Factor,
               pc_OscElement->f64_Offset,
               0, true, static_cast<int32_t>(Qt::DisplayRole));

            rc_Data.c_MinEdit = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
               pc_OscElement->c_MinValue,
               pc_OscElement->f64_Factor,
               pc_OscElement->f64_Offset,
               0, true, static_cast<int32_t>(Qt::EditRole));

            // Max
            rc_Data.c_Max = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
               pc_OscElement->c_MaxValue,
               pc_OscElement->f64_Factor,
               pc_OscElement->f64_Offset,
               0, true, static_cast<int32_t>(Qt::DisplayRole));

            rc_Data.c_MaxEdit = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
               pc_OscElement->c_MaxValue,
               pc_OscElement->f64_Factor,
               pc_OscElement->f64_Offset,
               0, true, static_cast<int32_t>(Qt::EditRole));

            // No tool tip
            rc_Data.c_MinToolTipContent.clear();
            rc_Data.c_MaxToolTipContent.clear();
         }
         else
         {
            QString c_Final;
            // Min
            C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(pc_OscElement->c_MinValue,
                                                                    pc_OscElement->f64_Factor,
                                                                    pc_OscElement->f64_Offset,
                                                                    c_Final);
            rc_Data.c_Min = c_Final;
            rc_Data.c_MinEdit = QPoint();
            rc_Data.c_MinToolTipContent = c_Final;

            // Max
            C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(pc_OscElement->c_MaxValue,
                                                                    pc_OscElement->f64_Factor,
                                                                    pc_OscElement->f64_Offset,
                                                                    c_Final);
            rc_Data.c_Max = c_Final;
            rc_Data.c_MaxEdit = QPoint();
            rc_Data.c_MaxToolTipContent = c_Final;
         }
      }

      // Font for min and max
      rc_Data.c_MinMaxFont = mc_STYLE_GUIDE_FONT_REGULAR_12;
      if ((pc_UiElement->q_AutoMinMaxActive == false) &&
          ((pc_UiElement->q_InterpretAsString == false) && (pc_OscElement->GetArray() == true)))
      {
         //Special link handling
         rc_Data.c_MinMaxFont.setUnderline(true);
      }

      // Foreground for min and max
      //Check inactive
      if (pc_UiElement->q_AutoMinMaxActive == true)
      {
         rc_Data.c_MinMaxForeground = mc_STYLE_GUIDE_COLOR_10;
      }
      else if (q_MinOverMax == true)
      {
         //error
         rc_Data.c_MinMaxForeground = mc_STYLE_GUIDE_COLOR_18;
      }
      else if (q_HasLink == true)
      {
         //Link color
         rc_Data.c_MinMaxForeground = mc_STYLE_GUIDE_COLOR_LINK;
      }
      else
      {
         //Default
         rc_Data.c_MinMaxForeground = mc_STYLE_GUIDE_COLOR_6;
      }

      // Various simple elements
      C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OscElement, *pc_UiElement,
                                               C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_FACTOR,
                                               rc_Data.c_Factor, 0, 0);

      C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OscElement, *pc_UiElement,
                                               C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_OFFSET,
                                               rc_Data.c_Offset, 0, 0);

      C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OscElement, *pc_UiElement,
                                               C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_UNIT,
                                               rc_Data.c_Unit, 0, 0);
      if (rc_Data.c_Unit == "")
      {
         rc_Data.c_Unit = "-";
      }

      // Dataset
      rc_Data.c_DataSetData.clear();
      for (u32_DataSetCounter = 0; u32_DataSetCounter < pc_OscElement->c_DataSetValues.size(); ++u32_DataSetCounter)
      {
         C_DpListTableDataSetData c_DataSetData;
         bool q_ValueBelowMin = false;
         bool q_ValueOverMax = false;

         if (pc_UiElement->q_InterpretAsString == true)
         {
            QString c_Result =
               C_SdNdeDpContentUtil::h_ConvertToString(pc_OscElement->c_DataSetValues[u32_DataSetCounter]);
            c_DataSetData.c_DataSetEdit = c_Result;
            c_Result += "\\0";
            c_DataSetData.c_DataSet = c_Result;
            c_DataSetData.c_DataSetToolTipContent = c_Result;
         }
         else
         {
            if (pc_OscElement->GetArray() == false)
            {
               c_DataSetData.c_DataSet = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                  pc_OscElement->c_DataSetValues[u32_DataSetCounter],
                  pc_OscElement->f64_Factor,
                  pc_OscElement->f64_Offset,
                  0, true, static_cast<int32_t>(Qt::DisplayRole));

               c_DataSetData.c_DataSetEdit = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(
                  pc_OscElement->c_DataSetValues[u32_DataSetCounter],
                  pc_OscElement->f64_Factor,
                  pc_OscElement->f64_Offset,
                  0, true, static_cast<int32_t>(Qt::EditRole));

               // No tool tip
               c_DataSetData.c_DataSetToolTipContent.clear();
            }
            else
            {
               QString c_Final;
               C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(
                  pc_OscElement->c_DataSetValues[u32_DataSetCounter],
                  pc_OscElement->f64_Factor,
                  pc_OscElement->f64_Offset,
                  c_Final);
               c_DataSetData.c_DataSet = c_Final;

               //Special link handling
               c_DataSetData.c_DataSetEdit = QPoint();

               c_DataSetData.c_DataSetToolTipContent = c_Final;
            }
         }

         // Foreground for dataset
         rc_List.CheckErrorDataSetValue(ou32_ElementIndex, u32_DataSetCounter,
                                        &q_ValueBelowMin, &q_ValueOverMax, NULL);

         if ((q_ValueOverMax == true) ||
             (q_ValueBelowMin == true))
         {
            c_DataSetData.c_DataSetForeground = mc_STYLE_GUIDE_COLOR_18;
         }
         else if (q_HasLink == true)
         {
            //Link
            c_DataSetData.c_DataSetForeground = mc_STYLE_GUIDE_COLOR_LINK;
         }
         else
         {
            //Default
            c_DataSetData.c_DataSetForeground = mc_STYLE_GUIDE_COLOR_6;
         }

         rc_Data.c_DataSetData.push_back(c_DataSetData);
      }

      // Font for dataset
      rc_Data.c_DataSetFont = mc_STYLE_GUIDE_FONT_REGULAR_12;
      if (q_HasLink == true)
      {
         rc_Data.c_DataSetFont.setUnderline(true);
      }

      // Access
      rc_Data.c_Access = C_SdNdeDpUtil::h_ConvertElementAccessToString(pc_OscElement->e_Access);
      C_SdNdeDpUtil::h_ConvertToElementGeneric(*pc_OscElement, *pc_UiElement,
                                               C_SdNdeDpUtil::E_ElementDataChangeType::eELEMENT_ACCESS,
                                               rc_Data.c_AccessEdit, 0, 0);

      // Data size
      u32_SizeByte = pc_OscElement->GetSizeByte();
      if (u32_SizeByte == 1)
      {
         rc_Data.c_DataSize = static_cast<QString>("%1 Byte").arg(u32_SizeByte);
      }
      else
      {
         rc_Data.c_DataSize = static_cast<QString>("%1 Bytes").arg(u32_SizeByte);
      }

      // Address
      rc_Data.c_Address = static_cast<QString>("%1 (%2)").arg(
         pc_OscElement->u32_NvmStartAddress - pc_Datapool->u32_NvmStartAddress).arg(
         pc_OscElement->u32_NvmStartAddress);

      // Auto min max
      rc_Data.q_AutoMinMaxActive = pc_UiElement->q_AutoMinMaxActive;
      if (pc_UiElement->q_AutoMinMaxActive == true)
      {
         rc_Data.s32_AutoMinMaxCheckState = static_cast<int32_t>(Qt::Checked);
      }
      else
      {
         rc_Data.s32_AutoMinMaxCheckState = static_cast<int32_t>(Qt::Unchecked);
      }

      // Event call
      if (pc_OscElement->q_DiagEventCall == true)
      {
         rc_Data.s32_EventCallCheckState = static_cast<int32_t>(Qt::Checked);
      }
      else
      {
         rc_Data.s32_EventCallCheckState = static_cast<int32_t>(Qt::Unchecked);
      }

      if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(this->mu32_NodeIndex,
                                                           this->mu32_DataPoolIndex,
                                                           e_DataPoolType) == C_NO_ERR)
      {
         // Invalid

         if ((((q_NameConflict == true) || (q_NameInvalid == true)) || (q_MinOverMax == true)) ||
             (q_DataSetInvalid == true))
         {
            QString c_Content;

            // Heading
            rc_Data.c_InvalidToolTipHeading = static_cast<QString>(C_GtGetText::h_GetText("%1 has invalid content")).
                                              arg(C_PuiSdHandlerNodeLogic::h_GetElementTypeName(e_DataPoolType));

            // Content
            const QString c_Name = C_PuiSdHandlerNodeLogic::h_GetElementTypeName(e_DataPoolType);

            if (((q_NameConflict == true) || (q_NameInvalid == true)) || (q_MinOverMax == true))
            {
               c_Content += C_GtGetText::h_GetText("Invalid properties:\n");
               if (q_NameConflict == true)
               {
                  c_Content +=
                     static_cast<QString>(C_GtGetText::h_GetText("Duplicate %1 name detected.\n")).arg(
                        c_Name);
               }
               if (q_NameInvalid == true)
               {
                  c_Content +=
                     static_cast<QString>(C_GtGetText::h_GetText(
                                             "%1 name is empty or contains invalid characters.\n"))
                     .arg(c_Name);
               }
               if (q_MinOverMax == true)
               {
                  c_Content += C_GtGetText::h_GetText("Minimum value over maximum value\n");
               }
               c_Content += "\n";
            }
            if (q_DataSetInvalid == true)
            {
               c_Content += C_GtGetText::h_GetText("Data sets:\n");
               for (uint32_t u32_ItAppl = 0;
                    (u32_ItAppl < c_InvalidDataSetIndices.size()) &&
                    (u32_ItAppl < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                    ++u32_ItAppl)
               {
                  const C_OscNodeDataPoolDataSet * const pc_DataSet =
                     C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListDataSet(this->mu32_NodeIndex,
                                                                                this->mu32_DataPoolIndex,
                                                                                this->mu32_ListIndex,
                                                                                c_InvalidDataSetIndices[
                                                                                   u32_ItAppl]);
                  if (pc_DataSet != NULL)
                  {
                     c_Content += static_cast<QString>("%1\n").arg(pc_DataSet->c_Name.c_str());
                  }
               }
               if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidDataSetIndices.size())
               {
                  c_Content += static_cast<QString>("+%1\n").arg(
                     static_cast<uint32_t>(c_InvalidDataSetIndices.size()) -
                     mu32_TOOL_TIP_MAXIMUM_ITEMS);
               }
               c_Content += "\n";
            }
            rc_Data.c_InvalidToolTipContent = c_Content;

            // Icon
            rc_Data.c_InvalidIconRole.push_back(QString::number(20)); // icon size
            //Show error
            rc_Data.c_InvalidIconRole.push_back("://images/Error_iconV2.svg");
         }
         else
         {
            // No tool tip
            rc_Data.c_InvalidToolTipContent.clear();
            rc_Data.c_InvalidToolTipHeading.clear();

            // No icon
         }

         // Icon
         rc_Data.c_IconIconRole.clear();
         rc_Data.c_IconIconRole.push_back(QString::number(16)); // icon size
         if (e_DataPoolType == C_OscNodeDataPool::E_Type::eNVM)
         {
            rc_Data.c_IconIconRole.push_back(":/images/system_definition/IconParameter.svg");
         }
         else // DIAG Datapool
         {
            rc_Data.c_IconIconRole.push_back(":/images/system_definition/IconVariable.svg");
            //no extra handling for COMM & HAL Datapools because they do not use this list visualization
         }
      }

      // String flag
      rc_Data.q_InterpretAsString = pc_UiElement->q_InterpretAsString;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Node data pool index
   \param[in]  oru32_DataPoolListIndex          Node data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Node data pool list element index
   \param[in]  orc_NewData                      New data
   \param[in]  ore_DataChangeType               Data change type
   \param[in]  oru32_ArrayIndex                 Optional array index
   \param[in]  ors32_DataSetIndex               Optional data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListTableModel::m_DataChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                           const uint32_t & oru32_DataPoolListIndex,
                                           const uint32_t & oru32_DataPoolListElementIndex,
                                           const QVariant & orc_NewData,
                                           const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType,
                                           const uint32_t & oru32_ArrayIndex, const int32_t & ors32_DataSetIndex)
{
   Q_EMIT this->SigDataChange(oru32_NodeIndex, oru32_DataPoolIndex,
                              oru32_DataPoolListIndex,
                              oru32_DataPoolListElementIndex, orc_NewData,
                              ore_DataChangeType,
                              oru32_ArrayIndex, ors32_DataSetIndex);

   // Update the local data
   this->m_FillDpListElementInfo(oru32_DataPoolListElementIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if item needs link

   \param[in]  opc_UiElement    UI Element to check for link
   \param[in]  opc_OscElement   OSC Element to check for link

   \return
   True  Link
   False Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListTableModel::m_CheckLink(const C_PuiSdNodeDataPoolListElement * const opc_UiElement,
                                          const C_OscNodeDataPoolListElement * const opc_OscElement) const
{
   bool q_Retval = false;

   if ((opc_UiElement != NULL) && (opc_OscElement != NULL))
   {
      if ((opc_UiElement->q_InterpretAsString == false) && (opc_OscElement->GetArray() == true))
      {
         //Special link handling
         q_Retval = true;
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
void C_SdNdeDpListTableModel::m_MoveItems(const std::vector<uint32_t> & orc_ContiguousIndices,
                                          const uint32_t ou32_TargetIndex) const
{
   if (orc_ContiguousIndices.size() > 0UL)
   {
      std::vector<uint32_t> c_ContiguousIndicesCopy = orc_ContiguousIndices;
      bool q_Forward;
      if (c_ContiguousIndicesCopy[0UL] < ou32_TargetIndex)
      {
         q_Forward = true;
      }
      else
      {
         q_Forward = false;
      }
      for (uint32_t u32_ItItem = 0UL; u32_ItItem < c_ContiguousIndicesCopy.size(); ++u32_ItItem)
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
void C_SdNdeDpListTableModel::m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex) const
{
   C_PuiSdHandler::h_GetInstance()->MoveDataPoolListElement(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                            this->mu32_ListIndex, ou32_SourceIndex, ou32_TargetIndex);
}
