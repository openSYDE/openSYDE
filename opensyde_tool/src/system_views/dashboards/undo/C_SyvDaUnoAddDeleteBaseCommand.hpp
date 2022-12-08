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
#include "C_SyvDaDashboardSnapshot.hpp"
#include "C_PuiSvReadDataConfiguration.hpp"
#include "C_SebUnoAddDeleteBaseCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaUnoAddDeleteBaseCommand :
   public C_SebUnoAddDeleteBaseCommand
{
public:
   C_SyvDaUnoAddDeleteBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                  const QString & orc_Text, QUndoCommand * const opc_Parent = NULL, const C_SyvDaDashboardSnapshot & orc_InitialSnapshotData =
                                     C_SyvDaDashboardSnapshot());

protected:
   void m_DeleteSave(void) override;
   void m_Restore(void) override;
   void m_RestoreReadRailsOnly(void);

   int32_t m_GetBoundaryType(void) const override;
   int32_t m_GetImageType(void) const override;
   int32_t m_GetLineArrowType(void) const override;
   int32_t m_GetTextElementType(void) const override;

   void m_SetDataPositionOffset(const QPointF & orc_NewPos);
   void m_SetDataZetOffset(const float64_t of64_HighestUsedZetValue);

   C_PuiSvDashboard m_GetDataBackup(void) const;
   int32_t m_GetStoredReadRailCount(void) const;
   int32_t m_AddReadRailToInternalBackup(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                         const C_PuiSvReadDataConfiguration & orc_Value);

private:
   QMap<C_PuiBsTemporaryDataId, uint64_t> mc_MapTypeAndIndexToId;
   C_SyvDaDashboardSnapshot mc_DataBackup;
   QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> mc_SavedRailAssignments;

   void m_SaveToData(void);
   void m_Clear(void);
   void m_Delete(void);
   static const C_PuiSvDbWidgetBase * mh_GetGenericWidget(const uint32_t ou32_ViewIndex,
                                                          const uint32_t ou32_DashboardIndex,
                                                          const C_PuiSvDbDataElement * const opc_DataElement);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
