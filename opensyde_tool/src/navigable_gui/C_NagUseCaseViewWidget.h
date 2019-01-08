//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing use case specific widgets based on class C_NagUseCaseWidget

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

#ifndef C_NAGUSECASEVIEWWIDGET_H
#define C_NAGUSECASEVIEWWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "stwtypes.h"

#include "C_NagUseCaseWidget.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_NagUseCaseViewWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_NagUseCaseViewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagUseCaseViewWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_NagUseCaseViewWidget();

   void SetUseCaseWidget(C_NagUseCaseWidget * const opc_Widget, const stw_types::sint32 os32_Mode,
                         const stw_types::sint32 os32_SubMode = 0, const QString & orc_ItemName = "",
                         const QString & orc_ItemSubSubName = "");
   void RemoveUseCaseWidget(void);

   void UpdateUseCaseWidget(const stw_types::sint32 os32_SubMode, const QString & orc_ItemName,
                            const QString & orc_ItemSubName);
   void UpdateItemName(const QString & orc_ItemName, const QString & orc_ItemSubName);

   void InitText(void) const;
   void InitBackground(void);

   void SetPushButtonIcon(const QString & orc_IconPath) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigPushButtonIconPressed(void);

private:
   void m_OnPushButtonIconPress(void);
   void m_HandleSubSections(void) const;

   //Avoid call
   C_NagUseCaseViewWidget(const C_NagUseCaseViewWidget &);
   C_NagUseCaseViewWidget & operator =(const C_NagUseCaseViewWidget &);

   Ui::C_NagUseCaseViewWidget * mpc_Ui;
   C_NagUseCaseWidget * mpc_Widget;
   stw_types::sint32 ms32_Mode;
   stw_types::sint32 ms32_Submode;
   QString mc_ItemName;
   QString mc_ItemSubSubName;
   static const QString mhc_SVGIconPath;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
