//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tool tip window (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOOLTIP_H
#define C_NAGTOOLTIP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QPointer>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagToolTip;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

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
   ~C_NagToolTip() override;
   void SetHeading(const QString & orc_Heading) const;
   void SetContent(const QString & orc_Content) const;
   void SetType(const E_Type oe_Type) const;
   void DoMove(const QPoint & orc_GlobalPos);
   static bool h_HideAll(void);

protected:
   void showEvent(QShowEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;

private:
   //Avoid call
   C_NagToolTip(const C_NagToolTip &);
   C_NagToolTip & operator =(const C_NagToolTip &) &;

   Ui::C_NagToolTip * mpc_Ui;
   static std::vector<QPointer<C_NagToolTip> > mhc_ExistingToolTips; ///< All known existing tooltips
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif // C_NAGTOOLTIP_H
