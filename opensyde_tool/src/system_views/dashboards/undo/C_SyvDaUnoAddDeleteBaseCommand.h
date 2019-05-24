//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class for dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAUNOADDDELETEBASECOMMAND_H
#define C_SYVDAUNOADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDashboard.h"
#include "C_PuiSvReadDataConfiguration.h"
#include "C_SebUnoAddDeleteBaseCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoAddDeleteBaseCommand :
   public C_SebUnoAddDeleteBaseCommand
{
public:
   C_SyvDaUnoAddDeleteBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                  const QString & orc_Text, QUndoCommand * const opc_Parent = NULL,
                                  const C_PuiSvDashboard & orc_InitialSnapshotData = C_PuiSvDashboard());

protected:
   virtual void m_DeleteSave(void);
   virtual void m_Restore(void);
   void m_RestoreReadRailsOnly(void);

   virtual stw_types::sint32 GetBoundaryType(void) const;
   virtual stw_types::sint32 GetImageType(void) const;
   virtual stw_types::sint32 GetLineArrowType(void) const;
   virtual stw_types::sint32 GetTextElementType(void) const;

   C_PuiSvDashboard GetDataBackup(void) const;
   stw_types::sint32 GetStoredReadRailCount(void) const;
   stw_types::sint32 AddReadRailToInternalBackup(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                                 const C_PuiSvReadDataConfiguration & orc_Value);

private:
   QMap<C_PuiBsTemporaryDataID, stw_types::uint64> mc_MapTypeAndIndexToID;
   C_PuiSvDashboard mc_DataBackup;
   QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> mc_SavedRailAssignments;

   void m_SaveToData(void);
   void m_Clear(void);
   void m_Delete(void);
   static const C_PuiSvDbWidgetBase * mh_GetGenericWidget(const stw_types::uint32 ou32_ViewIndex,
                                                          const stw_types::uint32 ou32_DashboardIndex,
                                                          const C_PuiSvDbDataElement * const opc_DataElement);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
