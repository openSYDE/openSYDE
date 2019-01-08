//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tool tip window (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.10.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_NAGTOOLTIP_H
#define C_NAGTOOLTIP_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include <QPointer>

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_NagToolTip;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_NagToolTip :
   public QWidget
{
   Q_OBJECT

public:
   enum E_Type
   {
      eDEFAULT,
      eWARNING,
      eERROR
   };

   explicit C_NagToolTip(const QString & orc_Heading = "", const QString & orc_Content = "",
                         QWidget * const opc_Parent = NULL);
   ~C_NagToolTip();
   void SetHeading(const QString & orc_Heading) const;
   void SetContent(const QString & orc_Content) const;
   void SetType(const E_Type oe_Type) const;
   void DoMove(const QPoint & orc_GlobalPos);
   static bool h_HideAll(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_NagToolTip(const C_NagToolTip &);
   C_NagToolTip & operator =(const C_NagToolTip &);

   Ui::C_NagToolTip * mpc_Ui;
   static std::vector<QPointer<C_NagToolTip> > mhc_ExistingToolTips; ///< All known existing tooltips
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif // C_NAGTOOLTIP_H
