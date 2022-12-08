//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Properties dialog for dashboard tab properties (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDTABPROPERTIES_H
#define C_SYVDADASHBOARDTABPROPERTIES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

#include "C_OgePopUpDialog.hpp"
#include "C_PuiSvDashboard.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaDashboardTabProperties;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaDashboardTabProperties :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardTabProperties(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                          const QString & orc_Name, const uint32_t ou32_DashboardIndex,
                                          const uint32_t ou32_ViewIndex, const bool oq_NewDashboard = false);
   ~C_SyvDaDashboardTabProperties(void) override;

   void InitStaticNames(void);
   QString GetDashboardTabName() const;
   QString GetDashboardTabComment() const;
   stw::opensyde_gui_logic::C_PuiSvDashboard::E_TabType GetDashboardTabType() const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SyvDaDashboardTabProperties(const C_SyvDaDashboardTabProperties &);
   C_SyvDaDashboardTabProperties & operator =(const C_SyvDaDashboardTabProperties &) &;

   Ui::C_SyvDaDashboardTabProperties * mpc_Ui;
   stw::opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;

   const uint32_t mu32_DashboardIndex;
   const uint32_t mu32_ViewIndex;
   const bool mq_NewDashboard;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   bool m_CheckDashboardTabName(void) const;
   void m_SetCommonDashboardType(const bool oq_IsChecked) const;
   void m_SetChartDashboardType(const bool oq_IsChecked) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
