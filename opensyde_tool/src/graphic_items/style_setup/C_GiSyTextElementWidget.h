//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for boundary style setup to customize text element optics (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISYTEXTELEMENTWIDGET_H
#define C_GISYTEXTELEMENTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QColor>
#include <QFont>
#include <QLabel>

#include "C_GiSyBaseWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_GiSyTextElementWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyTextElementWidget :
   public QWidget
{
   Q_OBJECT

public:
   C_GiSyTextElementWidget(C_GiSyBaseWidget & orc_Parent);
   ~C_GiSyTextElementWidget();

   void InitStaticNames(void) const;

   QFont GetFontStyle(void) const;
   void SetFontStyle(const QFont & orc_Font);

   QColor GetFontColor(void) const;
   void SetFontColor(const QColor & orc_Value);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;

private:
   //Avoid call
   C_GiSyTextElementWidget(const C_GiSyTextElementWidget &);
   C_GiSyTextElementWidget & operator =(const C_GiSyTextElementWidget &);

   void m_UpdatePreview(void);

   void m_FontColorClicked(void);
   void m_FontStyleUpdated(const QFont & orc_Font);

   Ui::C_GiSyTextElementWidget * mpc_Ui;
   C_GiSyBaseWidget * mpc_ParentDialog;
   QColor mc_FontColor;
   QFont mc_FontStyle;

   QLabel * mpc_LabelColorFont;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
