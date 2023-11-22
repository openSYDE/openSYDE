//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for navigation bar tab (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWINAVIGATIONTAB_HPP
#define C_OGEWINAVIGATIONTAB_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPushButton>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_OgeWiNavigationTab;
}

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiNavigationTab :
   public QPushButton
{
   Q_OBJECT

public:
   explicit C_OgeWiNavigationTab(QWidget * const opc_Parent = NULL);
   ~C_OgeWiNavigationTab(void) override;

   void SetEnabled(const bool oq_Enabled);
   void SetIncludeLeftBorder(const bool oq_Include);
   void SetActive(const bool oq_Active);
   void SetIconSvg(const QString & orc_IconSvg, const QString & orc_IconSvgDisabled);
   void SetText(const QString & orc_NewText) const;

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

private:
   Ui::C_OgeWiNavigationTab * mpc_Ui;
   bool mq_Active;
   QString mc_IconEnabled;
   QString mc_IconDisabled;

   //Avoid call
   C_OgeWiNavigationTab(const C_OgeWiNavigationTab &);
   C_OgeWiNavigationTab & operator =(const C_OgeWiNavigationTab &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
