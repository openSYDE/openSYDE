//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tool tip window with an image
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOOLTIPWITHIMAGE_H
#define C_NAGTOOLTIPWITHIMAGE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QPointer>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_NagToolTipWithImage;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagToolTipWithImage :
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

   explicit C_NagToolTipWithImage(const QString & orc_Heading = "", const QString & orc_Content = "",
                                  QWidget * const opc_Parent = NULL);
   ~C_NagToolTipWithImage(void) override;

   void SetHeading(const QString & orc_Heading) const;
   void SetContent(const QString & orc_Content) const;
   void SetType(const E_Type oe_Type) const;
   void DoMove(const QPoint & orc_GlobalPos);
   void HideImage() const;
   void ShowImage(const QString & orc_ImagePath, const QString & orc_ImageText = "") const;
   static bool h_HideAll(void);

protected:
   void showEvent(QShowEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;

private:
   //Avoid call
   C_NagToolTipWithImage(const C_NagToolTipWithImage &);
   C_NagToolTipWithImage & operator =(const C_NagToolTipWithImage &) &;

   Ui::C_NagToolTipWithImage * mpc_Ui;
   static std::vector<QPointer<C_NagToolTipWithImage> > mhc_ExistingToolTips; ///< All known existing tooltips

   static const uint16_t mhu16_IMAGE_WIDTH;
   static const QString mhc_DEFAULT_IMAGE;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
