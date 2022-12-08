//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channel overview.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCHANNELTREEMODEL_H
#define C_SDNDEHALCCHANNELTREEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblTreModel.hpp"
#include "C_TblTreItem.hpp"
#include "C_OscHalcConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeHalcChannelTreeModel :
   public C_TblTreModel
{
   Q_OBJECT

public:
   C_SdNdeHalcChannelTreeModel(QObject * const opc_Parent = NULL);
   ~C_SdNdeHalcChannelTreeModel(void) override;

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   void SetNode(const uint32_t ou32_NodeIndex);
   void UpdateChannelText(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                          const bool oq_UseChannelIndex);
   void Clear(void);
   void Copy(const QModelIndex & orc_CurrentChannel) const;
   void Paste(QWidget * const opc_Parent, const QModelIndexList & orc_TargetIndexes);
   void Reset(const QModelIndexList & orc_Indexes);
   void CheckError(const uint32_t ou32_DomainIndex);
   uint32_t GetNodeIndex(void) const;

   static void h_GetIndexesFromModelIndex(const QModelIndex & orc_ModelIndex, uint32_t & oru32_DomainIndex,
                                          uint32_t & oru32_ChannelIndex, bool & orq_ChannelCase);
   QModelIndex GetModelIndexFromIndexes(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                                        const bool oq_ChannelCase) const;

private:
   C_SdNdeHalcChannelTreeModel(const C_SdNdeHalcChannelTreeModel &);
   C_SdNdeHalcChannelTreeModel & operator =(const C_SdNdeHalcChannelTreeModel &) &;

   uint32_t mu32_NodeIndex;
   std::vector<QIcon> mc_InputIcons;
   std::vector<QIcon> mc_OutputIcons;
   std::vector<QIcon> mc_OtherIcons;

   std::vector<QIcon> mc_Icons;

   static void mh_SetChannelText(C_TblTreItem * const opc_Item, const QString & orc_ChannelName,
                                 const QString & orc_ChannelID, const QString & orc_Comment);
   static QIcon mh_InitIcon(const stw::opensyde_core::C_OscHalcDefDomain::E_Category oe_Category, const bool oq_Large,
                            const bool oq_Error, const bool oq_Linked);
   QIcon m_GetIcon(const stw::opensyde_core::C_OscHalcDefDomain::E_Category oe_Category, const bool oq_Large,
                   const bool oq_Error, const bool oq_Linked) const;
   uint32_t m_GetIconIdentifier(const stw::opensyde_core::C_OscHalcDefDomain::E_Category oe_Category,
                                const bool oq_Large, const bool oq_Error, const bool oq_Linked) const;
   void m_InitIconsOfCategory(const stw::opensyde_core::C_OscHalcDefDomain::E_Category oe_Category);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
