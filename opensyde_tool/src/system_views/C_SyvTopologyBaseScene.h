//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Common graphics scene with system view functionality (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVTOPOLOGYBASESCENE_H
#define C_SYVTOPOLOGYBASESCENE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_SebTopologyBaseScene.h"
#include "C_GiSvPcBusConnector.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvTopologyBaseScene :
   public C_SebTopologyBaseScene
{
public:
   C_SyvTopologyBaseScene(const stw_types::uint32 ou32_ViewIndex, QObject * const opc_Parent = NULL);
   virtual ~C_SyvTopologyBaseScene();

   virtual void Load(void);

   //lint -e{1735} Suppression, because default parameters are identical
   virtual void CopyFromManagerToScene(const QPointF * const opc_Pos = NULL) override;

   virtual bool IsItemDeletable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsZOrderChangeable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsAlignmentUsable(const QGraphicsItem * const opc_Item) const override;
   virtual bool IsSceneRubberBandAvailable(void) const override;

protected:
   virtual stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManager(void) override;
   virtual const stw_opensyde_gui_logic::C_SebBaseCopyPasteManager * m_GetCopyPasteManagerConst(void) const override;

   virtual C_GiLiCANBus * m_CreateCANBus(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                                         C_GiTextElementBus * const opc_TextElementName,
                                         const std::vector<QPointF> * const opc_Points,
                                         QGraphicsItem * const opc_Parent) override;
   virtual C_GiLiEthernetBus * m_CreateEthernetBus(const stw_types::sint32 & ors32_Index,
                                                   const stw_types::uint64 & oru64_ID,
                                                   C_GiTextElementBus * const opc_TextElementName,
                                                   const std::vector<QPointF> * const opc_Points,
                                                   QGraphicsItem * const opc_Parent) override;
   virtual C_GiTextElementBus * m_CreateBusTextElement(const stw_types::sint32 & ors32_Index,
                                                       const stw_types::uint64 & oru64_ID,
                                                       QGraphicsItem * const opc_Parent) override;

   virtual void m_AddBusToScene(C_GiLiBus * const opc_BusGraphicsItem) override;
   virtual void m_AddBusConnectorToScene(C_GiLiBusConnector * const opc_BusConnectorGraphicsItem) override;
   virtual void m_AddTextElementToScene(C_GiBiTextElement * const opc_Item) override;
   virtual void m_AddLineArrowToScene(C_GiBiArrow * const opc_Item) override;
   virtual void m_AddBoundaryToScene(C_GiBiBoundary * const opc_Item) override;
   virtual void m_AddImageGroupToScene(C_GiBiImageGroup * const opc_Item) override;
   virtual void m_AddRectBaseGroupToScene(C_GiBiRectBaseGroup * const opc_Item) override;
   virtual void m_AddLineGroupToScene(C_GiLiLineGroup * const opc_Item) override;
   virtual void m_AddPcBusConnectorToScene(C_GiSvPcBusConnector * const opc_PcBusConnectorGraphicsItem);

   // drag and drop function
   virtual bool m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position) override;

   // copy past functions
   virtual void m_Copy(void);
   virtual void m_Cut(void);
   C_GiLiBus * m_CheckBusState(void) const;
   void m_AddPcBusConnector(const C_GiLiBus * const opc_Bus, C_GiBiConnectableItem * const opc_Pc, const stw_opensyde_gui_logic::C_PuiBsLineBase & orc_LineData =
                               stw_opensyde_gui_logic::C_PuiBsLineBase());
   static QPointF mh_GetInitialConnectionPoint(const QGraphicsItem * const opc_Item);
   bool m_HandlePcReconnectIfNecessary(QString & orc_BusName);

   const stw_types::uint32 mu32_ViewIndex;

private:
   //Avoid call
   C_SyvTopologyBaseScene(const C_SyvTopologyBaseScene &);
   C_SyvTopologyBaseScene & operator =(const C_SyvTopologyBaseScene &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
