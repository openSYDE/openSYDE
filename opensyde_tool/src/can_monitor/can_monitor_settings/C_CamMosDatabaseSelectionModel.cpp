//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for database message selection (implementation)

   Model for database message selection

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>

#include "stwtypes.h"
#include "C_GtGetText.h"
#include "C_CamDbHandler.h"
#include "C_CamGenSigUtil.h"
#include "C_SdTooltipUtil.h"
#include "C_CamMosDatabaseSelectionModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_CamMosDatabaseSelectionModel::mhc_IconDatabase = "://images/IconDatabaseBlue.svg";
const QString C_CamMosDatabaseSelectionModel::mhc_IconMessage = "://images/IconMessage.svg";

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
C_CamMosDatabaseSelectionModel::C_CamMosDatabaseSelectionModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosDatabaseSelectionModel::~C_CamMosDatabaseSelectionModel(void)
{
   delete (this->mpc_InvisibleRootItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize table content from data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionModel::Init(void)
{
   this->beginResetModel();
   //Clear
   delete (this->mpc_InvisibleRootItem);
   this->mpc_InvisibleRootItem = new C_TblTreItem();
   //Always parse for all messages (prerequisite)
   C_CamDbHandler::h_GetInstance()->FindAllMessages();
   m_Init();
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected data elements

   array:
   0: database name
   1: message name

   \param[in]  orc_Index   Model index

   \return
   Current selected data elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::array<QString,
                       2> > C_CamMosDatabaseSelectionModel::GetDataElements(const QModelIndex & orc_Index) const
{
   std::vector<std::array<QString, 2> > c_Retval;
   if (orc_Index.isValid() == false)
   {
      //Invisible root
      for (sint32 s32_ItChild = 0L; s32_ItChild < this->rowCount(orc_Index); ++s32_ItChild)
      {
         mh_CombineIndices(c_Retval, this->GetDataElements(orc_Index.child(s32_ItChild, 0)));
      }
   }
   else
   {
      if (orc_Index.parent().isValid() == false)
      {
         //Database
         for (sint32 s32_ItChild = 0L; s32_ItChild < this->rowCount(orc_Index); ++s32_ItChild)
         {
            mh_CombineIndices(c_Retval, this->GetDataElements(orc_Index.child(s32_ItChild, 0)));
         }
      }
      else
      {
         std::array<QString, 2> c_Item;
         //Don't use data
         //-> file is shortened, message name is changed
         //-> that's what the maps are for
         const QString c_DisplayedDatabase = this->data(orc_Index.parent()).toString();
         const QString c_DisplayedMessage = this->data(orc_Index).toString();
         const QMap<QString, QString>::const_iterator c_ItDb = this->mc_DatabaseMap.find(c_DisplayedDatabase);
         const QMap<QString, QString>::const_iterator c_ItMsg = this->mc_MessageMap.find(c_DisplayedMessage);
         if ((c_ItDb != this->mc_DatabaseMap.end()) && (c_ItMsg != this->mc_MessageMap.end()))
         {
            //Message
            c_Item[0UL] = c_ItDb.value();
            c_Item[1UL] = c_ItMsg.value();
            c_Retval.push_back(c_Item);
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get tree column count

   \param[in]  orc_Parent  Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_CamMosDatabaseSelectionModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get item index by item name

   \param[in]  orc_ItemText   Item display text

   \return
   Model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_CamMosDatabaseSelectionModel::GetIndexForItem(const QString & orc_ItemText) const
{
   QModelIndex c_Retval;

   for (sintn sn_ItChild = 0; sn_ItChild < this->rowCount(); ++sn_ItChild)
   {
      const QModelIndex c_Tmp = this->index(sn_ItChild, 0);
      if (this->data(c_Tmp).toString().compare(orc_ItemText) == 0)
      {
         c_Retval = c_Tmp;
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert model index to generic item representation

   \param[in]  orc_ItemIndex  Model index

   \return
   Generic item representation
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_CamMosDatabaseSelectionModel::GetGenericRepresentationForIndex(const QModelIndex & orc_ItemIndex)
const
{
   std::vector<uint32> c_Retval;
   QModelIndex c_CurItem = orc_ItemIndex;
   while (c_CurItem.isValid() == true)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_TblTreItem * const pc_TreeItem = static_cast<const C_TblTreItem * const>(c_CurItem.internalPointer());
      if (pc_TreeItem != NULL)
      {
         c_Retval.insert(c_Retval.begin(), pc_TreeItem->u32_Index);
         c_CurItem = c_CurItem.parent();
      }
      else
      {
         break;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize tree structure for data pool lists

   Requires: call of C_CamDbHandler::h_GetInstance()->FindAllMessages();
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionModel::m_Init(void)
{
   const QMap<QString, C_CamDbDbc> & rc_DBCFiles = C_CamDbHandler::h_GetInstance()->GetDBCFiles();
   const QMap<QString, C_CamDbOsy> & rc_OSYFiles = C_CamDbHandler::h_GetInstance()->GetOSYFiles();

   //Reserve
   this->mpc_InvisibleRootItem->ReserveChildrenSpace(static_cast<uint32>(rc_DBCFiles.size()) +
                                                     static_cast<uint32>(rc_OSYFiles.size()));

   //DBC
   for (QMap<QString, C_CamDbDbc>::const_iterator c_ItDbc = rc_DBCFiles.begin(); c_ItDbc != rc_DBCFiles.end();
        ++c_ItDbc)
   {
      if (c_ItDbc->GetActive())
      {
         const std::vector<QString> c_Messages = c_ItDbc->GetFoundMessages();
         //Database
         C_TblTreItem * const pc_DatabaseItem = this->m_CreateAndFillDatabaseNode(
            c_ItDbc.key(), this->mpc_InvisibleRootItem);
         if (c_Messages.size() == 0UL)
         {
            //Special handling for no messages
            pc_DatabaseItem->c_Name = static_cast<QString>(C_GtGetText::h_GetText("%1 (No mapped messages)")).arg(
               pc_DatabaseItem->c_Name);
         }
         else
         {
            //Reserve
            pc_DatabaseItem->ReserveChildrenSpace(c_Messages.size());
            //Each message
            for (std::vector<QString>::const_iterator c_ItMessage = c_Messages.begin();
                 c_ItMessage != c_Messages.end(); ++c_ItMessage)
            {
               const C_CieConverter::C_CIECanMessage * const pc_Message =
                  C_CamDbHandler::h_GetInstance()->GetDbcMessage(c_ItDbc.key(), *c_ItMessage, false, 0UL);
               if (pc_Message != NULL)
               {
                  const C_OSCCanMessage c_OSCMessage = C_CamGenSigUtil::h_ConvertDBCToOSY(*pc_Message);

                  m_CreateAndFillMessageNode(c_OSCMessage, pc_DatabaseItem);
               }
            }
         }
      }
   }

   //OSY
   for (QMap<QString, C_CamDbOsy>::const_iterator c_ItOsy = rc_OSYFiles.begin(); c_ItOsy != rc_OSYFiles.end();
        ++c_ItOsy)
   {
      if (c_ItOsy->GetActive())
      {
         const QMap<QString, C_CamDbOsyMessageId> & rc_Messages = c_ItOsy->GetFoundMessages();
         //Database
         C_TblTreItem * const pc_DatabaseItem = this->m_CreateAndFillDatabaseNode(
            c_ItOsy.key(), this->mpc_InvisibleRootItem);
         if (static_cast<uint32>(rc_Messages.size()) == 0UL)
         {
            //Special handling for no messages
            pc_DatabaseItem->c_Name = static_cast<QString>(C_GtGetText::h_GetText("%1 (No mapped messages)")).arg(
               pc_DatabaseItem->c_Name);
         }
         else
         {
            //Reserve
            pc_DatabaseItem->ReserveChildrenSpace(rc_Messages.size());
            //Each message
            for (QMap<QString, C_CamDbOsyMessageId>::const_iterator c_ItMessage = rc_Messages.begin();
                 c_ItMessage != rc_Messages.end(); ++c_ItMessage)
            {
               const C_OSCCanMessage * const pc_Message = C_CamDbHandler::h_GetInstance()->GetOSCMessage(
                  c_ItOsy.key(), c_ItMessage.key(), true, c_ItMessage->u32_Hash);
               if (pc_Message != NULL)
               {
                  m_CreateAndFillMessageNode(*pc_Message, pc_DatabaseItem);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create and fill tree node for database item (without messages)

   \param[in]      orc_File         File to create node for
   \param[in,out]  opc_ParentItem   Parent item to insert this node

   \return
   Database tree node
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreItem * C_CamMosDatabaseSelectionModel::m_CreateAndFillDatabaseNode(const QString & orc_File,
                                                                           C_TblTreSimpleItem * const opc_ParentItem)
{
   C_TblTreItem * const pc_Retval = new C_TblTreItem();
   const QFileInfo c_FileInfo(orc_File);

   //Static
   pc_Retval->c_Icon = QIcon(C_CamMosDatabaseSelectionModel::mhc_IconDatabase);
   pc_Retval->c_Name = c_FileInfo.completeBaseName();
   pc_Retval->c_ToolTipHeading = C_GtGetText::h_GetText("Database");
   pc_Retval->c_ToolTipContent = orc_File;
   //Remember the original name (required by getter)
   this->mc_DatabaseMap.insert(pc_Retval->c_Name, orc_File);
   pc_Retval->q_Selectable = false;
   opc_ParentItem->AddChild(pc_Retval);
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create and fill tree node for message item (without signals)

   \param[in]      orc_Message      Message to create node for
   \param[in,out]  opc_ParentItem   Parent item to insert this node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionModel::m_CreateAndFillMessageNode(const C_OSCCanMessage & orc_Message,
                                                                C_TblTreSimpleItem * const opc_ParentItem)
{
   C_TblTreItem * const pc_MessageItem = new C_TblTreItem();

   //Init current node
   pc_MessageItem->u32_Index = 0UL;
   pc_MessageItem->c_Name = static_cast<QString>("%1 (0x%2)").arg(orc_Message.c_Name.c_str()).
                            arg(static_cast<QString>("%1").arg(orc_Message.u32_CanId, 0, 16).toUpper());

   //Remember the original name (required by getter)
   this->mc_MessageMap.insert(pc_MessageItem->c_Name, orc_Message.c_Name.c_str());
   pc_MessageItem->c_ToolTipHeading = orc_Message.c_Name.c_str();
   pc_MessageItem->c_ToolTipContent = C_SdTooltipUtil::h_GetToolTipContentMessage(orc_Message);
   pc_MessageItem->c_Icon = QIcon(C_CamMosDatabaseSelectionModel::mhc_IconMessage);
   opc_ParentItem->AddChild(pc_MessageItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append all items from the vector orc_SmallVectorToAdd to the vector orc_BigVectorToAppendTo

   \param[in,out]  orc_BigVectorToAppendTo   All current items which will get expanded
   \param[in]      orc_SmallVectorToAdd      New items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosDatabaseSelectionModel::mh_CombineIndices(std::vector<std::array<QString,
                                                                              2> > & orc_BigVectorToAppendTo,
                                                       const std::vector<std::array<QString,
                                                                                    2> > & orc_SmallVectorToAdd)
{
   //Reserve
   orc_BigVectorToAppendTo.reserve(orc_BigVectorToAppendTo.size() + orc_SmallVectorToAdd.size());
   //Append all
   for (uint32 u32_ItItem = 0UL; u32_ItItem < orc_SmallVectorToAdd.size(); ++u32_ItItem)
   {
      orc_BigVectorToAppendTo.push_back(orc_SmallVectorToAdd[u32_ItItem]);
   }
}
