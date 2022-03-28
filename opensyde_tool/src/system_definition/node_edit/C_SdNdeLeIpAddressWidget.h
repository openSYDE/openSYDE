//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing one single IPv4 address.
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDELEIPADDRESSWIDGET_H
#define C_SDNDELEIPADDRESSWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeLeIpAddressWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeLeIpAddressWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeLeIpAddressWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeLeIpAddressWidget(void);

   void SetIPAddress(const stw_types::uint8 * const opu8_IPAddress);
   std::vector<stw_types::sint32> GetIPAddress(void) const;
   void SetCursorPosition(const stw_types::uint32 ou32_Pos);

   virtual bool eventFilter(QObject * opc_Obj, QEvent * opc_Event) override;

   void SetError(const bool oq_Error) const;

   void SlotTextChanged(const QLineEdit * const opc_LineEdit);

   std::vector<QLineEdit *> GetLineEdits(void);
   bool IsLineEditPartOfWidget(const QLineEdit * const opc_LineEdit) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SignalTextChanged(QLineEdit * opc_LineEdit);

private:
   Ui::C_SdNdeLeIpAddressWidget * mpc_Ui;

   static const stw_types::sintn mhsn_IPV4SIZE = 4;      // IPv4 address with four line edits
   static const stw_types::sintn mhsn_IPV4MAXDIGITS = 3; // max number of digits of one line edit

   QLineEdit * mapc_LineEdit[mhsn_IPV4SIZE];
   QRegularExpressionValidator * mapc_Validator[mhsn_IPV4SIZE];

   void m_MoveNextLineEdit (const stw_types::uint32 ou32_Pos);
   void m_MovePrevLineEdit (const stw_types::uint32 ou32_Pos);

   void m_FocusInSlot(void);
   void m_FocusOutSlot(void);

   void m_RemoveLeadingZeroes(QLineEdit * const opc_LineEdit);

   //Avoid call
   C_SdNdeLeIpAddressWidget(const C_SdNdeLeIpAddressWidget &);
   C_SdNdeLeIpAddressWidget & operator =(const C_SdNdeLeIpAddressWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
