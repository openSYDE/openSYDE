//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common graphics scene with system view functionality (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVTOPOLOGYBASESCENE_H
#define C_SYVTOPOLOGYBASESCENE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SebTopologyBaseScene.hpp"
#include "C_GiSvPcBusConnector.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvTopologyBaseScene :
   public C_SebTopologyBaseScene
{
public:
   C_SyvTopologyBaseScene(const uint32_t ou32_ViewIndex = 0, QObject * const opc_Parent = NULL);
   ~C_SyvTopologyBaseScene() override;

   void Load(void) override;

   //lint -e{1735} Suppression, because default parameters are identical
   void CopyFromManagerToScene(const QPointF * const opc_Pos = NULL) override;

   bool IsItemDeletable(const QGraphicsItem * const opc_Item) const override;
   bool IsZetOrderChangeable(const QGraphicsItem * const opc_Item) const override;
   bool IsAlignmentUsable(const QGraphicsItem * const opc_Item) const override;
   bool IsSceneRubberBandAvailable(void) const override;

protected:
   stw::opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManager(void) override;
   const stw::opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManagerConst(void) const override;

   C_GiLiCanBus * m_CreateCanBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                 C_GiTextElementBus * const opc_TextElementName,
                                 const std::vector<QPointF> * const opc_Points,
                                 QGraphicsItem * const opc_Parent) override;
   C_GiLiEthernetBus * m_CreateEthernetBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                           C_GiTextElementBus * const opc_TextElementName,
                                           const std::vector<QPointF> * const opc_Points,
                                           QGraphicsItem * const opc_Parent) override;
   C_GiTextElementBus * m_CreateBusTextElement(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                               QGraphicsItem * const opc_Parent) override;

   void m_AddBusToScene(C_GiLiBus * const opc_BusGraphicsItem) override;
   void m_AddBusConnectorToScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem) override;
   void m_AddTextElementToScene(C_GiBiTextElement * const opc_Item) override;
   void m_AddLineArrowToScene(C_GiBiArrow * const opc_Item) override;
   void m_AddBoundaryToScene(C_GiBiBoundary * const opc_Item) override;
   void m_AddImageGroupToScene(C_GiBiImageGroup * const opc_Item) override;
   void m_AddRectBaseGroupToScene(C_GiBiRectBaseGroup * const opc_Item) override;
   void m_AddLineGroupToScene(C_GiLiLineGroup * const opc_Item) override;
   virtual void m_AddPcBusConnectorToScene(C_GiSvPcBusConnector * const opc_PcBusConnectorGraphicsItem);

   // drag and drop function
   bool m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position) override;

   // copy paste functions
   void m_Copy(void) override;
   void m_Cut(void) override;
   C_GiLiBus * m_CheckBusState(void) const;
   void m_AddPcBusConnector(const C_GiLiBus * const opc_Bus, C_GiBiConnectableItem * const opc_Pc, const stw::opensyde_gui_logic::C_PuiBsLineBase & orc_LineData =
                               stw::opensyde_gui_logic::C_PuiBsLineBase());
   static QPointF mh_GetInitialConnectionPoint(const QGraphicsItem * const opc_Item);
   bool m_HandlePcReconnectIfNecessary(QString & orc_BusName);

   const uint32_t mu32_ViewIndex;

private:
   //Avoid call
   C_SyvTopologyBaseScene(const C_SyvTopologyBaseScene &);
   C_SyvTopologyBaseScene & operator =(const C_SyvTopologyBaseScene &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
