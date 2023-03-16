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
#include "stwtypes.hpp"

#include "C_TblTreeModelCheckable.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_PopServiceProjSettingsModel :
   public C_TblTreeModelCheckable
{
public:
   explicit C_PopServiceProjSettingsModel(QObject * const opc_Parent = NULL);
   ~C_PopServiceProjSettingsModel(void) override;

   int32_t Init(const std::vector<uint32_t> & orc_ElementIndices) override;
   void GetCheckedItems(std::vector<uint32_t> & orc_ElementIndices,
                        std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerElement) const override;
   uint32_t GetCheckedItemCount(void) const override;

private:
   void m_CheckInitItems(C_TblTreeModelCheckableItem & orc_VisibleRootItem,
                         const std::vector<uint32_t> & orc_ViewIndices) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
