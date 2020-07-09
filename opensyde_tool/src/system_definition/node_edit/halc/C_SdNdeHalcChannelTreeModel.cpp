//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channels overview.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"

#include "C_SdNdeHalcChannelTreeModel.h"
#include "C_TblTreItem.h"

#include "TGLUtils.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeHalcChannelCopyPaste.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

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
sintn C_SdNdeHalcChannelTreeModel::columnCount(const QModelIndex & orc_Parent) const
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
void C_SdNdeHalcChannelTreeModel::SetNode(const stw_types::uint32 ou32_NodeIndex)
{
   const C_OSCHalcConfig * pc_HalcConfig = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(ou32_NodeIndex);

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

      for (uint32 u32_DomainIt = 0; u32_DomainIt < pc_HalcConfig->GetDomainSize(); u32_DomainIt++)
      {
         // create tree items for domains and its channels
         const C_OSCHalcConfigDomain * pc_Domain = pc_HalcConfig->GetDomainConfigDataConst(u32_DomainIt);
         if (pc_Domain != NULL)
         {
            C_TblTreItem * const pc_DomainTreeItem = new C_TblTreItem();

            pc_DomainTreeItem->c_Name = QString("%1 (%2)").arg(pc_Domain->c_Name.c_str())
                                        .arg(pc_Domain->c_Channels.size());
            // TODO Tooltip?

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
               for (uint32 u32_ChannelIt = 0; u32_ChannelIt < pc_Domain->c_Channels.size(); u32_ChannelIt++)
               {
                  C_TblTreItem * const pc_Child = new C_TblTreItem();
                  const C_OSCHalcConfigChannel & rc_ConfigChannel = pc_Domain->c_ChannelConfigs[u32_ChannelIt];

                  pc_Child->c_Name = QString("%1 (%2)").arg(rc_ConfigChannel.c_Name.c_str(),
                                                            pc_Domain->c_Channels[u32_ChannelIt].c_Name.c_str());
                  pc_Child->c_ToolTipHeading = pc_Child->c_Name;
                  pc_Child->c_ToolTipContent = rc_ConfigChannel.c_Comment.c_str();
                  pc_Child->u32_Index = u32_ChannelIt;

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
               delete pc_DomainTreeItem;
            }
         }
      }

      this->endResetModel();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update texts of specified channel.

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::UpdateChannelText(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                    const bool oq_UseChannelIndex)
{
   if ((this->mpc_InvisibleRootItem != NULL) && (ou32_DomainIndex < this->mpc_InvisibleRootItem->c_Children.size()))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_TblTreItem * const pc_DomainItem =
         dynamic_cast<C_TblTreItem *>(this->mpc_InvisibleRootItem->c_Children[ou32_DomainIndex]);

      if (pc_DomainItem != NULL)
      {
         const C_OSCHalcConfigDomain * pc_Domain =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(this->mu32_NodeIndex, ou32_DomainIndex);
         const C_OSCHalcConfigChannel * pc_Channel =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(this->mu32_NodeIndex, ou32_DomainIndex,
                                                                            ou32_ChannelIndex, oq_UseChannelIndex);
         C_TblTreItem * const pc_ChannelItem =
            oq_UseChannelIndex ? pc_DomainItem->GetItem(ou32_ChannelIndex) : pc_DomainItem;

         if ((pc_Domain != NULL) && (pc_ChannelItem != NULL))
         {
            // insert model data
            if (ou32_ChannelIndex < pc_Domain->c_Channels.size())
            {
               // channel case
               pc_ChannelItem->c_Name = QString("%1 (%2)").arg(pc_Channel->c_Name.c_str(),
                                                               pc_Domain->c_Channels[ou32_ChannelIndex].c_Name.c_str());
            }
            else
            {
               // domain case
               pc_ChannelItem->c_Name = pc_Channel->c_Name.c_str();
            }
            pc_ChannelItem->c_ToolTipHeading = pc_ChannelItem->c_Name;
            pc_ChannelItem->c_ToolTipContent = pc_Channel->c_Comment.c_str();

            Q_EMIT (dataChanged(m_GetModelIndexFromIndexes(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex),
                                m_GetModelIndexFromIndexes(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex)));
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
      // update
      for (QModelIndexList::const_iterator c_ItIndex = orc_TargetIndexes.begin();
           c_ItIndex != orc_TargetIndexes.end(); ++c_ItIndex)
      {
         uint32 u32_DomainIndex;
         uint32 u32_ChannelIndex;
         bool q_ChannelCase;

         C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(*c_ItIndex, u32_DomainIndex, u32_ChannelIndex,
                                                                 q_ChannelCase);
         this->UpdateChannelText(u32_DomainIndex, u32_ChannelIndex, q_ChannelCase);
      }
   }
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
                                                             uint32 & oru32_DomainIndex, uint32 & oru32_ChannelIndex,
                                                             bool & orq_ChannelCase)
{
   oru32_DomainIndex = 0;
   oru32_ChannelIndex = 0;

   if (orc_ModelIndex.isValid() == true)
   {
      if (orc_ModelIndex.parent().isValid() == true)
      {
         // channel case
         oru32_DomainIndex = static_cast<uint32>(orc_ModelIndex.parent().row());
         oru32_ChannelIndex = static_cast<uint32>(orc_ModelIndex.row());
         orq_ChannelCase = true;
      }
      else
      {
         //domain case
         oru32_DomainIndex = static_cast<uint32>(orc_ModelIndex.row());
         // channel index not relevant in this case
         // attention: if parents are not disabled, further distinction of "has children" would be needed!
         orq_ChannelCase = false;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get model indexes from indexes

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_ChannelCase       Channel case

   \return  Corresponding model index
*/
//----------------------------------------------------------------------------------------------------------------------
QModelIndex C_SdNdeHalcChannelTreeModel::m_GetModelIndexFromIndexes(const uint32 ou32_DomainIndex,
                                                                    const uint32 ou32_ChannelIndex,
                                                                    const bool oq_ChannelCase) const
{
   QModelIndex c_ModelIndex = this->index(ou32_DomainIndex, 0); // flat tree so column is always 0

   if (oq_ChannelCase)
   {
      c_ModelIndex = c_ModelIndex.child(ou32_ChannelIndex, 0);
   }

   return c_ModelIndex;
}
