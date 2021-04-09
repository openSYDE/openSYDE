//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC configuration import items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwerrors.h"
#include "TGLUtils.h"

#include "C_GtGetText.h"

#include "C_SdNdeHalcConfigImportModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigImportItem::C_SdNdeHalcConfigImportItem(void) :
   C_TblTreItem(),
   u32_ImportIndex(0UL),
   e_CheckState(Qt::Unchecked),
   q_Checkable(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up model

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigImportModel::C_SdNdeHalcConfigImportModel(QObject * const opc_Parent) :
   C_TblTreModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigImportModel::~C_SdNdeHalcConfigImportModel(void)
{
   this->m_CleanUpLastModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the model

   \param[in]       orc_Config         HALC configuration of the current node
   \param[in]       orc_ImportConfig   Imported HALC configuration for updating

   \retval   C_NO_ERR   No errors, model is initialized
   \retval   C_NOACT    No domains does match (Id does not match), nothing to import
   \retval   C_CHECKSUM Configuration is not compatible
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcConfigImportModel::Init(const C_OSCHalcConfig & orc_Config,
                                          const C_OSCHalcConfigStandalone & orc_ImportConfig)
{
   sint32 s32_Return = C_NO_ERR;
   uint32 u32_ImpDomainCounter;
   bool q_AtLeastOneExists = false;
   C_SdNdeHalcConfigImportItem * pc_VisibleRootItem;

   // Save both configuration
   this->mc_ConfigCopy = orc_Config;
   this->mc_ImportConfigCopy = orc_ImportConfig;

   this->beginResetModel();

   // Clear
   this->m_CleanUpLastModel();

   this->mpc_InvisibleRootItem = new C_SdNdeHalcConfigImportItem();

   pc_VisibleRootItem = new C_SdNdeHalcConfigImportItem();
   pc_VisibleRootItem->c_Name = C_GtGetText::h_GetText("Configuration");

   // Check domains
   for (u32_ImpDomainCounter = 0U; u32_ImpDomainCounter < this->mc_ImportConfigCopy.c_Domains.size();
        ++u32_ImpDomainCounter)
   {
      const C_OSCHalcConfigStandaloneDomain & rc_ImpDomain = this->mc_ImportConfigCopy.c_Domains[u32_ImpDomainCounter];
      C_SdNdeHalcConfigImportItem * const pc_DomainItem = new C_SdNdeHalcConfigImportItem();
      uint32 u32_CurDomainCounter;

      pc_DomainItem->c_Name = rc_ImpDomain.c_DomainConfig.c_Name.c_str();
      pc_DomainItem->c_ToolTipHeading = pc_DomainItem->c_Name;
      pc_DomainItem->u32_ImportIndex = u32_ImpDomainCounter;
      if (rc_ImpDomain.c_Channels.empty() == true)
      {
         pc_DomainItem->c_ToolTipContent = rc_ImpDomain.c_DomainConfig.c_Comment.c_str();
         pc_DomainItem->c_ToolTipContent += "\n\n";
         pc_DomainItem->c_ToolTipContent += C_GtGetText::h_GetText("Configuration:\n");
         pc_DomainItem->c_ToolTipContent += C_GtGetText::h_GetText("   Safety Relevant Parameters: ");
         pc_DomainItem->c_ToolTipContent += rc_ImpDomain.c_DomainConfig.q_SafetyRelevant ?
                                            C_GtGetText::h_GetText("Yes") : C_GtGetText::h_GetText("No");
      }

      // Check domains
      for (u32_CurDomainCounter = 0U; u32_CurDomainCounter < this->mc_ConfigCopy.GetDomainSize();
           ++u32_CurDomainCounter)
      {
         const C_OSCHalcConfigDomain * const pc_CurConfig = this->mc_ConfigCopy.GetDomainConfigDataConst(
            u32_CurDomainCounter);

         if (pc_CurConfig != NULL)
         {
            if (pc_CurConfig->c_Id == rc_ImpDomain.c_Id)
            {
               // Domain Ids matches
               uint32 u32_ImpChannelCounter;

               pc_DomainItem->u32_Index = u32_CurDomainCounter;
               pc_DomainItem->q_Selectable = false;

               // If the domain has any channels
               if (rc_ImpDomain.c_Channels.empty() == false)
               {
                  // Check channels
                  for (u32_ImpChannelCounter = 0U; u32_ImpChannelCounter < rc_ImpDomain.c_StandaloneChannels.size();
                       ++u32_ImpChannelCounter)
                  {
                     C_SdNdeHalcConfigImportItem * const pc_ChannelItem = new C_SdNdeHalcConfigImportItem();
                     uint32 u32_CurChannelCounter;
                     const C_OSCHalcConfigChannel & rc_ImpChannelConfig =
                        rc_ImpDomain.c_ChannelConfigs[u32_ImpChannelCounter];
                     const C_OSCHalcDefChannelDef & rc_ImpChannelDef = rc_ImpDomain.c_Channels[u32_ImpChannelCounter];

                     pc_ChannelItem->c_Name =
                        static_cast<QString>("%1 (%2)").arg(
                           rc_ImpChannelConfig.c_Name.c_str(), rc_ImpChannelDef.c_Name.c_str());
                     pc_ChannelItem->c_ToolTipHeading = pc_ChannelItem->c_Name;
                     pc_ChannelItem->c_ToolTipContent =
                        this->m_CreateTooltipContent(u32_CurDomainCounter, u32_ImpChannelCounter);

                     pc_ChannelItem->u32_ImportIndex = u32_ImpChannelCounter;
                     pc_ChannelItem->q_Enabled = false;
                     pc_ChannelItem->q_Selectable = false;

                     for (u32_CurChannelCounter = 0U; u32_CurChannelCounter < pc_CurConfig->c_Channels.size();
                          ++u32_CurChannelCounter)
                     {
                        if (pc_CurConfig->c_Channels[u32_CurChannelCounter].c_Name == rc_ImpChannelDef.c_Name)
                        {
                           // Channel Ids match found
                           pc_ChannelItem->u32_Index = u32_CurChannelCounter;
                           pc_ChannelItem->q_Enabled = true;
                           pc_ChannelItem->q_Checkable = true;

                           // Check if channel is still matching

                           break;
                        }
                     }

                     pc_DomainItem->AddChild(pc_ChannelItem);
                  }
               }

               break;
            }
         }
      }

      q_AtLeastOneExists = true;
      pc_DomainItem->q_Enabled = true;
      pc_DomainItem->q_Checkable = true;

      pc_VisibleRootItem->AddChild(pc_DomainItem);
   }

   pc_VisibleRootItem->q_Enabled = q_AtLeastOneExists;
   pc_VisibleRootItem->q_Checkable = q_AtLeastOneExists;
   pc_VisibleRootItem->q_Selectable = false;

   this->mpc_InvisibleRootItem->AddChild(pc_VisibleRootItem);
   this->endResetModel();

   if (q_AtLeastOneExists == false)
   {
      // Nothing to import
      s32_Return = C_NOACT;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapts the selected elements of the HALC configuration set by Init function and returns the new config

   \param[out]    orc_AdaptedConfig   Adapted HALC configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportModel::GetAdaptedConfiguration(C_OSCHalcConfig & orc_AdaptedConfig)
{
   uint32 u32_DomainCounter;

   const C_SdNdeHalcConfigImportItem * pc_VisibleRootItem = NULL;

   if (this->mpc_InvisibleRootItem->c_Children.size() > 0)
   {
      pc_VisibleRootItem =
         dynamic_cast<const C_SdNdeHalcConfigImportItem * const>(this->mpc_InvisibleRootItem->c_Children[0]);
   }

   if (pc_VisibleRootItem != NULL)
   {
      orc_AdaptedConfig = this->mc_ConfigCopy;

      for (u32_DomainCounter = 0UL; u32_DomainCounter < pc_VisibleRootItem->c_Children.size();
           ++u32_DomainCounter)
      {
         const C_SdNdeHalcConfigImportItem * const pc_DomainItem =
            dynamic_cast<const C_SdNdeHalcConfigImportItem * const>(pc_VisibleRootItem->c_Children[u32_DomainCounter]);

         if ((pc_DomainItem != NULL) &&
             (pc_DomainItem->q_Enabled == true))
         {
            uint32 u32_ChannelCounter;
            const C_OSCHalcConfigDomain * const pc_DomainConfigToAdapt = orc_AdaptedConfig.GetDomainConfigDataConst(
               pc_DomainItem->u32_Index);

            tgl_assert(pc_DomainConfigToAdapt != NULL);
            tgl_assert(pc_DomainItem->u32_Index < orc_AdaptedConfig.GetDomainSize());
            tgl_assert(pc_DomainItem->u32_ImportIndex < this->mc_ImportConfigCopy.c_Domains.size());

            if (pc_DomainConfigToAdapt != NULL)
            {
               C_OSCHalcConfigDomain c_DomainConfigAdaption = *pc_DomainConfigToAdapt;
               const C_OSCHalcConfigDomain & rc_ImpDomainConfig =
                  this->mc_ImportConfigCopy.c_Domains[pc_DomainItem->u32_ImportIndex];
               bool q_Changed = false;

               if (pc_DomainItem->c_Children.size() == 0)
               {
                  if (pc_DomainItem->e_CheckState != Qt::Unchecked)
                  {
                     // Adapt domain configuration
                     // Update the domain specific properties without channel configuration
                     c_DomainConfigAdaption.c_DomainConfig = rc_ImpDomainConfig.c_DomainConfig;

                     q_Changed = true;
                  }
               }
               else
               {
                  // Channel configurations
                  for (u32_ChannelCounter = 0UL; u32_ChannelCounter < pc_DomainItem->c_Children.size();
                       ++u32_ChannelCounter)
                  {
                     const C_SdNdeHalcConfigImportItem * const pc_ChildItem =
                        dynamic_cast<const C_SdNdeHalcConfigImportItem * const>(
                           pc_DomainItem->c_Children[u32_ChannelCounter]);

                     if ((pc_ChildItem != NULL) &&
                         (pc_ChildItem->e_CheckState != Qt::Unchecked))
                     {
                        // Adapt channel
                        c_DomainConfigAdaption.c_ChannelConfigs[pc_ChildItem->u32_Index] =
                           rc_ImpDomainConfig.c_ChannelConfigs[pc_ChildItem->u32_ImportIndex];

                        q_Changed = true;
                     }
                  }
               }

               if (q_Changed == true)
               {
                  // Update the configuration with the adapted domain and channel configuration
                  tgl_assert(orc_AdaptedConfig.SetDomainConfig(pc_DomainItem->u32_Index,
                                                               c_DomainConfigAdaption) == C_NO_ERR);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if selection of linked channels is valid

   \param[out]     orc_DomainIndices            Domain indices corresponding to orc_MissingChannelIndices
   \param[out]     orc_MissingChannelIndices    Indices of all linked channels that are not checked

   \retval  true     For all selected linked channels the link buddies are also selected
   \retval  false    There exist linked channels where one is selected and the other one not
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigImportModel::IsSelectionOfLinkedChannelsValid(std::vector<uint32> & orc_DomainIndices,
                                                                    std::vector<std::vector<uint32> > & orc_MissingChannelIndices)
{
   bool q_LinkedValid = true;
   uint32 u32_DomainCounter;

   const C_SdNdeHalcConfigImportItem * pc_VisibleRootItem = NULL;

   if (this->mpc_InvisibleRootItem->c_Children.size() > 0)
   {
      pc_VisibleRootItem =
         dynamic_cast<const C_SdNdeHalcConfigImportItem * const>(this->mpc_InvisibleRootItem->c_Children[0]);
   }

   if (pc_VisibleRootItem != NULL)
   {
      for (u32_DomainCounter = 0UL; u32_DomainCounter < pc_VisibleRootItem->c_Children.size();
           ++u32_DomainCounter)
      {
         const C_SdNdeHalcConfigImportItem * const pc_DomainItem =
            dynamic_cast<const C_SdNdeHalcConfigImportItem * const>(pc_VisibleRootItem->c_Children[u32_DomainCounter]);
         std::map<uint32, bool> c_LinkBuddyCheckStates;

         if ((pc_DomainItem != NULL) && (pc_DomainItem->q_Enabled == true))
         {
            uint32 u32_ChannelCounter;

            tgl_assert(pc_DomainItem->u32_Index < this->mc_ConfigCopy.GetDomainSize());
            tgl_assert(pc_DomainItem->u32_ImportIndex < this->mc_ImportConfigCopy.c_Domains.size());

            const C_OSCHalcConfigDomain & rc_ImpDomainConfig =
               this->mc_ImportConfigCopy.c_Domains[pc_DomainItem->u32_ImportIndex];
            const C_OSCHalcConfigDomain * const pc_CopyDomainConfig =
               this->mc_ConfigCopy.GetDomainConfigDataConst(pc_DomainItem->u32_Index);

            if (pc_CopyDomainConfig != NULL)
            {
               for (u32_ChannelCounter = 0UL; u32_ChannelCounter < pc_DomainItem->c_Children.size();
                    ++u32_ChannelCounter)
               {
                  const C_SdNdeHalcConfigImportItem * const pc_ChildItem =
                     dynamic_cast<const C_SdNdeHalcConfigImportItem * const>(pc_DomainItem->c_Children[
                                                                                u32_ChannelCounter]);

                  if ((pc_ChildItem != NULL) && (pc_ChildItem->e_CheckState != Qt::Unchecked) &&
                      (pc_ChildItem->u32_ImportIndex < rc_ImpDomainConfig.c_ChannelConfigs.size()))
                  {
                     bool q_IsLinkedOld;
                     bool q_IsLinkedNew;
                     std::vector<uint32> c_LinkedChannelIndicesOld;
                     std::vector<uint32> c_LinkedChannelIndicesNew;
                     const C_OSCHalcConfigChannel & rc_Channel =
                        rc_ImpDomainConfig.c_ChannelConfigs[pc_ChildItem->u32_ImportIndex];

                     // use copy for linked check (standalone config has no use case information!)
                     // together with imported use case index
                     pc_CopyDomainConfig->CheckChannelLinked(u32_ChannelCounter, true, q_IsLinkedNew, NULL,
                                                             &c_LinkedChannelIndicesNew, &rc_Channel.u32_UseCaseIndex);
                     pc_CopyDomainConfig->CheckChannelLinked(u32_ChannelCounter, true, q_IsLinkedOld, NULL,
                                                             &c_LinkedChannelIndicesOld, NULL);

                     // check of linked channels: are all link-buddies also checked?
                     if ((q_IsLinkedNew == true) || (q_IsLinkedOld == true))
                     {
                        std::map<uint32, bool>::const_iterator c_ItFind;

                        // if current channel is not in the map, its buddies are also not there and need to be added
                        c_ItFind = c_LinkBuddyCheckStates.find(u32_ChannelCounter);
                        if (c_ItFind == c_LinkBuddyCheckStates.end())
                        {
                           uint32 u32_LinkedChannelCounter;

                           if (q_IsLinkedNew == true)
                           {
                              // add all linked channel buddies for channels that get linked
                              for (u32_LinkedChannelCounter = 0UL;
                                   u32_LinkedChannelCounter < c_LinkedChannelIndicesNew.size();
                                   u32_LinkedChannelCounter++)
                              {
                                 c_ItFind =
                                    c_LinkBuddyCheckStates.find(c_LinkedChannelIndicesNew[u32_LinkedChannelCounter]);
                                 if (c_ItFind == c_LinkBuddyCheckStates.end())
                                 {
                                    // only add / set to false if it not already exists
                                    c_LinkBuddyCheckStates[c_LinkedChannelIndicesNew[u32_LinkedChannelCounter]] = false;
                                 }
                              }
                           }

                           if (q_IsLinkedOld == true)
                           {
                              // add all linked channel buddies for channels that were linked
                              for (u32_LinkedChannelCounter = 0UL;
                                   u32_LinkedChannelCounter < c_LinkedChannelIndicesOld.size();
                                   u32_LinkedChannelCounter++)
                              {
                                 c_ItFind =
                                    c_LinkBuddyCheckStates.find(c_LinkedChannelIndicesOld[u32_LinkedChannelCounter]);
                                 if (c_ItFind == c_LinkBuddyCheckStates.end())
                                 {
                                    // only add / set to false if it not already exists
                                    c_LinkBuddyCheckStates[c_LinkedChannelIndicesOld[u32_LinkedChannelCounter]] = false;
                                 }
                              }
                           }
                        }

                        // current channel is certainly checked
                        c_LinkBuddyCheckStates[u32_ChannelCounter] = true;
                     }
                  }
               }
            }
         }

         if (c_LinkBuddyCheckStates.empty() == false)
         {
            // find all missing linked channels and collect their indices
            std::map<uint32, bool>::const_iterator c_ItLinkBuddies;
            std::vector<uint32> c_MissingChannelIndicesPerDomain;
            for (c_ItLinkBuddies = c_LinkBuddyCheckStates.begin(); c_ItLinkBuddies != c_LinkBuddyCheckStates.end();
                 ++c_ItLinkBuddies)
            {
               if (c_ItLinkBuddies->second == false)
               {
                  c_MissingChannelIndicesPerDomain.push_back(c_ItLinkBuddies->first);
                  q_LinkedValid = false;
               }
            }

            if (c_MissingChannelIndicesPerDomain.empty() == false)
            {
               orc_DomainIndices.push_back(u32_DomainCounter);
               orc_MissingChannelIndices.push_back(c_MissingChannelIndicesPerDomain);
            }
         }
      }
   }
   return q_LinkedValid;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channels

   This function only works if all domains and channels are visible, i.e. if index = row in tree.

   \param[in]  orc_DomainIndices    Domain indices
   \param[in]  orc_ChannelIndices   Channel indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportModel::CheckChannels(const std::vector<uint32> & orc_DomainIndices,
                                                 const std::vector<std::vector<uint32> > & orc_ChannelIndices)
{
   tgl_assert(orc_DomainIndices.size() == orc_ChannelIndices.size());

   const QModelIndex & rc_VisibleRoot = this->index(0, 0);
   if (rc_VisibleRoot.isValid() == true)
   {
      for (uint32 u32_DomainCounter = 0; u32_DomainCounter < orc_DomainIndices.size(); u32_DomainCounter++)
      {
         const std::vector<uint32> & rc_ChannelIndices = orc_ChannelIndices[u32_DomainCounter];
         const QModelIndex & rc_Domain = rc_VisibleRoot.child(orc_DomainIndices[u32_DomainCounter], 0);

         if (rc_Domain.isValid() == true)
         {
            for (uint32 u32_ChannelCounter = 0; u32_ChannelCounter < rc_ChannelIndices.size(); u32_ChannelCounter++)
            {
               const QModelIndex & rc_Channel = rc_Domain.child(rc_ChannelIndices[u32_ChannelCounter], 0);
               this->setData(rc_Channel, static_cast<sintn>(Qt::Checked), static_cast<sintn>(Qt::CheckStateRole));
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get tree column count

   \param[in] orc_Parent Parent

   \return
   Column count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeHalcConfigImportModel::columnCount(const QModelIndex & orc_Parent) const
{
   Q_UNUSED(orc_Parent)
   return 1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data at index

   \param[in]  orc_Index   Index
   \param[in]  osn_Role    Data role

   \return
   Data
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_SdNdeHalcConfigImportModel::data(const QModelIndex & orc_Index, const sintn osn_Role) const
{
   QVariant c_Return;

   if ((osn_Role == static_cast<sintn>(Qt::CheckStateRole)) &&
       (orc_Index.isValid() == true))
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      C_SdNdeHalcConfigImportItem * const pc_TreeItem =
         static_cast<C_SdNdeHalcConfigImportItem * const>(orc_Index.internalPointer());

      if (pc_TreeItem != NULL)
      {
         c_Return = static_cast<sintn>(pc_TreeItem->e_CheckState);
      }
   }
   else
   {
      c_Return = C_TblTreModel::data(orc_Index, osn_Role);
   }

   return c_Return;
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
bool C_SdNdeHalcConfigImportModel::setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                                           const sintn osn_Role)
{
   bool q_Return = false;

   if (osn_Role == static_cast<sintn>(Qt::CheckStateRole))
   {
      if (orc_Index.isValid() == true)
      {
         //lint -e{9079}  Result of Qt interface restrictions, set by index function
         C_SdNdeHalcConfigImportItem * const pc_TreeItem =
            static_cast<C_SdNdeHalcConfigImportItem * const>(orc_Index.internalPointer());

         if (pc_TreeItem != NULL)
         {
            QModelIndex c_StartIndex = orc_Index;
            QModelIndex c_EndIndex = orc_Index;

            // Switching from unchecked and partial to checked
            pc_TreeItem->e_CheckState = (pc_TreeItem->e_CheckState == Qt::Checked) ? Qt::Unchecked : Qt::Checked;

            // Check if the item has children
            this->m_CheckChildren(pc_TreeItem, c_StartIndex, c_EndIndex);

            // or has a parent
            this->m_CheckParent(pc_TreeItem, orc_Index, c_StartIndex);

            Q_EMIT (this->dataChanged(c_StartIndex, c_EndIndex, QVector<stw_types::sintn>() << osn_Role));
            q_Return = true;
         }
      }
   }
   else
   {
      q_Return = C_TblTreModel::setData(orc_Index, orc_Value, osn_Role);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get item flags

   \param[in] orc_Index Index

   \return
   Item flags
*/
//----------------------------------------------------------------------------------------------------------------------
Qt::ItemFlags C_SdNdeHalcConfigImportModel::flags(const QModelIndex & orc_Index) const
{
   Qt::ItemFlags c_Retval = C_TblTreModel::flags(orc_Index);

   if (orc_Index.isValid() == true)
   {
      //lint -e{9079}  Result of Qt interface restrictions, set by index function
      const C_SdNdeHalcConfigImportItem * const pc_TreeItem =
         static_cast<const C_SdNdeHalcConfigImportItem * const>(orc_Index.internalPointer());
      if (pc_TreeItem != NULL)
      {
         if (pc_TreeItem->q_Checkable == true)
         {
            c_Retval = c_Retval | Qt::ItemIsUserCheckable;
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks all children (recursive) and adapts the check state to the state of the parent

   \param[in,out]   opc_TreeItem   Parent to check its children
   \param[in]       orc_ItemIndex  Index of the parent
   \param[out]      orc_EndIndex   Last index of the last child
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportModel::m_CheckChildren(C_SdNdeHalcConfigImportItem * const opc_TreeItem,
                                                   const QModelIndex & orc_ItemIndex, QModelIndex & orc_EndIndex)
{
   if (opc_TreeItem->c_Children.size() > 0)
   {
      uint32 u32_ChildCounter;
      const uint32 u32_IndexLastChild = opc_TreeItem->c_Children.size() - 1UL;

      orc_EndIndex = this->index(u32_IndexLastChild, 0, orc_ItemIndex);

      for (u32_ChildCounter = 0U; u32_ChildCounter < opc_TreeItem->c_Children.size(); ++u32_ChildCounter)
      {
         C_SdNdeHalcConfigImportItem * const pc_ChildItem =
            dynamic_cast<C_SdNdeHalcConfigImportItem * const>(opc_TreeItem->c_Children[u32_ChildCounter]);
         const QModelIndex c_ChildIndex = this->index(u32_ChildCounter, 0, orc_ItemIndex);

         // Set all children to the same checked state
         if (pc_ChildItem != NULL)
         {
            pc_ChildItem->e_CheckState = opc_TreeItem->e_CheckState;

            // Check the children for their children
            this->m_CheckChildren(pc_ChildItem, c_ChildIndex, orc_EndIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the parent (recursive) and adapts the check state to the state of the child

   \param[in]       opc_TreeItem   Parent to check its children
   \param[in]       orc_ItemIndex  Index of the child
   \param[out]      orc_StartIndex First index of the first parent
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportModel::m_CheckParent(const C_SdNdeHalcConfigImportItem * const opc_TreeItem,
                                                 const QModelIndex & orc_ItemIndex, QModelIndex & orc_StartIndex)
{
   if (opc_TreeItem->pc_Parent != NULL)
   {
      C_SdNdeHalcConfigImportItem * const pc_ParentItem =
         dynamic_cast<C_SdNdeHalcConfigImportItem * const>(opc_TreeItem->pc_Parent);

      if (pc_ParentItem != NULL)
      {
         bool q_AllAreChecked = true;
         bool q_AtLeastOneIsChecked = false;
         uint32 u32_ChildCounter;
         const QModelIndex c_ParentIndex = this->parent(orc_ItemIndex);
         orc_StartIndex = c_ParentIndex;

         // Check the other children to get the necessary state of the parent
         for (u32_ChildCounter = 0U; u32_ChildCounter < pc_ParentItem->c_Children.size(); ++u32_ChildCounter)
         {
            C_SdNdeHalcConfigImportItem * const pc_ChildItem =
               dynamic_cast<C_SdNdeHalcConfigImportItem * const>(pc_ParentItem->c_Children[u32_ChildCounter]);

            if (pc_ChildItem != NULL)
            {
               if (pc_ChildItem->e_CheckState == Qt::Unchecked)
               {
                  q_AllAreChecked = false;
               }
               else
               {
                  q_AtLeastOneIsChecked = true;
               }
            }
         }

         if (q_AllAreChecked == true)
         {
            pc_ParentItem->e_CheckState = Qt::Checked;
         }
         else if (q_AtLeastOneIsChecked == true)
         {
            pc_ParentItem->e_CheckState = Qt::PartiallyChecked;
         }
         else
         {
            pc_ParentItem->e_CheckState = Qt::Unchecked;
         }

         // Check the parent of the parent
         this->m_CheckParent(pc_ParentItem, c_ParentIndex, orc_StartIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clean up current model

   Warning: pointer might be invalid after call of this function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportModel::m_CleanUpLastModel(void)
{
   if (this->mpc_InvisibleRootItem != NULL)
   {
      delete (this->mpc_InvisibleRootItem);
      this->mpc_InvisibleRootItem = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create tool tip content

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index

   \return
   tool tip content
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeHalcConfigImportModel::m_CreateTooltipContent(const uint32 ou32_DomainIndex,
                                                             const uint32 ou32_ChannelIndex) const
{
   QString c_Return;

   if ((ou32_DomainIndex < this->mc_ImportConfigCopy.c_Domains.size()) &&
       (ou32_DomainIndex < this->mc_ConfigCopy.GetDomainSize()))
   {
      const C_OSCHalcConfigDomain * const pc_CurDomain = this->mc_ConfigCopy.GetDomainConfigDataConst(ou32_DomainIndex);
      const C_OSCHalcConfigStandaloneDomain & rc_ImpDomain =  this->mc_ImportConfigCopy.c_Domains[ou32_DomainIndex];

      if ((pc_CurDomain != NULL) && (ou32_ChannelIndex < rc_ImpDomain.c_ChannelConfigs.size()))
      {
         const C_OSCHalcConfigChannel & rc_ImpChannelConfig = rc_ImpDomain.c_ChannelConfigs[ou32_ChannelIndex];

         std::vector<uint32> c_LinkedChannelIndices;
         bool q_IsLinked = false;

         // comment
         c_Return = rc_ImpChannelConfig.c_Comment.c_str();
         c_Return += "\n\n";

         c_Return += C_GtGetText::h_GetText("Configuration: ");
         c_Return += "\n";

         // safety flag
         c_Return += C_GtGetText::h_GetText("   Safety Relevant Parameters: ");
         c_Return += rc_ImpChannelConfig.q_SafetyRelevant ?
                     C_GtGetText::h_GetText("Yes") : C_GtGetText::h_GetText("No");
         c_Return += "\n";

         // selected use case

         if (rc_ImpChannelConfig.u32_UseCaseIndex < pc_CurDomain->c_ChannelUseCases.size())
         // use pc_CurDomain for use cases because they are part of the definition
         {
            const C_OSCHalcDefChannelUseCase & rc_UseCase =
               pc_CurDomain->c_ChannelUseCases[rc_ImpChannelConfig.u32_UseCaseIndex];
            c_Return += C_GtGetText::h_GetText("   Use Case: ");
            c_Return += rc_UseCase.c_Display.c_str();
            c_Return += "\n";
         }

         // linked channel information
         pc_CurDomain->CheckChannelLinked(ou32_ChannelIndex, true, q_IsLinked, NULL,
                                          &c_LinkedChannelIndices, &rc_ImpChannelConfig.u32_UseCaseIndex);
         // import configuration does not know about linked channels so we need to ask current domain;
         // and we want the names of the import channel to be displayed so we cannot use opc_LinkedChannelNames
         if (q_IsLinked == true)
         {
            c_Return += C_GtGetText::h_GetText("   Linked to: ");
            for (uint32 u32_LinkBuddyCounter = 0; u32_LinkBuddyCounter < c_LinkedChannelIndices.size();
                 u32_LinkBuddyCounter++)
            {
               const uint32 u32_CurLinkedChannelIndex = c_LinkedChannelIndices[u32_LinkBuddyCounter];

               if (u32_LinkBuddyCounter != 0)
               {
                  c_Return += ", ";
               }

               if ((u32_CurLinkedChannelIndex < rc_ImpDomain.c_ChannelConfigs.size()) &&
                   (u32_CurLinkedChannelIndex < pc_CurDomain->c_Channels.size()))
               {
                  c_Return += rc_ImpDomain.c_ChannelConfigs[u32_CurLinkedChannelIndex].c_Name.c_str();
                  c_Return += " (";
                  c_Return += pc_CurDomain->c_Channels[u32_CurLinkedChannelIndex].c_Name.c_str();
                  c_Return += ")";
               }
            }
         }
      }
   }
   return c_Return;
}
