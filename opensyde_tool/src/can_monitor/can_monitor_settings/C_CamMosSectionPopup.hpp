//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup widget for displaying settings widgets in collapsed state (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSSECTIONPOPUP_HPP
#define C_CAMMOSSECTIONPOPUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVBoxLayout>
#include "C_OgeWiOnlyBackground.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosSectionPopup :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   C_CamMosSectionPopup(QWidget * const opc_Parent = NULL);
   ~C_CamMosSectionPopup(void) override;

   void Show(const QPoint & orc_Pos);
   void SetWidget(QWidget * const opc_Widget);
   void DoMove(const QPoint & orc_Pos);

private:
   QVBoxLayout * mpc_Layout;
   //Avoid call
   C_CamMosSectionPopup(const C_CamMosSectionPopup &);
   C_CamMosSectionPopup & operator =(const C_CamMosSectionPopup &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
