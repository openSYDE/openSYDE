//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for update properties
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAUPPROPERTIES_HPP
#define C_FLAUPPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QIcon>
#include <QVBoxLayout>

#include "C_FlaUpListWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaUpProperties;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaUpProperties :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_FlaUpProperties(QWidget * const opc_Parent = NULL);
   ~C_FlaUpProperties(void) override;

   void SaveUserSettings(void) const;
   QStringList GetHexFilePaths(void) const;
   QStringList GetHexFileDeviceNames(void) const;
   bool AreAllFilesValid(void) const;
   void EnableSettings(const bool oq_Enabled);

private:
   Ui::C_FlaUpProperties * mpc_Ui;
   void m_UpdateLabelTitleWithFileCounter(void) const;
   QVBoxLayout * mpc_ScrollLayout;
   C_FlaUpListWidget * mpc_ListWidget;

   //Avoid call
   C_FlaUpProperties(const C_FlaUpProperties &);
   C_FlaUpProperties & operator =(const C_FlaUpProperties &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
