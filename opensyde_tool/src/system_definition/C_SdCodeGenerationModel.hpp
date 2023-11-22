//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for file generation items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDCODEGENERATIONMODEL_HPP
#define C_SDCODEGENERATIONMODEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_TblTreeModelCheckable.hpp"

#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdCodeGenerationModel :
   public C_TblTreeModelCheckable
{
   Q_OBJECT

public:
   explicit C_SdCodeGenerationModel(QObject * const opc_Parent = NULL);
   ~C_SdCodeGenerationModel() override;

   int32_t Init(const std::vector<uint32_t> & orc_ElementIndices) override;
   void GetCheckedItems(std::vector<uint32_t> & orc_ElementIndices,
                        std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerElement) const override;
   uint32_t GetCheckedItemCount(void) const override;

private:
   void m_CheckInitItems(C_TblTreeModelCheckableItem & orc_VisibleRootItem,
                         const std::vector<uint32_t> & orc_NodeIndices) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
