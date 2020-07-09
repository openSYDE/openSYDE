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

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -restore

   void SetNode(const stw_types::uint32 ou32_NodeIndex);
   void UpdateChannelText(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                          const bool oq_UseChannelIndex);
   void Clear(void);
   void Copy(const QModelIndex & orc_CurrentChannel) const;
   void Paste(QWidget * const opc_Parent, const QModelIndexList & orc_TargetIndexes);

   static void h_GetIndexesFromModelIndex(const QModelIndex & orc_ModelIndex, stw_types::uint32 & oru32_DomainIndex,
                                          stw_types::uint32 & oru32_ChannelIndex, bool & orq_ChannelCase);

private:
   C_SdNdeHalcChannelTreeModel(const C_SdNdeHalcChannelTreeModel &);
   C_SdNdeHalcChannelTreeModel & operator =(const C_SdNdeHalcChannelTreeModel &);

   stw_types::uint32 mu32_NodeIndex;

   QModelIndex m_GetModelIndexFromIndexes(const stw_types::uint32 ou32_DomainIndex,
                                          const stw_types::uint32 ou32_ChannelIndex, const bool oq_ChannelCase) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
