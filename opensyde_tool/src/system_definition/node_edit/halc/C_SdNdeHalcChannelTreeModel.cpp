//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channels overview.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"

#include "C_SdNdeHalcChannelTreeModel.hpp"
#include "C_TblTreItem.hpp"

#include "constants.hpp"
#include "stwerrors.hpp"

#include "TglUtils.hpp"
#include "C_SdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeHalcChannelCopyPaste.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

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
C_SdNdeHalcChannelTreeModel::C_SdNdeHalcChannelTreeModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent),
   mu32_NodeIndex(0)
{
   // initialize icons
   if (this->mc_Icons.empty() == true)
   {
      // #icons = 3 (#categories) x 2 (domain vs channel) x 2 (linked vs. not linked) x 2 (valid vs. error) = 24
      this->mc_Icons.resize(24);

      this->m_InitIconsOfCategory(C_OscHalcConfigDomain::eCA_INPUT);
      this->m_InitIconsOfCategory(C_OscHalcConfigDomain::eCA_OUTPUT);
      this->m_InitIconsOfCategory(C_OscHalcConfigDomain::eCA_OTHER);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor.

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcChannelTreeModel::~C_SdNdeHalcChannelTreeModel()
{
   delete (this->mpc_InvisibleRootItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return number of columns.

   Here: Only one column.

   \param[in]  orc_Parent  Parent

   \return
   Number of columns, namely 1
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeHalcChannelTreeModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node index.

   Create all tree children.

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::SetNode(const uint32_t ou32_NodeIndex)
{
   const C_OscHalcConfig * const pc_HalcConfig = C_PuiSdHandler::h_GetInstance()->GetHalcConfig(ou32_NodeIndex);

   this->mu32_NodeIndex = ou32_NodeIndex;

   if (pc_HalcConfig != NULL)
   {
      // clear
      this->Clear();

      // insert model data
      this->beginResetModel();
      if (this->mpc_InvisibleRootItem == NULL)
      {
         this->mpc_InvisibleRootItem = new C_TblTreItem();
      }

      for (uint32_t u32_DomainIt = 0; u32_DomainIt < pc_HalcConfig->GetDomainSize(); u32_DomainIt++)
      {
         // create tree items for domains and its channels
         const C_OscHalcConfigDomain * const pc_Domain = pc_HalcConfig->GetDomainConfigDataConst(u32_DomainIt);
         if (pc_Domain != NULL)
         {
            C_TblTreItem * const pc_DomainTreeItem = new C_TblTreItem();

            if (pc_Domain->c_Channels.empty() == false)
            {
               pc_DomainTreeItem->c_Name = static_cast<QString>("%1 (%2)").arg(pc_Domain->c_Name.c_str())
                                           .arg(pc_Domain->c_Channels.size());
            }
            else
            {
               pc_DomainTreeItem->c_Name = pc_Domain->c_Name.c_str();
            }
            pc_DomainTreeItem->c_ToolTipHeading = pc_Domain->c_Name.c_str();
            pc_DomainTreeItem->c_ToolTipContent = pc_Domain->c_DomainConfig.c_Comment.c_str();

            // icon is done by CheckError()

            tgl_assert(pc_Domain->c_Channels.size() == pc_Domain->c_ChannelConfigs.size());
            if (pc_Domain->c_Channels.size() == pc_Domain->c_ChannelConfigs.size())
            {
               // set selectable only if domain without channels
               if (pc_Domain->c_Channels.empty() == false)
               {
                  pc_DomainTreeItem->q_Selectable = false;
                  pc_DomainTreeItem->q_Enabled = false;
                  // disabled because else the currentRowChanged signal gets emitted on click
                  // if enabling h_GetIndexesFromModelIndex will not work properly anymore
               }

               // add channels
               for (uint32_t u32_ChannelIt = 0; u32_ChannelIt < pc_Domain->c_Channels.size(); u32_ChannelIt++)
               {
                  C_TblTreItem * const pc_Child = new C_TblTreItem();
                  const C_OscHalcConfigChannel & rc_ConfigChannel = pc_Domain->c_ChannelConfigs[u32_ChannelIt];

                  mh_SetChannelText(pc_Child, rc_ConfigChannel.c_Name.c_str(),
                                    pc_Domain->c_Channels[u32_ChannelIt].c_Name.c_str(),
                                    rc_ConfigChannel.c_Comment.c_str());
                  pc_Child->u32_Index = u32_ChannelIt;

                  // icon is done by CheckError()

                  // add to tree
                  pc_DomainTreeItem->AddChild(pc_Child);
               }
            }

            if (this->mpc_InvisibleRootItem != NULL)
            {
               // add to tree
               this->mpc_InvisibleRootItem->AddChild(pc_DomainTreeItem);
            }
            else
            {
               pc_DomainTreeItem->ClearChildren();
               delete pc_DomainTreeItem;
            }
         }

         // error check and icons
         this->CheckError(u32_DomainIt);
      }

      this->endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update texts and icon of specified channel.

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::UpdateChannelText(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                                    const bool oq_UseChannelIndex)
{
   if ((this->mpc_InvisibleRootItem != NULL) && (ou32_DomainIndex < this->mpc_InvisibleRootItem->c_Children.size()))
   {
      C_TblTreItem * const pc_DomainItem =
         dynamic_cast<C_TblTreItem *>(this->mpc_InvisibleRootItem->c_Children[ou32_DomainIndex]);

      if (pc_DomainItem != NULL)
      {
         const C_OscHalcConfigDomain * const pc_Domain =
            C_PuiSdHandler::h_GetInstance()->GetHalcDomainConfigDataConst(this->mu32_NodeIndex, ou32_DomainIndex);
         const C_OscHalcConfigChannel * const pc_Channel =
            C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelConfigData(this->mu32_NodeIndex, ou32_DomainIndex,
                                                                            ou32_ChannelIndex, oq_UseChannelIndex);
         C_TblTreItem * const pc_ChannelItem =
            (oq_UseChannelIndex == true) ? pc_DomainItem->GetItem(ou32_ChannelIndex) : pc_DomainItem;

         if ((pc_Domain != NULL) && (pc_ChannelItem != NULL))
         {
            // name and comment
            if (ou32_ChannelIndex < pc_Domain->c_Channels.size())
            {
               // channel case
               mh_SetChannelText(pc_ChannelItem, pc_Channel->c_Name.c_str(),
                                 pc_Domain->c_Channels[ou32_ChannelIndex].c_Name.c_str(),
                                 pc_Channel->c_Comment.c_str());
            }
            else
            {
               // domain case
               pc_ChannelItem->c_Name = pc_Channel->c_Name.c_str();
               pc_ChannelItem->c_ToolTipHeading = pc_Channel->c_Name.c_str();
               pc_ChannelItem->c_ToolTipContent = pc_Channel->c_Comment.c_str();
            }

            // update domain and channel visualization
            Q_EMIT (dataChanged(GetModelIndexFromIndexes(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex),
                                GetModelIndexFromIndexes(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex),
                                QVector<int32_t>() << static_cast<int32_t>(Qt::DisplayRole) <<
                                ms32_USER_ROLE_TOOL_TIP_HEADING << ms32_USER_ROLE_TOOL_TIP_CONTENT));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear all children.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::Clear(void)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      std::vector<C_TblTreSimpleItem *>::const_iterator c_ItChildren;

      this->beginResetModel();

      // clear channels (children of children)
      for (c_ItChildren = this->mpc_InvisibleRootItem->c_Children.begin();
           c_ItChildren != this->mpc_InvisibleRootItem->c_Children.end(); ++c_ItChildren)
      {
         if (*c_ItChildren != NULL)
         {
            (*c_ItChildren)->ClearChildren();
         }
      }

      // clear domains (children)
      this->mpc_InvisibleRootItem->ClearChildren();
      this->endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy selected

   \param[in]  orc_CurrentChannel   Model index of channel that gets copied
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::Copy(const QModelIndex & orc_CurrentChannel) const
{
   C_SdNdeHalcChannelCopyPaste::h_Copy(this->mu32_NodeIndex, orc_CurrentChannel);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Paste data to channels of selected indices.

   \param[in,out]  opc_Parent          Parent widget for message box
   \param[in]      orc_TargetIndexes   Indexes of channels to paste at
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::Paste(QWidget * const opc_Parent, const QModelIndexList & orc_TargetIndexes)
{
   if (C_SdNdeHalcChannelCopyPaste::h_Paste(opc_Parent, this->mu32_NodeIndex, orc_TargetIndexes) == true)
   {
      if (orc_TargetIndexes.empty() == false)
      {
         uint32_t u32_DomainIndex;
         uint32_t u32_ChannelIndex;
         bool q_ChannelCase;
         C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(*(orc_TargetIndexes.begin()), u32_DomainIndex,
                                                                 u32_ChannelIndex, q_ChannelCase);
         Q_UNUSED(q_ChannelCase)
         Q_UNUSED(u32_ChannelIndex)
         // update error icons
         this->CheckError(u32_DomainIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset configuration of specified indices

   \param[in]  orc_Indexes    Indexes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::Reset(const QModelIndexList & orc_Indexes)
{
   uint32_t u32_DomainIndex;
   uint32_t u32_ChannelIndex;
   bool q_ChannelCase;
   bool q_IsLinked;

   for (QModelIndexList::const_iterator c_ItIndex = orc_Indexes.begin(); c_ItIndex != orc_Indexes.end(); ++c_ItIndex)
   {
      std::vector<uint32_t> c_LinkedChannelIndices;
      C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(*c_ItIndex, u32_DomainIndex, u32_ChannelIndex,
                                                              q_ChannelCase);

      // first: reset use case of all linked channels
      C_PuiSdHandler::h_GetInstance()->CheckHalcDomainChannelLinked(this->mu32_NodeIndex, u32_DomainIndex,
                                                                    u32_ChannelIndex, q_ChannelCase, q_IsLinked, NULL,
                                                                    &c_LinkedChannelIndices);
      if (q_IsLinked == true)
      {
         for (std::vector<uint32_t>::const_iterator c_ItLinked = c_LinkedChannelIndices.begin();
              c_ItLinked != c_LinkedChannelIndices.end(); ++c_ItLinked)
         {
            C_PuiSdHandler::h_GetInstance()->ResetHalcDomainChannelUseCase(this->mu32_NodeIndex, u32_DomainIndex,
                                                                           *c_ItLinked, q_ChannelCase);
         }
      }

      // finally reset channel
      tgl_assert(C_PuiSdHandler::h_GetInstance()->
                 ResetHalcDomainChannelConfig(this->mu32_NodeIndex, u32_DomainIndex,
                                              u32_ChannelIndex, q_ChannelCase) == C_NO_ERR);

      this->UpdateChannelText(u32_DomainIndex, u32_ChannelIndex, q_ChannelCase);
   }

   if (orc_Indexes.empty() == false)
   {
      // update error icon
      this->CheckError(u32_DomainIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check error and adapt icon accordingly

   Always check complete domain, as the domain must show the error too.

   \param[in]  ou32_DomainIndex  Domain index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::CheckError(const uint32_t ou32_DomainIndex)
{
   const C_OscHalcConfigDomain * const pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHalcDomainConfigDataConst(this->mu32_NodeIndex, ou32_DomainIndex);

   if (pc_Domain != NULL)
   {
      if ((this->mpc_InvisibleRootItem != NULL) && (ou32_DomainIndex < this->mpc_InvisibleRootItem->c_Children.size()))
      {
         C_TblTreItem * const pc_DomainItem =
            dynamic_cast<C_TblTreItem *>(this->mpc_InvisibleRootItem->c_Children[ou32_DomainIndex]);

         if (pc_DomainItem != NULL)
         {
            bool q_AtLeastOneChannelInvalid = false;
            bool q_DomainNameInvalid;

            // check channels
            for (uint32_t u32_ItChannel = 0; u32_ItChannel < pc_Domain->c_ChannelConfigs.size(); u32_ItChannel++)
            {
               bool q_Invalid;
               bool q_Linked;
               C_PuiSdHandler::h_GetInstance()->CheckHalcDomainChannelError(this->mu32_NodeIndex, ou32_DomainIndex,
                                                                            u32_ItChannel, true, q_Invalid);
               C_PuiSdHandler::h_GetInstance()->CheckHalcDomainChannelLinked(this->mu32_NodeIndex, ou32_DomainIndex,
                                                                             u32_ItChannel, true, q_Linked);
               C_TblTreItem * const pc_ChannelItem = pc_DomainItem->GetItem(u32_ItChannel);

               // channel icon
               pc_ChannelItem->c_Icon = this->m_GetIcon(pc_Domain->e_Category, false, q_Invalid, q_Linked);
               if (q_Invalid == true)
               {
                  q_AtLeastOneChannelInvalid = true;
               }
            }

            // check domain
            pc_Domain->c_DomainConfig.CheckConfigValid(&q_DomainNameInvalid);
            // domain icon
            pc_DomainItem->c_Icon = this->m_GetIcon(pc_Domain->e_Category, true,
                                                    (q_DomainNameInvalid || q_AtLeastOneChannelInvalid), false);

            // update domain and channel visualization
            Q_EMIT (dataChanged(GetModelIndexFromIndexes(ou32_DomainIndex, 0, true),
                                GetModelIndexFromIndexes(ou32_DomainIndex, pc_Domain->c_ChannelConfigs.size(),
                                                         true),
                                QVector<int32_t>() << static_cast<int32_t>(Qt::DecorationRole)));
            Q_EMIT (dataChanged(GetModelIndexFromIndexes(ou32_DomainIndex, 0 /*irrelevant*/, false),
                                GetModelIndexFromIndexes(ou32_DomainIndex, 0 /*irrelevant*/, false),
                                QVector<int32_t>() << static_cast<int32_t>(Qt::DecorationRole)));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node index

   \return
   node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeHalcChannelTreeModel::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get domain and channel indexes from model index

   \param[in]   orc_ModelIndex      Model index
   \param[out]  oru32_DomainIndex   Corresponding domain index
   \param[out]  oru32_ChannelIndex  Corresponding domain index
   \param[out]  orq_ChannelCase     Channel case
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(const QModelIndex & orc_ModelIndex,
                                                             uint32_t & oru32_DomainIndex,
                                                             uint32_t & oru32_ChannelIndex, bool & orq_ChannelCase)
{
   oru32_DomainIndex = 0;
   oru32_ChannelIndex = 0;
   orq_ChannelCase = false;

   if (orc_ModelIndex.isValid() == true)
   {
      if (orc_ModelIndex.parent().isValid() == true)
      {
         // channel case
         oru32_DomainIndex = static_cast<uint32_t>(orc_ModelIndex.parent().row());
         oru32_ChannelIndex = static_cast<uint32_t>(orc_ModelIndex.row());
         orq_ChannelCase = true;
      }
      else
      {
         //domain case
         oru32_DomainIndex = static_cast<uint32_t>(orc_ModelIndex.row());
         // channel index not relevant in this case
         // attention: if parents are not disabled, further distinction of "has children" would be needed!
         orq_ChannelCase = false;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert domain and channel indexes to model index

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_ChannelCase       Channel case

   \return  Corresponding model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_SdNdeHalcChannelTreeModel::GetModelIndexFromIndexes(const uint32_t ou32_DomainIndex,
                                                                  const uint32_t ou32_ChannelIndex,
                                                                  const bool oq_ChannelCase) const
{
   QModelIndex c_ModelIndex = this->index(ou32_DomainIndex, 0); // flat tree so column is always 0

   if (oq_ChannelCase)
   {
      c_ModelIndex = this->index(ou32_ChannelIndex, 0, c_ModelIndex);
   }

   return c_ModelIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set item name and tooltip

   \param[in,out]  opc_Item         Item
   \param[in]      orc_ChannelName  Channel name
   \param[in]      orc_ChannelID    Channel ID
   \param[in]      orc_Comment      Comment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::mh_SetChannelText(C_TblTreItem * const opc_Item, const QString & orc_ChannelName,
                                                    const QString & orc_ChannelID, const QString & orc_Comment)
{
   if (opc_Item != NULL)
   {
      opc_Item->c_Name = static_cast<QString>("%1  %2").arg(orc_ChannelID, orc_ChannelName);
      opc_Item->c_ToolTipHeading = static_cast<QString>("%1 (%2)").arg(orc_ChannelName, orc_ChannelID);
      opc_Item->c_ToolTipContent = orc_Comment;
   }
} //lint !e429  //no memory leak because of correct tree clean up

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize icon from file path

   Don't call this too often. Calling for every channel was too time expensive.

   \param[in]  oe_Category    Category
   \param[in]  oq_Large       Large
   \param[in]  oq_Error       Error
   \param[in]  oq_Linked      Linked

   \return
   Icon
*/
//----------------------------------------------------------------------------------------------------------------------
QIcon C_SdNdeHalcChannelTreeModel::mh_InitIcon(const C_OscHalcDefDomain::E_Category oe_Category, const bool oq_Large,
                                               const bool oq_Error, const bool oq_Linked)
{
   QIcon c_Icon;

   QString c_IconPath = "://images/system_definition/NodeEdit/halc/";
   const QSize c_ICON_SIZE = QSize(16, 16);

   switch (oe_Category)
   {
   case C_OscHalcDefDomain::eCA_INPUT:
      c_IconPath += "Input";
      break;
   case C_OscHalcDefDomain::eCA_OUTPUT:
      c_IconPath += "Output";
      break;
   case C_OscHalcDefDomain::eCA_OTHER:
   default:
      c_IconPath += "Other";
      break;
   }

   c_IconPath += oq_Large ? "LargeCenter" : "Small";

   c_IconPath += oq_Linked ? "Linked" : "";

   if (oq_Error == true)
   {
      c_IconPath += "Error";

      // use same icon for all modes & states
      c_Icon = C_SdUtil::h_InitStaticIconSvg(c_IconPath + ".svg", c_ICON_SIZE);
   }
   else
   {
      c_Icon = C_SdUtil::h_InitStaticIconSvg(c_IconPath + "Inactive.svg", c_ICON_SIZE);
      // use different colored icon for active state
      c_Icon.addPixmap(QIcon(c_IconPath + "Active.svg").pixmap(c_ICON_SIZE), QIcon::Selected, QIcon::On);
      c_Icon.addPixmap(QIcon(c_IconPath + "Active.svg").pixmap(c_ICON_SIZE), QIcon::Selected, QIcon::Off);
      // "off" icon is used in branch-closed state
   }

   return c_Icon;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get icon depending on properties

   \param[in]  oe_Category    Category (input, output, other)
   \param[in]  oq_Large       Large
   \param[in]  oq_Error       Error
   \param[in]  oq_Linked      Linked

   \return
   Icon
*/
//----------------------------------------------------------------------------------------------------------------------
QIcon C_SdNdeHalcChannelTreeModel::m_GetIcon(const C_OscHalcDefDomain::E_Category oe_Category, const bool oq_Large,
                                             const bool oq_Error, const bool oq_Linked) const
{
   const uint32_t u32_Index = this->m_GetIconIdentifier(oe_Category, oq_Large, oq_Error, oq_Linked);

   tgl_assert(u32_Index < this->mc_Icons.size());
   const QIcon & rc_Icon = this->mc_Icons[u32_Index];

   return rc_Icon;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get icon index

   Resulting order in vector:
   0 Input small
   1 Input small error
   2 Input large
   3 Input large error
   4 Input small linked
   5 Input small linked error
   6 Output large
    ... etc. ...

   \param[in]  oe_Category    Category
   \param[in]  oq_Large       Large
   \param[in]  oq_Error       Error
   \param[in]  oq_Linked      Linked

   \return
   Icon identifier
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeHalcChannelTreeModel::m_GetIconIdentifier(const C_OscHalcDefDomain::E_Category oe_Category,
                                                          const bool oq_Large, const bool oq_Error,
                                                          const bool oq_Linked) const
{
   uint32_t u32_Return = 0;

   switch (oe_Category)
   {
   case C_OscHalcDefDomain::eCA_INPUT:
      u32_Return = 0;
      break;
   case C_OscHalcDefDomain::eCA_OUTPUT:
      u32_Return = 6;
      break;
   case C_OscHalcDefDomain::eCA_OTHER:
   default:
      u32_Return = 12;
      break;
   }

   if (oq_Large == true)
   {
      u32_Return += 2;
   }

   if (oq_Linked == true)
   {
      u32_Return += 4;
   }

   if (oq_Error == true)
   {
      u32_Return += 1;
   }

   tgl_assert(u32_Return < this->mc_Icons.size());
   if (u32_Return >= this->mc_Icons.size())
   {
      u32_Return = 0;
   }

   return u32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize icons of domain category

   \param[in]  oe_Category    Category
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::m_InitIconsOfCategory(const C_OscHalcDefDomain::E_Category oe_Category)
{
   uint32_t u32_Index;

   // domain icon
   u32_Index = this->m_GetIconIdentifier(oe_Category, true, false, false);
   mc_Icons[u32_Index] = mh_InitIcon(oe_Category, true, false, false);

   // domain icon error
   u32_Index = m_GetIconIdentifier(oe_Category, true, true, false);
   mc_Icons[u32_Index] = mh_InitIcon(oe_Category, true, true, false);

   // channel icon
   u32_Index = m_GetIconIdentifier(oe_Category, false, false, false);
   mc_Icons[u32_Index] = mh_InitIcon(oe_Category, false, false, false);

   // channel icon error
   u32_Index = m_GetIconIdentifier(oe_Category, false, true, false);
   mc_Icons[u32_Index] = mh_InitIcon(oe_Category, false, true, false);

   // linked channel icon
   u32_Index = m_GetIconIdentifier(oe_Category, false, false, true);
   mc_Icons[u32_Index] = mh_InitIcon(oe_Category, false, false, true);

   // linked channel icon error
   u32_Index = m_GetIconIdentifier(oe_Category, false, true, true);
   mc_Icons[u32_Index] = mh_InitIcon(oe_Category, false, true, true);
}
