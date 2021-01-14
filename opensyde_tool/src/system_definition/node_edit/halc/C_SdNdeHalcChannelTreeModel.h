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
#include "C_TblTreModel.h"
#include "C_TblTreItem.h"
#include "C_OSCHalcConfig.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeHalcChannelTreeModel :
   public C_TblTreModel
{
   Q_OBJECT

public:
   C_SdNdeHalcChannelTreeModel(QObject * const opc_Parent = NULL);
   ~C_SdNdeHalcChannelTreeModel(void);

   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   void SetNode(const stw_types::uint32 ou32_NodeIndex);
   void UpdateChannelText(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                          const bool oq_UseChannelIndex);
   void Clear(void);
   void Copy(const QModelIndex & orc_CurrentChannel) const;
   void Paste(QWidget * const opc_Parent, const QModelIndexList & orc_TargetIndexes);
   void Reset(const QModelIndexList & orc_Indexes);
   void CheckError(const stw_types::uint32 ou32_DomainIndex);
   stw_types::uint32 GetNodeIndex(void) const;

   static void h_GetIndexesFromModelIndex(const QModelIndex & orc_ModelIndex, stw_types::uint32 & oru32_DomainIndex,
                                          stw_types::uint32 & oru32_ChannelIndex, bool & orq_ChannelCase);
   QModelIndex GetModelIndexFromIndexes(const stw_types::uint32 ou32_DomainIndex,
                                        const stw_types::uint32 ou32_ChannelIndex, const bool oq_ChannelCase) const;

private:
   C_SdNdeHalcChannelTreeModel(const C_SdNdeHalcChannelTreeModel &);
   C_SdNdeHalcChannelTreeModel & operator =(const C_SdNdeHalcChannelTreeModel &);

   stw_types::uint32 mu32_NodeIndex;
   std::vector<QIcon> mc_InputIcons;
   std::vector<QIcon> mc_OutputIcons;
   std::vector<QIcon> mc_OtherIcons;

   std::vector<QIcon> mc_Icons;

   static void mh_SetChannelText(C_TblTreItem * const opc_Item, const QString & orc_ChannelName,
                                 const QString & orc_ChannelID, const QString & orc_Comment);
   static QIcon mh_InitIcon(const stw_opensyde_core::C_OSCHalcDefDomain::E_Category oe_Category, const bool oq_Large,
                            const bool oq_Error, const bool oq_Linked);
   QIcon m_GetIcon(const stw_opensyde_core::C_OSCHalcDefDomain::E_Category oe_Category, const bool oq_Large,
                   const bool oq_Error, const bool oq_Linked) const;
   stw_types::uint32 m_GetIconIdentifier(const stw_opensyde_core::C_OSCHalcDefDomain::E_Category oe_Category,
                                         const bool oq_Large, const bool oq_Error, const bool oq_Linked) const;
   void m_InitIconsOfCategory(const stw_opensyde_core::C_OSCHalcDefDomain::E_Category oe_Category);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
