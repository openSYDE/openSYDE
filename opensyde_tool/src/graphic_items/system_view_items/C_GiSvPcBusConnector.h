//-----------------------------------------------------------------------------
/*!
   \file
   \brief       PC bus connection (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GIPCBUSCONNECTOR_H
#define C_GIPCBUSCONNECTOR_H

/* -- Includes ------------------------------------------------------------- */
#include "C_GiLiBusConnectorBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSvPcBusConnector :
   public C_GiLiBusConnectorBase
{
public:
   C_GiSvPcBusConnector(const stw_types::uint64 ou64_UniqueID, const stw_types::uint32 ou32_ViewIndex,
                        const std::vector<QPointF> & orc_InteractionPoints, const C_GiLiBus * const opc_BusItem,
                        C_GiBiConnectableItem * const opc_GenericItem);
   virtual stw_types::sintn type(void) const override;
   virtual void GenerateHint(void);
   //lint -e{1735} Suppression, because default parameters are identical
   virtual void SetAnimated(const bool oq_Active, const bool oq_Inverse = false, const bool oq_SpeedUp = false,
                            const QPolygonF oc_Polygon = QPolygonF(), const bool oq_ShowOrignalLine = true) override;
   void UpdateData(void) const;
   void SetEditMode(const bool oq_Active);

protected:
   virtual void m_OnIterationGenericInteractionPointMove(QGraphicsItem * const opc_HighestParentItem,
                                                         const QPointF & orc_CurPos, bool & orq_RestoreMouseCursor);
   virtual bool m_OnGenericInteractionPointMouseRelease(const QPointF & orc_ScenePos);
   virtual void m_OnBusChange(const QPointF & orc_ScenePos);
   virtual void m_AdaptStyleToBus(const C_GiLiBus * const opc_Bus);

private:
   const stw_types::uint32 mu32_ViewIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
