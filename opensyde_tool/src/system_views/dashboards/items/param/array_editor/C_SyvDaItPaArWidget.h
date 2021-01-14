//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data set array edit widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAARWIDGET_H
#define C_SYVDAITPAARWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_PuiSvDbDataElementHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItPaArWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItPaArWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItPaArWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                const stw_types::uint32 & oru32_ElementIndex,
                                stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget,
                                const bool oq_ECUValues);
   ~C_SyvDaItPaArWidget(void);
   void InitStaticNames(void);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvDaItPaArWidget * const mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_Parent;
   const bool mq_ECUValues;
   const stw_types::uint32 mu32_ElementIndex;
   stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const mpc_DataWidget;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   //Avoid call
   C_SyvDaItPaArWidget(const C_SyvDaItPaArWidget &);
   C_SyvDaItPaArWidget & operator =(const C_SyvDaItPaArWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
