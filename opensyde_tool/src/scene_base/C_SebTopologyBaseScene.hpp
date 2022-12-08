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

#include "C_SebScene.hpp"
#include "C_GiLiCanBus.hpp"
#include "C_GiLiEthernetBus.hpp"

#include "C_PuiSdCompleteBusConnectionData.hpp"
#include "C_SdManTopologyBusConnectorReconnectManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebTopologyBaseScene :
   public C_SebScene
{
   Q_OBJECT

public:
   C_SebTopologyBaseScene(QObject * const opc_Parent = NULL);
   ~C_SebTopologyBaseScene() override;

   virtual void Load(void);

   void CheckAllItemsForChanges(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeChanged(const uint32_t ou32_Index);
   void SigBusChanged(const uint32_t ou32_Index);

protected:
   void m_LoadProject(void);
   void m_LoadSubset(const QVector<uint32_t> & orc_NodeIndices, const QVector<uint32_t> & orc_BusIndices,
                     const QVector<uint32_t> & orc_OtherStartIndices, const bool & orq_Selection, const std::vector<
                        stw::opensyde_gui_logic::C_PuiSdCompleteBusConnectionData
                        > * const opc_AdditionalConnectionData,
                     const QMap<stw::opensyde_gui_logic::C_PuiBsTemporaryDataId,
                                uint64_t> * const opc_IdMap);
   uint64_t m_GetNewUniqueBusConnectorId(const QMap<
                                            stw::opensyde_gui_logic::C_PuiBsTemporaryDataId,
                                            uint64_t> * const opc_IdMap = NULL, const uint32_t & oru32_Index = 0, const std::vector<stw::opensyde_gui_logic::C_PuiSdCompleteBusConnectionData> * const opc_AdditionalConnectionData =
                                            NULL);

   virtual C_GiNode * m_CreateNode(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                   const float64_t & orf64_Width, const float64_t & orf64_Height,
                                   QGraphicsItem * const opc_Parent) = 0;
   virtual C_GiLiCanBus * m_CreateCanBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                         C_GiTextElementBus * const opc_TextElementName,
                                         const std::vector<QPointF> * const opc_Points,
                                         QGraphicsItem * const opc_Parent) = 0;
   virtual C_GiLiEthernetBus * m_CreateEthernetBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                                   C_GiTextElementBus * const opc_TextElementName,
                                                   const std::vector<QPointF> * const opc_Points,
                                                   QGraphicsItem * const opc_Parent) = 0;
   virtual C_GiTextElementBus * m_CreateBusTextElement(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                                       QGraphicsItem * const opc_Parent) = 0;

   virtual void m_AddBusTextElementToScene(C_GiTextElementBus * const opc_Item);
   virtual void m_AddNodeToScene(C_GiNode * const opc_NodeGraphicsItem);
   virtual void m_AddBusToScene(C_GiLiBus * const opc_BusGraphicsItem);
   virtual void m_AddBusConnectorToScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem);
   virtual void m_AddBusConnectorBaseToScene(C_GiLiBusConnectorBase * const opc_BusConnectorGraphicsItem);

   void m_InitSceneUndoManager(void);
   void m_RestoreCursors(void) const;

   void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

   stw::opensyde_gui_logic::C_SdManTopologyBusConnectorReconnectManager mc_BusConnectorReconnectManager;

private:
   //Avoid call
   C_SebTopologyBaseScene(const C_SebTopologyBaseScene &);
   C_SebTopologyBaseScene & operator =(const C_SebTopologyBaseScene &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
