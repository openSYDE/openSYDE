//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers visualization and functionality of a node (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILINODE_HPP
#define C_GILINODE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsDropShadowEffect>

#include "stwtypes.hpp"
#include "C_GiPort.hpp"
#include "C_OscNode.hpp"
#include "C_GiRectPixmap.hpp"
#include "C_GiNodeBoundary.hpp"
#include "C_GiRectBaseGroup.hpp"
#include "C_GiBiCustomToolTip.hpp"
#include "C_PuiSdNodeConnectionId.hpp"
#include "C_PuiSdNodeInterfaceAutomaticProperties.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiLiBusConnector;

class C_GiNode :
   public C_GiRectBaseGroup,
   public C_GiBiCustomToolTip
{
   Q_OBJECT

public:
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiNode(const int32_t & ors32_Index, const uint64_t & oru64_Id,
            const float64_t & orf64_Width = mhf64_INITIAL_WIDTH_NODE,
            const float64_t & orf64_Height = mhf64_INITIAL_HEIGHT_NODE, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiNode() override;
   void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const override;
   virtual void FindClosestPort(const QPointF &orc_ScenePoint, C_GiPort * (&orpc_Closest)) const;
   int32_t type() const override;

   QString GetText(void) const;
   QString GetFirstSelectableFullName(void) const;

   void AddConnection(C_GiLiBusConnector * const opc_Connection);
   void AddConnectionAndData(C_GiLiBusConnector * const opc_Connection,
                             const stw::opensyde_gui_logic::C_PuiSdNodeConnectionId & orc_NodeConnection,
                             const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties, const uint32_t & oru32_BusIndex);
   void UpdateConnection(const C_GiLiBusConnector * const opc_Connection,
                         const stw::opensyde_gui_logic::C_PuiSdNodeConnectionId & orc_NodeConnection,
                         const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                         const uint32_t & oru32_BusIndex) const;
   void ChangeInterface(const uint8_t ou8_Interface, const C_GiLiBusConnector * const opc_Connection,
                        const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties)
   const;
   const stw::opensyde_gui_logic::C_PuiSdNodeConnectionId * GetNodeConnectionId(
      const C_GiLiBusConnector * const opc_Connection) const;
   void LoadData(void) override;
   void UpdateData(void) override;
   void RemoveConnector(const C_GiLiBusConnector * const opc_BusConnectorGraphicsItem);
   bool CheckIndexRelevantForThisNode(const uint32_t ou32_NodeIndex) const;
   void DeleteData(void) override;
   bool HasConnectionType(const stw::opensyde_core::C_OscSystemBus::E_Type oe_Type) const;
   bool CheckConnectionAvailable(const stw::opensyde_core::C_OscSystemBus::E_Type & ore_Type) const;
   bool CheckInterfaceAvailable(const stw::opensyde_core::C_OscSystemBus::E_Type & ore_Type,
                                const uint8_t & oru8_Interface);
   int32_t GetIndexOfConnector(const C_GiLiBusConnector * const opc_Connection) const;
   void GetOscNodeConst(const stw::opensyde_core::C_OscNode * (&orpc_Node)) const;
   void RestoreDefaultCursor(void) override;
   void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) override;
   void GenerateHint(void) override;
   void ReevaluatePortState(void);
   void CheckNodeForChanges(void);

   bool GetValid(void) const;

   void SetDrawWhiteFilter(const bool oq_Active);

   //GI base
   void SetZetValueCustom(const float64_t of64_ZetValue) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigOpenContextMenu(const QPointF & orc_Pos);
   void SigHideToolTip(void);

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;

   virtual bool m_UpdateError(void);
   bool m_GetErrorStatus(void) const;
   void m_ResizeUpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight) override;

   void m_SetDrawBorder(const bool oq_Active);
   const C_GiNodeBoundary * m_GetBoundary(void) const;
   int32_t m_GetIconSize(void) const;
   uint32_t m_GetScaleCategory(void) const;

   static const float64_t mhf64_MIN_WIDTH_NODE;
   static const float64_t mhf64_MIN_HEIGHT_NODE;
   static const float64_t mhf64_INITIAL_WIDTH_NODE;
   static const float64_t mhf64_INITIAL_HEIGHT_NODE;
   static const uint32_t mhu32_SCALE_CATEGORY_0;
   static const uint32_t mhu32_SCALE_CATEGORY_1;
   static const uint32_t mhu32_SCALE_CATEGORY_2;
   static const uint32_t mhu32_SCALE_CATEGORY_3;
   static const uint32_t mhu32_SCALE_CATEGORY_4;
   static const uint32_t mhu32_SCALE_CATEGORY_5;
   static const uint32_t mhu32_SCALE_CATEGORY_6;
   static const uint32_t mhu32_SCALE_CATEGORY_7;

   bool mq_ErrorIconHovered;
   QString mc_ErrorText;

private:
   //Avoid call
   C_GiNode(const C_GiNode &);
   C_GiNode & operator =(const C_GiNode &) &; //lint !e1511 //we want to hide the base func.

   C_GiNodeBoundary * mpc_Boundary;
   QVector<C_GiPort *> mc_Ports;

   QVector<C_GiLiBusConnector *> mc_Connections;
   C_GiRectPixmap * mpc_ConflictIcon;
   bool mq_Valid;
   bool mq_DrawWhiteFilter;
   int32_t ms32_IconSize;

   void m_InitPorts(void);
   void m_InitConflictIcon(void);
   void m_DetectIconSize(void);

   void m_UpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight, const bool oq_Initial);

   static const float64_t mhaf64_SCALE_MIN_WIDTH_NODE[7];
   static const float64_t mhaf64_SCALE_MIN_HEIGHT_NODE[7];
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
