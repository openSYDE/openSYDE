//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard pie chart properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEPIECHART_H
#define C_SYVDAPEPIECHART_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_SyvDaPeBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPePieChart;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPePieChart :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPePieChart(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode);
   ~C_SyvDaPePieChart(void);

   void InitStaticNames(void) const;

   bool GetShowValue(void) const;
   bool GetShowUnit(void) const;

   void SetShowValue(const bool oq_Value) const;
   void SetShowUnit(const bool oq_Value) const;

private:
   //Avoid call
   C_SyvDaPePieChart(const C_SyvDaPePieChart &);
   C_SyvDaPePieChart & operator =(const C_SyvDaPePieChart &);

   void m_UpdatePreview(void);

   Ui::C_SyvDaPePieChart * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   C_SyvDaPeBase & mrc_ParentDialog;
   const bool mq_DarkMode;
};
}

#endif
