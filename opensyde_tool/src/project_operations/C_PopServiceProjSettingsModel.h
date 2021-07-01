//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for Create Service Dialog permission tree
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPSERVICEPROJSETTINGSMODEL_H
#define C_POPSERVICEPROJSETTINGSMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_TblTreeModelCheckable.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_PopServiceProjSettingsModel :
   public C_TblTreeModelCheckable
{
public:
   explicit C_PopServiceProjSettingsModel(QObject * const opc_Parent = NULL);
   ~C_PopServiceProjSettingsModel(void);

   virtual stw_types::sint32 Init(const std::vector<stw_types::uint32> & orc_ElementIndices);
   virtual void GetCheckedItems(std::vector<stw_types::uint32> & orc_ElementIndices,
                                std::vector<std::vector<stw_types::uint32> > & orc_ChildIndicesPerElement) const;
   virtual stw_types::uint32 GetCheckedItemCount(void) const;

private:
   void m_CheckInitItems(C_TblTreeModelCheckableItem & orc_VisibleRootItem,
                         const std::vector<stw_types::uint32> & orc_ViewIndices) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
