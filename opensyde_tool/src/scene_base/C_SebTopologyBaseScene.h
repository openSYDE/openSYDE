//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common graphics scene with topology functionality (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBTOPOLOGYBASESCENE_H
#define C_SEBTOPOLOGYBASESCENE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVector>

#include "C_SebScene.h"
#include "C_GiLiCANBus.h"
#include "C_GiLiEthernetBus.h"

#include "C_PuiSdCompleteBusConnectionData.h"
#include "C_SdManTopologyBusConnectorReconnectManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebTopologyBaseScene :
   public C_SebScene
{
   Q_OBJECT

public:
   C_SebTopologyBaseScene(QObject * const opc_Parent = NULL);
   virtual ~C_SebTopologyBaseScene();

   virtual void Load(void);

   void CheckAllItemsForChanges(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeChanged(const stw_types::uint32 ou32_Index);
   void SigBusChanged(const stw_types::uint32 ou32_Index);

protected:
   void m_LoadProject(void);
   void m_LoadSubset(const QVector<stw_types::uint32> & orc_NodeIndices,
                     const QVector<stw_types::uint32> & orc_BusIndices,
                     const QVector<stw_types::uint32> & orc_OtherStartIndices, const bool & orq_Selection, const std::vector<
                        stw_opensyde_gui_logic::C_PuiSdCompleteBusConnectionData
                        > * const opc_AdditionalConnectionData,
                     const QMap<stw_opensyde_gui_logic::C_PuiBsTemporaryDataID,
                                stw_types::uint64> * const opc_IDMap);
   stw_types::uint64 m_GetNewUniqueBusConnectorID(const QMap<
                                                     stw_opensyde_gui_logic::C_PuiBsTemporaryDataID,
                                                     stw_types::uint64> * const opc_IDMap = NULL,
                                                  const stw_types::uint32 & oru32_Index = 0, const std::vector<stw_opensyde_gui_logic::C_PuiSdCompleteBusConnectionData> * const opc_AdditionalConnectionData =
                                                     NULL);

   virtual C_GiNode * m_CreateNode(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                                   const stw_types::float64 & orf64_Width, const stw_types::float64 & orf64_Height,
                                   QGraphicsItem * const opc_Parent) = 0;
   virtual C_GiLiCANBus * m_CreateCANBus(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                                         C_GiTextElementBus * const opc_TextElementName,
                                         const std::vector<QPointF> * const opc_Points,
                                         QGraphicsItem * const opc_Parent) = 0;
   virtual C_GiLiEthernetBus * m_CreateEthernetBus(const stw_types::sint32 & ors32_Index,
                                                   const stw_types::uint64 & oru64_ID,
                                                   C_GiTextElementBus * const opc_TextElementName,
                                                   const std::vector<QPointF> * const opc_Points,
                                                   QGraphicsItem * const opc_Parent) = 0;
   virtual C_GiTextElementBus * m_CreateBusTextElement(const stw_types::sint32 & ors32_Index,
                                                       const stw_types::uint64 & oru64_ID,
                                                       QGraphicsItem * const opc_Parent) = 0;

   virtual void m_AddBusTextElementToScene(C_GiTextElementBus * const opc_Item);
   virtual void m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem);
   virtual void m_AddBusToScene(C_GiLiBus * const opc_BusGraphicsItem);
   virtual void m_AddBusConnectorToScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem);
   virtual void m_AddBusConnectorBaseToScene(C_GiLiBusConnectorBase * const opc_BusConnectorGraphicsItem);

   void m_InitSceneUndoManager(void);
   void m_RestoreCursors(void) const;

   virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

   stw_opensyde_gui_logic::C_SdManTopologyBusConnectorReconnectManager mc_BusConnectorReconnectManager;

private:
   //Avoid call
   C_SebTopologyBaseScene(const C_SebTopologyBaseScene &);
   C_SebTopologyBaseScene & operator =(const C_SebTopologyBaseScene &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
