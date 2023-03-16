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
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_PuiSvDbDataElementHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItPaArWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItPaArWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItPaArWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                const uint32_t & oru32_ElementIndex,
                                stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget,
                                const bool oq_EcuValues);
   ~C_SyvDaItPaArWidget(void) override;
   void InitStaticNames(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvDaItPaArWidget * const mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_Parent;
   const bool mq_EcuValues;
   const uint32_t mu32_ElementIndex;
   stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const mpc_DataWidget;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   //Avoid call
   C_SyvDaItPaArWidget(const C_SyvDaItPaArWidget &);
   C_SyvDaItPaArWidget & operator =(const C_SyvDaItPaArWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
