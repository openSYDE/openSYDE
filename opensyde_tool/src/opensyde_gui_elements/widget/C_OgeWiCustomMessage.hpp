//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog window for error, information, question and warning.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWICUSTOMMESSAGE_H
#define C_OGEWICUSTOMMESSAGE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QDialog>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_OgeWiCustomMessage;
}

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
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
   ~C_OgeWiCustomMessage(void) override;

   enum E_Outputs
   {
      eNO = 0,
      eOK = 1,
      eCANCEL = 2,
      //lint -e{849}  'eYES' has same enumerator value '1' as enumerator 'eOK'  -> eOK and eYES are synonyms
      eYES = eOK
   };

   void HandleMousePressEvent(const QMouseEvent * const opc_Event);
   void HandleMouseReleaseEvent(const QMouseEvent * const opc_Event);
   void HandleMouseMoveEvent(const QMouseEvent * const opc_Event);

   C_OgeWiCustomMessage::E_Outputs Execute(void);

   // equivalence to QMessageBox::setType
   void SetType(const E_Type & ore_MessageType);

   // equivalence to QMessageBox::setText, but we have 3 text fields (instead of one)
   void SetHeading(const QString & orc_Text);
   void SetDescription(const QString & orc_Text);
   void SetDetails(const QString & orc_Text);

   void SetOkButtonText(const QString & orc_Text) const;
   void SetNoButtonText(const QString & orc_Text) const;
   void SetCancelButtonText(const QString & orc_Text);
   void ShowCancelButton(void);

   void SetCustomMinWidth(const int32_t & ors32_MinWidth) const;
   void SetCustomMinHeight(const int32_t & ors32_MinHeight, const int32_t & ors32_MaxHeight);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_OgeWiCustomMessage * mpc_Ui;
   bool mq_Pressed;   ///< Indicator for mouse currently being pressed to allow specific actions
   QPoint mc_LastPos; ///< Last known position of this widget

   bool mq_ShowCancelButton;

   QPixmap mc_PixmapIcon;
   QString mc_Style;
   QString mc_TypeString;
   E_Type me_MessageType;
   QString mc_Heading;
   QString mc_Description;
   QString mc_Details;
   int32_t ms32_MinHeight;
   int32_t ms32_MaxHeight;

   E_Outputs me_Output;

   void m_SetText(const QString & orc_Heading, const QString & orc_Description, const QString & orc_Details);
   void m_InitButtons(void);
   void m_SetMessageType(const E_Type & ore_MessageType);
   void m_ExpandCollapseDetails(const bool & orq_Expand) const;
   void m_OkClicked(void);
   void m_NoClicked(void);
   void m_CancelClicked(void);
   void m_Size(void) const;

   // Avoid call
   C_OgeWiCustomMessage(const C_OgeWiCustomMessage &);
   C_OgeWiCustomMessage & operator =(const C_OgeWiCustomMessage &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace
#endif // C_OGEWICUSTOMMESSAGE_H
