//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit for list header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETEDLISTHEADERBASE_H
#define C_OGETEDLISTHEADERBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "C_NagToolTip.h"
#include "C_OgeTedContextMenuBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTedListHeaderBase :
   public C_OgeTedContextMenuBase
{
   Q_OBJECT

public:
   C_OgeTedListHeaderBase(QWidget * const opc_Parent = NULL);
   ~C_OgeTedListHeaderBase(void);

   QString GetFullText(void) const;
   void HideFullText(const bool oq_CheckForNewText = true);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigFocus(const bool oq_Val);
   void SigEditFinished(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void focusInEvent(QFocusEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

private:
   QString mc_CompleteText;
   bool mq_JustGotFocus;
   stw_opensyde_gui::C_NagToolTip * mpc_ToolTip;
   static const QFont mhc_Font;

   void m_ShowFullText(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
