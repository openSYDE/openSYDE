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

#include "stwtypes.h"

#include "C_GiSyBaseWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_GiSyBoundaryWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyBoundaryWidget :
   public QWidget
{
   Q_OBJECT

public:
   C_GiSyBoundaryWidget(C_GiSyBaseWidget & orc_Parent);
   ~C_GiSyBoundaryWidget();

   void InitStaticNames(void) const;

   stw_types::sintn GetBorderWidth(void) const;
   void SetBorderWidth(const stw_types::sintn osn_Value) const;

   QColor GetBorderColor(void) const;
   void SetBorderColor(const QColor & orc_Value);

   QColor GetBackgroundColor(void) const;
   void SetBackgroundColor(const QColor & orc_Value);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_GiSyBoundaryWidget(const C_GiSyBoundaryWidget &);
   C_GiSyBoundaryWidget & operator =(const C_GiSyBoundaryWidget &);

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
} //end of namespace

#endif
