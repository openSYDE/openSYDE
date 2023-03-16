//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for bus style setup to customize bus optics (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISYLINEWIDGET_H
#define C_GISYLINEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QColor>
#include <QLabel>

#include "stwtypes.hpp"

#include "C_GiSyBaseWidget.hpp"
#include "C_PuiBsLineArrow.hpp"
#include "C_OgeCbxIconOnly.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_GiSyLineWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyLineWidget :
   public QWidget
{
   Q_OBJECT

public:
   enum E_Type
   {
      eCAN_BUS = 0,
      eETHERNET_BUS = 1,
      eLINE = 2
   };

   C_GiSyLineWidget(const E_Type oe_Mode, C_GiSyBaseWidget & orc_Parent);
   ~C_GiSyLineWidget(void) override;

   void InitStaticNames(void) const;

   int32_t GetLineWidth(void) const;
   void SetLineWidth(const int32_t os32_Value);

   QColor GetLineColor(void) const;
   void SetLineColor(const QColor & orc_Value);

   QColor GetInnerLineColor(void) const;
   void SetInnerLineColor(const QColor & orc_Value);

   stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType GetLineStyle(void) const;
   void SetLineStyle(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType oe_Value);

   stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType GetStartArrow(void) const;
   void SetStartArrow(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType oe_Value);

   stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType GetEndArrow(void) const;
   void SetEndArrow(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_ArrowHeadType oe_Value);

protected:
   void showEvent(QShowEvent * const opc_Event) override;

private:
   //Avoid call
   C_GiSyLineWidget(const C_GiSyLineWidget &);
   C_GiSyLineWidget & operator =(const C_GiSyLineWidget &) &;

   void m_UpdatePreview(void);

   void m_ColorClicked(void);
   void m_ColorInnerClicked(void);
   void m_ValueChanged(void);
   static void mh_InitArrow(stw::opensyde_gui_elements::C_OgeCbxIconOnly * const opc_ComboBox,
                            const bool & orq_Rotate = false);

   Ui::C_GiSyLineWidget * mpc_Ui;
   C_GiSyBaseWidget * mpc_ParentDialog;
   const E_Type me_Mode;
   QColor mc_Color;
   QColor mc_InnerLineColor;

   QLabel * mpc_LabelColor;
   QLabel * mpc_LabelColorInner;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
