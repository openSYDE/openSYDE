//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for file generation items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDCODEGENERATIONMODEL_H
#define C_SDCODEGENERATIONMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_TblTreeModelCheckable.h"

#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdCodeGenerationModel :
   public C_TblTreeModelCheckable
{
   Q_OBJECT

public:
   explicit C_SdCodeGenerationModel(QObject * const opc_Parent = NULL);
   ~C_SdCodeGenerationModel();

   virtual stw_types::sint32 Init(const std::vector<stw_types::uint32> & orc_ElementIndices);
   virtual void GetCheckedItems(std::vector<stw_types::uint32> & orc_ElementIndices,
                                std::vector<std::vector<stw_types::uint32> > & orc_ChildIndicesPerElement) const;
   virtual stw_types::uint32 GetCheckedItemCount(void) const;

private:
   void m_CheckInitItems(C_TblTreeModelCheckableItem & orc_VisibleRootItem,
                         const std::vector<stw_types::uint32> & orc_NodeIndices) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
