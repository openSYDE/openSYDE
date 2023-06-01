//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for adding messages from J1939 Catalog

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QIcon>

#include "precomp_headers.hpp"
#include "stwtypes.hpp"
#include "constants.hpp"

#include "C_SdBueJ1939AddMessagesFromCatalogTableModel.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdUtil.hpp"
#include "C_OscCanUtil.hpp"
#include "C_CieConverter.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
 *   \param[in]  opc_Parent      Pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueJ1939AddMessagesFromCatalogTableModel::C_SdBueJ1939AddMessagesFromCatalogTableModel(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent)

{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get header data

   \param[in]  os32_Section      Section
   \param[in]  oe_Orientation    Orientation
   \param[in]  os32_Role         Role

   \return
   Header string
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueJ1939AddMessagesFromCatalogTableModel::headerData(const int32_t os32_Section,
                                                                  const Qt::Orientation oe_Orientation,
                                                                  const int32_t os32_Role) const
{
   QVariant c_Retval = QAbstractTableModel::headerData(os32_Section, oe_Orientation, os32_Role);

   if (oe_Orientation == Qt::Orientation::Horizontal)
   {
      const C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns e_Col = h_ColumnToEnum(os32_Section);
      if (os32_Role == static_cast<int32_t>(Qt::DisplayRole))
      {
         switch (e_Col)
         {
         case eICON:
            c_Retval = "";
            break;
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eJ1939_PGN:
            c_Retval = C_GtGetText::h_GetText("PGN");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN ID");
            break;
         default:
            break;
         }
      }
      else if (os32_Role == static_cast<int32_t>(Qt::TextAlignmentRole))
      {
         c_Retval = static_cast<QVariant>(Qt::AlignLeft | Qt::AlignVCenter);
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_CONTENT)
      {
         switch (e_Col)
         {
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Symbolic message name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment for this message");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN Identifier(Hex). 29 Bit");
            break;
         case eJ1939_PGN:
            c_Retval = C_GtGetText::h_GetText("Parameter Group Number. 18 Bit");
            break;
         case eICON:
         default:

            break;
         }
      }
      else if (os32_Role == ms32_USER_ROLE_TOOL_TIP_HEADING)
      {
         switch (e_Col)
         {
         case eNAME:
            c_Retval = C_GtGetText::h_GetText("Name");
            break;
         case eCOMMENT:
            c_Retval = C_GtGetText::h_GetText("Comment");
            break;
         case eCAN_ID:
            c_Retval = C_GtGetText::h_GetText("CAN Id");
            break;
         case eJ1939_PGN:
            c_Retval = C_GtGetText::h_GetText("PGN");
            break;
         case eICON:
         default:
            break;
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
/*! \brief   Get table row count

   \param[in]  orc_Parent  Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueJ1939AddMessagesFromCatalogTableModel::rowCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      s32_Retval = this->mc_MessageInfoList.size();
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
int32_t C_SdBueJ1939AddMessagesFromCatalogTableModel::columnCount(const QModelIndex & orc_Parent) const
{
   int32_t s32_Retval = 0;

   if (!orc_Parent.isValid())
   {
      //For table parent should always be invalid
      s32_Retval = 5;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  os32_Role   Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdBueJ1939AddMessagesFromCatalogTableModel::data(const QModelIndex & orc_Index,
                                                            const int32_t os32_Role) const
{
   QVariant c_Retval;

   if (orc_Index.isValid() == true)
   {
      const C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns e_Col = h_ColumnToEnum(orc_Index.column());
      if ((os32_Role == static_cast<int32_t>(Qt::DisplayRole)) || (os32_Role == static_cast<int32_t>(Qt::EditRole)))
      {
         const int32_t s32_Index = orc_Index.row();
         if ((static_cast<uint32_t>(s32_Index) < this->mc_MessageInfoList.size()) && (s32_Index >= 0))
         {
            switch (e_Col)
            {
            case eCAN_ID:
               c_Retval = this->mc_MessageInfoList[s32_Index].c_CanId;
               break;
            case eJ1939_PGN:
               c_Retval = this->mc_MessageInfoList[s32_Index].c_J1939Pgn;
               break;
            case eNAME:
               c_Retval = this->mc_MessageInfoList[s32_Index].c_Name;
               break;
            case eCOMMENT:
               c_Retval = this->mc_MessageInfoList[s32_Index].c_Comment;
               break;
            case eICON:
            default:
               break;
            }
         }
      }

      else if (os32_Role == static_cast<int32_t>(Qt::DecorationRole))
      {
         switch (e_Col)
         {
         case eICON:
            c_Retval = QIcon(":/images/system_definition/IconMessage.svg");
            break;
         case eNAME:
         case eCOMMENT:
         case eCAN_ID:
         case eJ1939_PGN:
         default:
            //No decoration
            break;
         }
      }

      else
      {
         //Unknown
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Column to enum conversion

   \param[in]  ors32_Column   Column

   \return
   Enum value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns C_SdBueJ1939AddMessagesFromCatalogTableModel::h_ColumnToEnum(
   const int32_t & ors32_Column)
{
   C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns e_Retval = eNAME;
   switch (ors32_Column)
   {
   case 0:
      e_Retval = eICON;
      break;
   case 1:
      e_Retval = eCAN_ID;
      break;
   case 2:
      e_Retval = eJ1939_PGN;
      break;
   case 3:
      e_Retval = eNAME;
      break;
   case 4:
      e_Retval = eCOMMENT;
      break;
   default:
      //Unknown
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
int32_t C_SdBueJ1939AddMessagesFromCatalogTableModel::h_EnumToColumn(
   const C_SdBueJ1939AddMessagesFromCatalogTableModel::E_Columns & ore_Value)
{
   int32_t s32_Retval;

   switch (ore_Value)
   {
   case eICON:
      s32_Retval = 0;
      break;
   case eCAN_ID:
      s32_Retval = 1;
      break;
   case eJ1939_PGN:
      s32_Retval = 2;
      break;
   case eNAME:
      s32_Retval = 3;
      break;
   case eCOMMENT:
      s32_Retval = 4;
      break;
   default:
      s32_Retval = -1;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if J1939 message

   \return
   true     J1939 message
   false    otherwise
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueJ1939AddMessagesFromCatalogTableModel::m_IsJ1939SpecificMessage(
   const C_CieConverter::C_CieNodeMessage & orc_Message) const
{
   bool q_IsJ1939Specific = false;

   if ((orc_Message.c_CanMessage.u16_Dlc == 8) && (orc_Message.c_CanMessage.q_IsExtended == true))
   {
      q_IsJ1939Specific = true;
   }

   return q_IsJ1939Specific;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Fill message info

 *
 *  Fills the message data in the list, to display in the overview table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTableModel::m_FillMessageInfo()
{
   this->mc_MessageInfoList.clear();

   this->mc_MessageInfoList.reserve(this->mc_MessagesImportedFromCatalog.size());
   for (uint32_t u32_MessageIndex = 0; u32_MessageIndex < this->mc_MessagesImportedFromCatalog.size();
        u32_MessageIndex++)
   {
      C_CieConverter::C_CieNodeMessage & rc_Message =
         this->mc_MessagesImportedFromCatalog[u32_MessageIndex];

      C_MsgTableData c_MessageData;
      c_MessageData.c_Name = static_cast<QString>(rc_Message.c_CanMessage.c_Name.c_str());
      c_MessageData.c_Comment = static_cast<QString>(rc_Message.c_CanMessage.c_Comment.c_str());
      c_MessageData.c_CanId =  "0x" + QString::number(rc_Message.c_CanMessage.u32_CanId, 16).toUpper();

      //PGN
      C_OscCanUtilJ1939PgInfo c_PgInfo;
      C_OscCanUtil::h_GetJ1939PgInfoFromCanId(rc_Message.c_CanMessage.u32_CanId, c_PgInfo);
      c_MessageData.c_J1939Pgn = C_OscCanUtil::h_GetVisiblePgn(c_PgInfo.u32_Pgn);

      this->mc_MessageInfoList.push_back(c_MessageData);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal data changes to table
 *
 *  \param[in]     orc_MessagesImported          Messages extracted from the DBC file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueJ1939AddMessagesFromCatalogTableModel::UpdateData(
   const std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_MessagesImported)

{
   this->mc_MessagesImportedFromCatalog = orc_MessagesImported;

   beginResetModel();
   this->m_FillMessageInfo();
   endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data based on given index

   \param[in]       ou32_MessageIndex     Message index

   \return
   Data for index
*/
//----------------------------------------------------------------------------------------------------------------------
const C_CieConverter::C_CieNodeMessage * C_SdBueJ1939AddMessagesFromCatalogTableModel::GetDataForIndex(
   const uint32_t ou32_MessageIndex) const
{
   const C_CieConverter::C_CieNodeMessage * const pc_RetVal =
      &(this->mc_MessagesImportedFromCatalog[ou32_MessageIndex]);

   return pc_RetVal;
}
