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
   q_UseParentIndex(false),
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

   \retval   C_NO_ERR  No errors, model is initialized
   \retval   C_NOACT   No domains does match (Id does not match), nothing to import
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcConfigImportModel::Init(const C_OSCHalcConfig & orc_Config,
                                          const C_OSCHalcConfigStandalone & orc_ImportConfig)
{
   sint32 s32_Return = C_NO_ERR;
   uint32 u32_ImpDomainCounter;
   bool q_AtLeastOnEnabled = false;

   // Save both configuration
   this->mc_ConfigCopy = orc_Config;
   this->mc_ImportConfigCopy = orc_ImportConfig;

   this->beginResetModel();

   // Clear
   this->m_CleanUpLastModel();
   this->mpc_InvisibleRootItem = new C_SdNdeHalcConfigImportItem();

   // Check domains
   for (u32_ImpDomainCounter = 0U; u32_ImpDomainCounter < this->mc_ImportConfigCopy.c_Domains.size();
        ++u32_ImpDomainCounter)
   {
      const C_OSCHalcConfigStandaloneDomain & rc_ImpDomain = this->mc_ImportConfigCopy.c_Domains[u32_ImpDomainCounter];
      C_SdNdeHalcConfigImportItem * const pc_DomainItem = new C_SdNdeHalcConfigImportItem();
      uint32 u32_CurDomainCounter;

      pc_DomainItem->c_Name = rc_ImpDomain.c_DomainConfig.c_Name.c_str();
      pc_DomainItem->u32_ImportIndex = u32_ImpDomainCounter;

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

               // If the domain does not have any channels, add the domain configuration itself as item
               if (pc_CurConfig->c_Channels.size() == 0)
               {
                  C_SdNdeHalcConfigImportItem * const pc_DomainConfigItem = new C_SdNdeHalcConfigImportItem();

                  pc_DomainConfigItem->c_Name = C_GtGetText::h_GetText("Domain configuration");
                  pc_DomainConfigItem->q_UseParentIndex = true;
                  pc_DomainConfigItem->q_Selectable = false;
                  pc_DomainConfigItem->q_Enabled = true;
                  pc_DomainConfigItem->q_Checkable = true;

                  pc_DomainItem->AddChild(pc_DomainConfigItem);
               }

               // Check channels
               for (u32_ImpChannelCounter = 0U; u32_ImpChannelCounter < rc_ImpDomain.c_StandaloneChannels.size();
                    ++u32_ImpChannelCounter)
               {
                  C_SdNdeHalcConfigImportItem * const pc_ChannelItem = new C_SdNdeHalcConfigImportItem();
                  uint32 u32_CurChannelCounter;

                  pc_ChannelItem->c_Name = rc_ImpDomain.c_Channels[u32_ImpChannelCounter].c_Name.c_str();
                  pc_ChannelItem->c_Name =
                     QString("%1 (%2)").arg(rc_ImpDomain.c_ChannelConfigs[u32_ImpChannelCounter].c_Name.c_str(),
                                            rc_ImpDomain.c_Channels[u32_ImpChannelCounter].c_Name.c_str());

                  pc_ChannelItem->u32_ImportIndex = u32_ImpChannelCounter;
                  pc_ChannelItem->q_Enabled = false;
                  pc_ChannelItem->q_Selectable = false;

                  for (u32_CurChannelCounter = 0U; u32_CurChannelCounter < pc_CurConfig->c_Channels.size();
                       ++u32_CurChannelCounter)
                  {
                     if (pc_CurConfig->c_Channels[u32_CurChannelCounter].c_Name ==
                         rc_ImpDomain.c_Channels[u32_ImpChannelCounter].c_Name)
                     {
                        // Channel Ids match found
                        pc_ChannelItem->u32_Index = u32_CurChannelCounter;
                        pc_ChannelItem->q_Enabled = true;
                        pc_ChannelItem->q_Checkable = true;

                        break;
                     }
                  }

                  pc_DomainItem->AddChild(pc_ChannelItem);
               }

               break;
            }
         }
      }

      if (pc_DomainItem->c_Children.size() > 0)
      {
         q_AtLeastOnEnabled = true;
         pc_DomainItem->q_Enabled = true;
         pc_DomainItem->q_Checkable = true;
      }
      else
      {
         pc_DomainItem->q_Enabled = false;
         pc_DomainItem->q_Selectable = false;
      }

      this->mpc_InvisibleRootItem->AddChild(pc_DomainItem);
   }

   this->endResetModel();

   if (q_AtLeastOnEnabled == false)
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

   orc_AdaptedConfig = this->mc_ConfigCopy;

   for (u32_DomainCounter = 0UL; u32_DomainCounter < this->mpc_InvisibleRootItem->c_Children.size();
        ++u32_DomainCounter)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_SdNdeHalcConfigImportItem * const pc_DomainItem =
         dynamic_cast<const C_SdNdeHalcConfigImportItem * const>(
            this->mpc_InvisibleRootItem->c_Children[u32_DomainCounter]);

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

            // Domain configuration and channels
            for (u32_ChannelCounter = 0UL; u32_ChannelCounter < pc_DomainItem->c_Children.size(); ++u32_ChannelCounter)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               const C_SdNdeHalcConfigImportItem * const pc_ChildItem =
                  dynamic_cast<const C_SdNdeHalcConfigImportItem * const>(
                     pc_DomainItem->c_Children[u32_ChannelCounter]);

               if ((pc_ChildItem != NULL) &&
                   (pc_ChildItem->e_CheckState != Qt::Unchecked))
               {
                  if (pc_ChildItem->q_UseParentIndex == true)
                  {
                     // Adapt domain configuration
                     // Update the domain specific properties without channel configuration
                     c_DomainConfigAdaption.c_DomainConfig = rc_ImpDomainConfig.c_DomainConfig;
                  }
                  else
                  {
                     // Adapt channel
                     c_DomainConfigAdaption.c_ChannelConfigs[pc_ChildItem->u32_Index] =
                        rc_ImpDomainConfig.c_ChannelConfigs[pc_ChildItem->u32_ImportIndex];
                  }

                  q_Changed = true;
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
      //lint -e{925}  Result of Qt interface restrictions, set by index function
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
         //lint -e{925}  Result of Qt interface restrictions, set by index function
         C_SdNdeHalcConfigImportItem * const pc_TreeItem =
            static_cast<C_SdNdeHalcConfigImportItem * const>(orc_Index.internalPointer());

         if (pc_TreeItem != NULL)
         {
            QModelIndex c_StartIndex = orc_Index;
            QModelIndex c_EndIndex = orc_Index;

            // Switching from unchecked and partial to checked
            pc_TreeItem->e_CheckState = (pc_TreeItem->e_CheckState == Qt::Checked) ? Qt::Unchecked : Qt::Checked;

            // Check if the item has children
            if (pc_TreeItem->c_Children.size() > 0)
            {
               uint32 u32_ChildCounter;
               const uint32 u32_IndexLastChild = pc_TreeItem->c_Children.size() - 1UL;

               for (u32_ChildCounter = 0U; u32_ChildCounter < pc_TreeItem->c_Children.size(); ++u32_ChildCounter)
               {
                  //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
                  C_SdNdeHalcConfigImportItem * const pc_ChildItem =
                     dynamic_cast<C_SdNdeHalcConfigImportItem * const>(pc_TreeItem->c_Children[u32_ChildCounter]);

                  // Set all children to the same checked state
                  if (pc_ChildItem != NULL)
                  {
                     pc_ChildItem->e_CheckState = pc_TreeItem->e_CheckState;
                  }
               }

               c_EndIndex = this->index(u32_IndexLastChild, 0, c_StartIndex);
            }
            // or has a parent
            else if (pc_TreeItem->pc_Parent != NULL)
            {
               //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
               C_SdNdeHalcConfigImportItem * const pc_ParentItem =
                  dynamic_cast<C_SdNdeHalcConfigImportItem * const>(pc_TreeItem->pc_Parent);

               if (pc_ParentItem != NULL)
               {
                  bool q_AllAreChecked = true;
                  bool q_AtLeastOneIsChecked = false;
                  uint32 u32_ChildCounter;
                  c_StartIndex = this->parent(orc_Index);

                  // Check the other children to get the necessary state of the parent
                  for (u32_ChildCounter = 0U; u32_ChildCounter < pc_ParentItem->c_Children.size(); ++u32_ChildCounter)
                  {
                     //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
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
               }
            }
            else
            {
               // Nothing to do
            }

            //lint -e{1793} Qt example
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
      //lint -e{925}  Result of Qt interface restrictions, set by index function
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
