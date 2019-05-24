//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom error dialog (header)

   Custom error dialog (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIERROR_H
#define C_OGEWIERROR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QString>
#include <QWidget>
#include <QDialog>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_OgeWiError;
}

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiError :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiError(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeWiError();

   void InitStaticNames(void) const;
   void SetErrorDescription(const QString & orc_Description) const;
   void SaveParentDialog(QDialog * const opc_ParentDialog);

private:
   Ui::C_OgeWiError * mpc_Ui;
   QDialog * mpc_ParentDialog;

   void m_OkClicked(void);
   //Avoid call
   C_OgeWiError(const C_OgeWiError &);
   C_OgeWiError & operator =(const C_OgeWiError &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
