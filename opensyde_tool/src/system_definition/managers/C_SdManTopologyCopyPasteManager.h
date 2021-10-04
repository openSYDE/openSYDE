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
#include "C_GiNode.h"
#include "C_SebBaseCopyPasteManager.h"
#include "C_PuiSdNodeConnectionId.h"
#include "C_PuiBsLineBase.h"
#include "C_SdTopologyDataSnapshot.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManTopologyCopyPasteManager :
   public C_SebBaseCopyPasteManager
{
public:
   C_SdManTopologyCopyPasteManager(void);
   virtual ~C_SdManTopologyCopyPasteManager(void);
   virtual const C_PuiBsElements * GetSnapshot(QWidget * const opc_Parent) override;
   virtual void CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems,
                                       const QMap<const QGraphicsItem *, stw_types::float64> & orc_NormalizedZValues)
   override;
   virtual bool CheckValidContentAndPrepareData(void) override;

protected:
   virtual void m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data) override;

private:
   C_SdTopologyDataSnapshot mc_LastKnownData;

   static void mh_CopyFromSceneToManagerHandleNode(const stw_opensyde_gui::C_GiNode * const opc_UiNodeItem,
                                                   C_SdTopologyDataSnapshot & orc_Snapshot,
                                                   const QMap<const QGraphicsItem *,
                                                              stw_types::float64> & orc_NormalizedZValues,
                                                   const QGraphicsItem * const opc_NodeItemOrigin);
   static void mh_CopyFromSceneToManagerHandleMultiNode(const stw_types::uint32 ou32_NodeIndex,
                                                        C_SdTopologyDataSnapshot & orc_Snapshot,
                                                        const QMap<const QGraphicsItem *,
                                                                   stw_types::float64> & orc_NormalizedZValues,
                                                        const QGraphicsItem * const opc_NodeItemOrigin);
   static void mh_RemoveConnection(C_SdTopologyDataSnapshot & orc_Data, const stw_types::uint32 & oru32_NodeIndex,
                                   const C_PuiSdNodeConnectionId & orc_ConnectionId);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
