//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for boundary style setup to customize boundary optics (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISYBOUNDARYWIDGET_H
#define C_GISYBOUNDARYWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QColor>
#include <QLabel>

#include "stwtypes.hpp"

#include "C_GiSyBaseWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_GiSyBoundaryWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyBoundaryWidget :
   public QWidget
{
   Q_OBJECT

public:
   C_GiSyBoundaryWidget(C_GiSyBaseWidget & orc_Parent);
   ~C_GiSyBoundaryWidget() override;

   void InitStaticNames(void) const;

   int32_t GetBorderWidth(void) const;
   void SetBorderWidth(const int32_t os32_Value) const;

   QColor GetBorderColor(void) const;
   void SetBorderColor(const QColor & orc_Value);

   QColor GetBackgroundColor(void) const;
   void SetBackgroundColor(const QColor & orc_Value);

protected:
   void showEvent(QShowEvent * const opc_Event) override;

private:
   //Avoid call
   C_GiSyBoundaryWidget(const C_GiSyBoundaryWidget &);
   C_GiSyBoundaryWidget & operator =(const C_GiSyBoundaryWidget &) &;

   void m_UpdatePreview(void);

   void m_BorderColorClicked(void);
   void m_BackgroundColorClicked(void);
   void m_WidthChanged(void);

   Ui::C_GiSyBoundaryWidget * mpc_Ui;
   C_GiSyBaseWidget * mpc_ParentDialog;
   QColor mc_BorderColor;
   QColor mc_BackgroundColor;

   QLabel * mpc_LabelBackground;
   QLabel * mpc_LabelBorder;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
