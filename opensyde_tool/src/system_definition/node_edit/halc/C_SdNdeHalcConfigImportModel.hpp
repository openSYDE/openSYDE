//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC configuration import items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCONFIGIMPORTMODEL_H
#define C_SDNDEHALCCONFIGIMPORTMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_TblTreModel.hpp"
#include "C_TblTreItem.hpp"

#include "C_OscHalcConfig.hpp"
#include "C_OscHalcConfigStandalone.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigImportItem :
   public C_TblTreItem
{
public:
   C_SdNdeHalcConfigImportItem(void);
   uint32_t u32_ImportIndex;
   Qt::CheckState e_CheckState;

   bool q_Checkable;
};

class C_SdNdeHalcConfigImportModel :
   public C_TblTreModel
{
public:
   explicit C_SdNdeHalcConfigImportModel(QObject * const opc_Parent = NULL);
   ~C_SdNdeHalcConfigImportModel(void) noexcept override;

   int32_t Init(const stw::opensyde_core::C_OscHalcConfig & orc_Config,
                const stw::opensyde_core::C_OscHalcConfigStandalone & orc_ImportConfig);
   void GetAdaptedConfiguration(stw::opensyde_core::C_OscHalcConfig & orc_AdaptedConfig);
   bool IsSelectionOfLinkedChannelsValid(std::vector<uint32_t> & orc_DomainIndices,
                                         std::vector<std::vector<uint32_t> > & orc_MissingChannelIndices);
   void CheckChannels(const std::vector<uint32_t> & orc_DomainIndices,
                      const std::vector<std::vector<uint32_t> > & orc_ChannelIndices);

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const int32_t os32_Role) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

private:
   void m_CheckChildren(C_SdNdeHalcConfigImportItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                        QModelIndex & orc_EndIndex);
   void m_CheckParent(const C_SdNdeHalcConfigImportItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                      QModelIndex & orc_StartIndex);
   void m_CleanUpLastModel(void);
   QString m_CreateTooltipContent(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex) const;

   stw::opensyde_core::C_OscHalcConfig mc_ConfigCopy;
   stw::opensyde_core::C_OscHalcConfigStandalone mc_ImportConfigCopy;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
