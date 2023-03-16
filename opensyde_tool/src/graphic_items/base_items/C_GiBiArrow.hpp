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

#include "stwtypes.hpp"
#include "C_GiLiLineGroup.hpp"
#include "C_GiUnique.hpp"
#include "C_PuiBsLineArrow.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiArrow :
   public C_GiLiLineGroup,
   public stw::opensyde_gui_logic::C_GiUnique
{
public:
   C_GiBiArrow(const uint64_t & oru64_Id = 0, const std::vector<QPointF> * const opc_Points = NULL,
               QGraphicsItem * const opc_Parent = NULL);
   ~C_GiBiArrow(void) override;
   QPainterPath shape(void) const override;
   void SetWidth(const int32_t & ors32_Width) override;
   int32_t type() const override;
   bool OpenStyleDialog(const bool oq_DarkMode);
   void CopyStyle(const QGraphicsItem * const opc_GuidelineItem) override;
   void ApplyStyle(const QColor & orc_LineColor, const int32_t os32_Width,
                   const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType oe_LineType,
                   const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType oe_StartArrow,
                   const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType oe_EndArrow);
   stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType GetStartArrowHeadType() const;
   stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType GetEndArrowHeadType() const;
   stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType GetLineType() const;
   void SetStartArrowHeadType(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_New);
   void SetEndArrowHeadType(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_New);
   void SetLineType(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType & ore_New);
   static bool h_HasOffsetInteractionPoint(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_Type);

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

   void m_GenerateArrows(void);
   void m_LoadFromData(const stw::opensyde_gui_logic::C_PuiBsLineArrow & orc_Data);
   void m_UpdateData(stw::opensyde_gui_logic::C_PuiBsLineArrow & orc_Data) const;

   stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType me_StartArrowHeadType;
   stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType me_EndArrowHeadType;
   stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType me_LineType;

private:
   //Avoid call
   C_GiBiArrow(const C_GiBiArrow &);
   C_GiBiArrow & operator =(const C_GiBiArrow &) &; //lint !e1511 //we want to hide the base func.
   C_GiBiArrow();

   void m_GenerateArrow(QGraphicsPathItem * const opc_ArrowItem,
                        const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType & ore_ArrowHeadType,
                        const C_GiLiLineConnection * const opc_Conn, const bool & orq_Start);

   QGraphicsPathItem * mpc_ArrowHeadStart;
   QGraphicsPathItem * mpc_ArrowHeadEnd;
   float64_t mf64_ArrowLength;
   float64_t mf64_ArrowHeight;
   static const float64_t mhf64_SHAPE_OFFSET_FACTOR;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
