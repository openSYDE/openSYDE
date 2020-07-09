//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Informational Line / Arrow for user (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIARROW_H
#define C_GILIARROW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.h"
#include "C_GiLiLineGroup.h"
#include "C_GiUnique.h"
#include "C_PuiBsLineArrow.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiArrow :
   public C_GiLiLineGroup,
   public stw_opensyde_gui_logic::C_GiUnique
{
public:
   C_GiBiArrow(const stw_types::uint64 & oru64_ID = 0, const std::vector<QPointF> * const opc_Points = NULL,
               QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiBiArrow(void);
   virtual QPainterPath shape(void) const override;
   virtual void SetWidth(const stw_types::sint32 & ors32_Width);
   virtual stw_types::sintn type() const override;
   bool OpenStyleDialog(const bool oq_DarkMode);
   virtual void CopyStyle(const QGraphicsItem * const opc_GuidelineItem);
   void ApplyStyle(const QColor & orc_LineColor, const stw_types::sintn osn_Width,
                   const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType oe_LineType,
                   const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType oe_StartArrow,
                   const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType oe_EndArrow);
   stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType GetStartArrowHeadType() const;
   stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType GetEndArrowHeadType() const;
   stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType GetLineType() const;
   void SetStartArrowHeadType(const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_New);
   void SetEndArrowHeadType(const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_New);
   void SetLineType(const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType & ore_New);
   static bool h_HasOffsetInteractionPoint(const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_Type);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   //lint -restore

   void m_GenerateArrows(void);
   void m_LoadFromData(const stw_opensyde_gui_logic::C_PuiBsLineArrow & orc_Data);
   void m_UpdateData(stw_opensyde_gui_logic::C_PuiBsLineArrow & orc_Data) const;

   stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType me_StartArrowHeadType;
   stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType me_EndArrowHeadType;
   stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType me_LineType;

private:
   //Avoid call
   C_GiBiArrow(const C_GiBiArrow &);
   C_GiBiArrow & operator =(const C_GiBiArrow &);
   C_GiBiArrow();

   void m_GenerateArrow(QGraphicsPathItem * const opc_ArrowItem,
                        const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_ArrowHeadType,
                        const C_GiLiLineConnection * const opc_Conn, const bool & orq_Start);

   QGraphicsPathItem * mpc_ArrowHeadStart;
   QGraphicsPathItem * mpc_ArrowHeadEnd;
   stw_types::float64 mf64_ArrowLength;
   stw_types::float64 mf64_ArrowHeight;
   static const stw_types::float64 mhf64_ShapeOffsetFactor;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
