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
#include "stwtypes.h"

#include "C_TblTreModel.h"
#include "C_TblTreItem.h"

#include "C_OSCHalcConfig.h"
#include "C_OSCHalcConfigStandalone.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigImportItem :
   public C_TblTreItem
{
public:
   C_SdNdeHalcConfigImportItem(void);
   stw_types::uint32 u32_ImportIndex;
   Qt::CheckState e_CheckState;

   bool q_Checkable;
};

class C_SdNdeHalcConfigImportModel :
   public C_TblTreModel
{
public:
   explicit C_SdNdeHalcConfigImportModel(QObject * const opc_Parent = NULL);
   ~C_SdNdeHalcConfigImportModel(void);

   stw_types::sint32 Init(const stw_opensyde_core::C_OSCHalcConfig & orc_Config,
                          const stw_opensyde_core::C_OSCHalcConfigStandalone & orc_ImportConfig);
   void GetAdaptedConfiguration(stw_opensyde_core::C_OSCHalcConfig & orc_AdaptedConfig);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions,
   // and default parameters are identical.
   //lint -save -e1960 -e1735
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role) override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

private:
   void m_CheckChildren(C_SdNdeHalcConfigImportItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                        QModelIndex & orc_EndIndex);
   void m_CheckParent(const C_SdNdeHalcConfigImportItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                      QModelIndex & orc_StartIndex);
   void m_CleanUpLastModel(void);

   stw_opensyde_core::C_OSCHalcConfig mc_ConfigCopy;
   stw_opensyde_core::C_OSCHalcConfigStandalone mc_ImportConfigCopy;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
