//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle most parts of copy paste implementation for scene (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANTOPOLOGYCOPYPASTEMANAGER_H
#define C_SDMANTOPOLOGYCOPYPASTEMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GiNode.hpp"
#include "C_SebBaseCopyPasteManager.hpp"
#include "C_PuiSdNodeConnectionId.hpp"
#include "C_PuiBsLineBase.hpp"
#include "C_SdTopologyDataSnapshot.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManTopologyCopyPasteManager :
   public C_SebBaseCopyPasteManager
{
public:
   C_SdManTopologyCopyPasteManager(void);
   ~C_SdManTopologyCopyPasteManager(void) override;
   const C_PuiBsElements * GetSnapshot(QWidget * const opc_Parent) override;
   void CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems, const QMap<const QGraphicsItem *,
                                                                                            float64_t> & orc_NormalizedZetValues)
   override;
   bool CheckValidContentAndPrepareData(void) override;

protected:
   void m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data) override;

private:
   C_SdTopologyDataSnapshot mc_LastKnownData;

   static void mh_CopyFromSceneToManagerHandleNode(const stw::opensyde_gui::C_GiNode * const opc_UiNodeItem,
                                                   C_SdTopologyDataSnapshot & orc_Snapshot,
                                                   const QMap<const QGraphicsItem *,
                                                              float64_t> & orc_NormalizedZetValues,
                                                   const QGraphicsItem * const opc_NodeItemOrigin);
   static void mh_CopyFromSceneToManagerHandleMultiNode(const uint32_t ou32_NodeIndex,
                                                        C_SdTopologyDataSnapshot & orc_Snapshot,
                                                        const QMap<const QGraphicsItem *,
                                                                   float64_t> & orc_NormalizedZetValues,
                                                        const QGraphicsItem * const opc_NodeItemOrigin);
   static void mh_RemoveConnection(C_SdTopologyDataSnapshot & orc_Data, const uint32_t & oru32_NodeIndex,
                                   const C_PuiSdNodeConnectionId & orc_ConnectionId);
   static void mh_RemoveCanOpenInformation(C_SdTopologyDataSnapshot & orc_Data);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
