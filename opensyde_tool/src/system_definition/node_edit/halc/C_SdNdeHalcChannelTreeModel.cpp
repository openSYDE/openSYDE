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

#include "TGLUtils.h"
#include "C_TblTreItem.h"

#include "C_PuiSdHandler.h"

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
               // add channels
               for (uint32 u32_ChannelIt = 0; u32_ChannelIt < pc_Domain->c_Channels.size(); u32_ChannelIt++)
               {
                  C_TblTreItem * const pc_Child = new C_TblTreItem();
                  const C_OSCHalcConfigChannel & rc_ConfigChannel = pc_Domain->c_ChannelConfigs[u32_ChannelIt];

                  pc_Child->c_Name = QString("%1 (%2)").arg(rc_ConfigChannel.c_Name.c_str(),
                                                            pc_Domain->c_Channels[u32_ChannelIt].c_str());
                  pc_Child->c_ToolTipHeading = pc_Child->c_Name;
                  pc_Child->c_ToolTipContent = rc_ConfigChannel.c_Comment.c_str();

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
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeModel::UpdateChannelText(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex)
{
   Q_UNUSED(ou32_DomainIndex)
   Q_UNUSED(ou32_ChannelIndex)

   this->SetNode(this->mu32_NodeIndex); // TODO: only update specific item

   /*
   if ((this->mpc_InvisibleRootItem != NULL) && (ou32_DomainIndex < this->mpc_InvisibleRootItem->c_Children.size()))
   {
      C_TblTreItem * const pc_DomainItem =
         dynamic_cast<C_TblTreItem *>(this->mpc_InvisibleRootItem->c_Children[ou32_DomainIndex]);

      if (pc_DomainItem != NULL)
      {
         const C_OSCHalcConfigDomain * pc_Domain =
            C_PuiSdHandler::h_GetInstance()->GetDomainConfigDataConst(this->mu32_NodeIndex, ou32_DomainIndex);
         const C_OSCHalcConfigChannel * pc_Channel =
            C_PuiSdHandler::h_GetInstance()->GetDomainChannelConfigData(this->mu32_NodeIndex, ou32_DomainIndex,
                                                                        ou32_ChannelIndex);
         C_TblTreItem * pc_ChannelItem = pc_DomainItem->GetItem(ou32_ChannelIndex);

         if ((pc_Domain != NULL) && (ou32_ChannelIndex < pc_Domain->c_Channels.size()) && (pc_ChannelItem != NULL))
         {
            pc_ChannelItem->c_Name = QString("%1 (%2)").arg(pc_Channel->c_Name.c_str(),
                                                            pc_Domain->c_Channels[ou32_ChannelIndex].c_str());
            pc_ChannelItem->c_ToolTipHeading = pc_ChannelItem->c_Name;
            pc_ChannelItem->c_ToolTipContent = pc_Channel->c_Comment.c_str();
         }
      }
   }*/
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
