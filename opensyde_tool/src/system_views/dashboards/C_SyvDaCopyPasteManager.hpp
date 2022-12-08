//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle most parts of copy paste implementation for scene (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACOPYPASTEMANAGER_H
#define C_SYVDACOPYPASTEMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvDaDashboardSnapshot.hpp"
#include "C_SebBaseCopyPasteManager.hpp"
#include "C_PuiSvDbElementIdCrcGroup.hpp"
#include "C_PuiSvReadDataConfiguration.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaCopyPasteManager :
   public C_SebBaseCopyPasteManager
{
public:
   C_SyvDaCopyPasteManager(void);
   ~C_SyvDaCopyPasteManager(void) override;

   const C_PuiBsElements * GetSnapshot(QWidget * const opc_Parent) override;
   const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> * GetRails(void) const;
   void PrepareCopyFromSceneToManager(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex);
   void CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems, const QMap<const QGraphicsItem *,
                                                                                            float64_t> & orc_NormalizedZetValues)
   override;
   bool CheckValidContentAndPrepareData(void) override;

   static void h_AdaptCopyDataForPaste(stw::opensyde_gui_logic::C_PuiSvDashboard & orc_CopyData,
                                       QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                            C_PuiSvReadDataConfiguration> & orc_Rails,
                                       const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                                  C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups,
                                       const uint32_t ou32_ViewIndex, QWidget * const opc_Parent);

   static const QString hc_CLIP_BOARD_BASE_TAG_NAME;

protected:
   void m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data) override;

private:
   uint32_t mu32_ViewIndex;
   uint32_t mu32_DashboardIndex;
   C_SyvDaDashboardSnapshot mc_LastKnownData;
   QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> mc_LastKnownRails;
   QMap<C_PuiSvDbNodeDataPoolListElementId,
        C_PuiSvDbElementIdCrcGroup> mc_LastKnownElementIdGroups;

   static bool mh_ValidateCrcs(C_PuiSvDashboard & orc_Data, QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                                 C_PuiSvReadDataConfiguration> & orc_Rails,
                               const QMap<C_PuiSvDbNodeDataPoolListElementId,
                                          C_PuiSvDbElementIdCrcGroup> & orc_ElementIdGroups);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
