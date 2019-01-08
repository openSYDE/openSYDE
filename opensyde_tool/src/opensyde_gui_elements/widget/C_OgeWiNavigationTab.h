//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for navigation bar tab (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWINAVIGATIONTAB_H
#define C_OGEWINAVIGATIONTAB_H

/* -- Includes ------------------------------------------------------------- */
#include <QPushButton>

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_OgeWiNavigationTab;
}

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeWiNavigationTab :
   public QPushButton
{
   Q_OBJECT

public:
   explicit C_OgeWiNavigationTab(QWidget * const opc_Parent = NULL);
   ~C_OgeWiNavigationTab(void);

   void SetIncludeLeftBorder(const bool oq_Include);
   void SetActive(const bool oq_Active);
   void SetIconSvg(const QString & orc_IconSvg) const;
   void SetText(const QString & orc_NewText) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_OgeWiNavigationTab * mpc_Ui;
   bool mq_Active;

   //Avoid call
   C_OgeWiNavigationTab(const C_OgeWiNavigationTab &);
   C_OgeWiNavigationTab & operator =(const C_OgeWiNavigationTab &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
