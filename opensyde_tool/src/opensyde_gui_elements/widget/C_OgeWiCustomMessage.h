//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog window for error, information, question and warning.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.04.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWICUSTOMMESSAGE_H
#define C_OGEWICUSTOMMESSAGE_H

/* -- Includes ------------------------------------------------------------- */
#include <QDialog>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_OgeWiCustomMessage;
}

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_OgeWiCustomMessage :
   public QDialog
{
   Q_OBJECT

public:
   enum E_Type
   {
      eERROR = 0,
      eINFORMATION = 1,
      eQUESTION = 2,
      eWARNING = 3
   };

   explicit C_OgeWiCustomMessage(QWidget * const opc_Parent = NULL,
                                 const E_Type & ore_MessageType = E_Type::eINFORMATION,
                                 const QString & orc_Description = "");
   ~C_OgeWiCustomMessage(void);

   enum E_Outputs
   {
      eINVALID = -1,
      eNO = 0,
      eOK = 1,
      eCANCEL = 2,
      //lint -e{849}  'eYES' has same enumerator value '1' as enumerator 'eOK'  -> eOK and eYES are synonyms
      eYES = eOK
   };

   void HandleMousePressEvent(const QMouseEvent * const opc_Event);
   void HandleMouseReleaseEvent(QMouseEvent * const opc_Event);
   void HandleMouseMoveEvent(const QMouseEvent * const opc_Event);

   C_OgeWiCustomMessage::E_Outputs Execute(void);

   // equivalence to QMessageBox::setType
   void SetType(const E_Type & ore_MessageType);

   // equivalence to QMessageBox::setText, but we have 3 text fields (instead of one)
   void SetHeading(const QString & orc_Text);
   void SetDescription(const QString & orc_Text);
   void SetDetails(const QString & orc_Text);

   void SetOKButtonText(const QString & orc_Text) const;
   void SetNOButtonText(const QString & orc_Text) const;
   void SetCancelButtonText(const QString & orc_Text);
   void ShowCancelButton(void);

   void SetCustomMinWidth(const stw_types::sint32 & ors32_MinWidth) const;
   void SetCustomMinHeight(const stw_types::sint32 & ors32_MinHeight) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_OgeWiCustomMessage * mpc_Ui;
   bool mq_Pressed;   ///< Indicator for mouse currently being pressed to allow specific actions
   QPoint mc_LastPos; ///< Last known position of this widget

   bool mq_ShowCancelButton;

   QString mc_PathExpandIcon;
   QString mc_PathMinimizeIcon;

   QPixmap mc_PixmapIcon;
   QString mc_Style;
   QString mc_TypeString;
   E_Type me_MessageType;
   QString mc_Heading;
   QString mc_Description;
   QString mc_Details;

   E_Outputs me_Output;

   void m_SetText(const QString & orc_Heading, const QString & orc_Description, const QString & orc_Details);
   void m_InitButtons(void);
   void m_SetMessageType(const E_Type & ore_MessageType);
   void m_ExpandCollapseDetails(void) const;
   void m_OKClicked(void);
   void m_NOClicked(void);
   void m_CancelClicked(void);
   void m_UpdateLabelSize(void);

   // Avoid call
   C_OgeWiCustomMessage(const C_OgeWiCustomMessage &);
   C_OgeWiCustomMessage & operator =(const C_OgeWiCustomMessage &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} // end of namespace
#endif // C_OGEWICUSTOMMESSAGE_H
