//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Properties dialog for dashboard tab properties (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SyvDaDashboardTabProperties_H
#define C_SyvDaDashboardTabProperties_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"

#include "C_OgePopUpDialog.h"
#include "C_PuiSvDashboard.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaDashboardTabProperties;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaDashboardTabProperties :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardTabProperties(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                          const QString & orc_Name, const stw_types::uint32 ou32_DashboardIndex,
                                          const stw_types::uint32 ou32_ViewIndex, const bool oq_NewDashboard = false);
   virtual ~C_SyvDaDashboardTabProperties(void);

   void InitStaticNames(void);
   QString GetDashboardTabName() const;
   QString GetDashboardTabComment() const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   //Avoid call
   C_SyvDaDashboardTabProperties(const C_SyvDaDashboardTabProperties &);
   C_SyvDaDashboardTabProperties & operator =(const C_SyvDaDashboardTabProperties &);

   Ui::C_SyvDaDashboardTabProperties * mpc_Ui;
   stw_opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;

   const stw_types::uint32 mu32_DashboardIndex;
   const stw_types::uint32 mu32_ViewIndex;
   const bool mq_NewDashboard;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   bool m_CheckDashboardTabName(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
