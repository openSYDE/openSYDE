//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for showing and editing of all J1939 specific PG properties
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEJ1939PGPROPERTIESDIALOG_HPP
#define C_SDBUEJ1939PGPROPERTIESDIALOG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"
#include "C_OscCanUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueJ1939PgPropertiesDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueJ1939PgPropertiesDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueJ1939PgPropertiesDialog(const uint32_t ou32_CanId,
                                           stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                           const QString & orc_MessageName);
   ~C_SdBueJ1939PgPropertiesDialog(void) override;

   void InitStaticNames(void) const;

   uint32_t GetNewCanId(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdBueJ1939PgPropertiesDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   stw::opensyde_core::C_OscCanUtilJ1939PgInfo mc_PgInfo;
   QString mc_MessageName;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_InitPgInfo(void) const;
   void m_SetPgInfo(stw::opensyde_core::C_OscCanUtilJ1939PgInfo & orc_PgInfo) const;
   bool m_HasDestinationAddress(void) const;
   void m_PgnValueChanged(void);
   void m_HandleDestinationSectionVisibility(const bool oq_HasDestinationAddress);

   //Avoid call
   C_SdBueJ1939PgPropertiesDialog(const C_SdBueJ1939PgPropertiesDialog &);
   C_SdBueJ1939PgPropertiesDialog & operator =(const C_SdBueJ1939PgPropertiesDialog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
