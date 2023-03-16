//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for bus specific lines (header)

   Base class for busses (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIBUS_HPP
#define C_GILIBUS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QString>

#include "stwtypes.hpp"

#include "C_PuiSdDataElement.hpp"
#include "C_GiBiCustomToolTip.hpp"
#include "C_GiLiLineGroup.hpp"
#include "C_GiUnique.hpp"
#include "C_OscSystemBus.hpp"
#include "C_GiTextElementBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiBus :
   public C_GiLiLineGroup,
   public stw::opensyde_gui_logic::C_PuiSdDataElement,
   public C_GiBiCustomToolTip,
   public stw::opensyde_gui_logic::C_GiUnique
{
   Q_OBJECT

public:
   C_GiLiBus(const int32_t & ors32_Index, const uint64_t & oru64_Id, C_GiTextElementBus * const opc_TextElementName,
             const bool oq_DoErrorCheck, const std::vector<QPointF> * const opc_Points = NULL,
             const bool & orq_MiddleLine = false, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiLiBus() override;

   int32_t type() const override;

   virtual bool OpenStyleDialog(void) = 0;
   QString GetName(void) const;
   QString GetBitrate(const bool oq_WithComma) const;

   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;
   virtual stw::opensyde_core::C_OscSystemBus::E_Type GetType() const = 0;
   void GenerateHint(void) override;
   void CopyStyle(const QGraphicsItem * const opc_GuidelineItem) override;

   //GI base
   void SetZetValueCustom(const float64_t of64_ZetValue) override;

   void SetDisabledLook(const bool oq_Disabled) override;
   void ApplyStyle(const QColor & orc_LineColor, const int32_t os32_Width);
   virtual void CheckBusForChanges(void);
   void SetResizing(const bool & orq_ResizeActive) override;
   C_GiTextElementBus * GetTextElementBus(void);

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;

   C_GiTextElementBus * mpc_TextElementName;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigHideToolTip(void);

private:
   //Avoid call
   C_GiLiBus(const C_GiLiBus &);
   C_GiLiBus & operator =(const C_GiLiBus &) &; //lint !e1511 //we want to hide the base func.

   void m_BusWasMoved(const int32_t & ors32_LineIndex, const QPointF & orc_PositionDifference);
   void m_UpdateTextElementName(void);
   void m_UnregisterTextElement(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
