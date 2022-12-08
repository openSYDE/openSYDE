//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class for system definition items (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYADDDELETEBASECOMMAND_H
#define C_SDMANUNOTOPOLOGYADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QMap>
#include <QObject>
#include "C_GiNode.hpp"
#include "C_SebUnoAddDeleteBaseCommand.hpp"
#include "C_PuiSdDataElement.hpp"
#include "C_SdTopologyDataSnapshot.hpp"
#include "C_PuiBsTemporaryDataId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyAddDeleteBaseCommand :
   public QObject,
   public C_SebUnoAddDeleteBaseCommand

{
   Q_OBJECT

public:
   C_SdManUnoTopologyAddDeleteBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                          const QString & orc_Text, QUndoCommand * const opc_Parent = NULL, const C_SdTopologyDataSnapshot & orc_InitialSnapshotData =
                                             C_SdTopologyDataSnapshot());
   ~C_SdManUnoTopologyAddDeleteBaseCommand(void) override;
   static void h_GetAllRelevantObjects(const QList<QGraphicsItem *> & orc_AllItems,
                                       const QList<QGraphicsItem *> & orc_SelectedItems,
                                       QList<QGraphicsItem *> & orc_AllRelevantItems);
   C_SdTopologyDataSnapshot GetDataBackup() const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);

protected:
   void m_DeleteSave(void) override;
   void m_Restore(void) override;

   int32_t m_GetBoundaryType(void) const override;
   int32_t m_GetImageType(void) const override;
   int32_t m_GetLineArrowType(void) const override;
   int32_t m_GetTextElementType(void) const override;

   void m_SetDataPositionOffset(const QPointF & orc_NewPos);
   void m_SetDataZetOffset(const float64_t of64_HighestUsedZetValue);

private:
   QMap<C_PuiBsTemporaryDataId, uint64_t> mc_MapTypeAndIndexToId;
   C_SdTopologyDataSnapshot mc_DataBackup;

   static void mh_SearchAndAddAllAffectedBusConnectorsToTheFront(const QList<QGraphicsItem *> & orc_AllItems,
                                                                 const int32_t & ors32_BusIndex,
                                                                 const int32_t & ors32_NodeIndex,
                                                                 QList<QGraphicsItem *> & orc_AllRelevantItems);
   static void mh_SearchAndAddAllAffectedBusTextElementsToTheFront(const QList<QGraphicsItem *> & orc_AllItems,
                                                                   const uint32_t ou32_BusIndex,
                                                                   QList<QGraphicsItem *> & orc_AllRelevantItems);
   void m_SaveToData(void);
   void m_SaveToDataNode(const stw::opensyde_gui::C_GiNode * const opc_UiNodeItem, const uint32_t ou32_NodeIndex,
                         const uint64_t ou64_UniqueId, uint32_t & oru32_NodeCounter);
   void m_SaveToDataMultiNode(const uint32_t ou32_NodeIndex);
   void m_Clear(void);
   void m_Delete(void);
   void m_HandleCanOpenBeforeDelete(void) const;
   void m_HandleCanOpenNodeBeforeDelete(const uint32_t ou32_Index) const;
   void m_HandleCanOpenManagerBeforeDelete(const uint32_t ou32_Index) const;
   void m_HandleCanOpenDeviceBeforeDelete(const uint32_t ou32_Index) const;
   void m_HandleCanOpenBusBeforeDelete(const uint32_t ou32_Index) const;
   void m_HandleCanOpenNodeBusConnectorBeforeDelete(const uint32_t ou32_NodeIndex,
                                                    const C_PuiSdNodeConnectionId & orc_ConnectionId) const;
   void m_HandleCanOpenManagerNodeBusConnectorBeforeDelete(const uint32_t ou32_NodeIndex,
                                                           const C_PuiSdNodeConnectionId & orc_ConnectionId) const;
   void m_HandleCanOpenDeviceNodeBusConnectorBeforeDelete(const uint32_t ou32_NodeIndex,
                                                          const C_PuiSdNodeConnectionId & orc_ConnectionId) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
