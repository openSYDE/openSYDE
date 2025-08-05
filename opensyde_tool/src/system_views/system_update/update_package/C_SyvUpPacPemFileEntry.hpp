//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Creating PEM File entry
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACPEMFILEENTRY_H
#define C_SYVUPPACPEMFILEENTRY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgeWiWithToolTip.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacPemFileEntry;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacPemFileEntry :
   public stw::opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SyvUpPacPemFileEntry(QWidget * const opc_Parent, const QString & orc_File,
                                   const int32_t os32_CurrentPemFileIndex);
   ~C_SyvUpPacPemFileEntry(void) override;

   QString c_FilePath;
   int32_t s32_CurrentPemFileIndex;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDeleteItemWidget(int32_t os32_CurrentPemFileIndex, bool oq_NeedToUpdateListIndex);

private:
   Ui::C_SyvUpPacPemFileEntry * mpc_Ui;

   void m_LoadPemFileInfo();
   void m_DeleteItem(void);

   //Avoid call
   C_SyvUpPacPemFileEntry(const C_SyvUpPacPemFileEntry &);
   C_SyvUpPacPemFileEntry & operator =(const C_SyvUpPacPemFileEntry &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
